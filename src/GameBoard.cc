#include "GameBoard.h"
#include <algorithm>
#include <chrono>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

namespace pt = boost::property_tree;

GameBoard::GameBoard(int numPlayers) :
  tileFactories(),
  myNumPlayers(numPlayers),
  maxNumFactories(numPlayers*2+1),
  pool(),
  whiteTileInPool(true),
  tileBag(),
  lastRound(false),
  rng(std::default_random_engine(
        std::chrono::system_clock::now().time_since_epoch().count())) {
  resetBoard();
}  // GameBoard::GameBoard

std::ostream& operator<<(std::ostream& out, const GameBoard& board) {
  // user will input 1-indexed value, even though we 0-index internally
  int factCt = 1;
  out << "Factories:\n";
  for (auto factory : board.tileFactories) {
    out << factCt++ << " ";
    for (int ii = 0; ii < azool::NUMCOLORS; ++ii) {
      for (int jj = 0; jj < factory.tileCounts[ii]; ++jj) {
        out << azool::TileColorStrings[ii] << ",";
      }
    }
    out << "\n";
  }
  out << "\nPOOL:\n";
  if (board.whiteTileInPool) {
    out << "[-1]\n";
  }
  for (int ii = 0; ii < azool::NUMCOLORS; ++ii) {
    out << azool::TileColorStrings[ii] << " x " << board.pool[ii] << "\n";
  }
  return out;
}

bool GameBoard::validFactoryRequest(int factoryIdx, azool::TileColor color) const {
  // check if color exists on specified factory
  bool retVal = factoryIdx < tileFactories.size() and
                factoryIdx > -1 and
                tileFactories[factoryIdx].tileCounts[color] > 0;
  return retVal;
}

bool GameBoard::takeTilesFromFactory(int factoryIdx, azool::TileColor color, int& numTiles) {
  // clear factory
  // add other tiles to pool
  // return # of tiles given to player
  // if invalid return false
  if (!validFactoryRequest(factoryIdx, color)) {
    return false;
  }
  numTiles = tileFactories[factoryIdx].tileCounts[color];
  // zero out the tiles of this color before adding the rest to the pool
  tileFactories[factoryIdx].tileCounts[color] = 0;
  for (int ii = 0; ii < azool::NUMCOLORS; ++ii) {
    pool[ii] += tileFactories[factoryIdx].tileCounts[ii];
  }
  tileFactories.erase(tileFactories.begin() + factoryIdx);
  return true;
}
bool GameBoard::takeTilesFromPool(azool::TileColor color, int& numTiles, bool& poolPenalty) {
  numTiles = pool[color];  // # of tiles given to player
  if (numTiles == 0) {
    // invalid - no tiles of the given color are in the pool
    return false;
  }
  // zero color count in pool
  pool[color] = 0;
  poolPenalty = whiteTileInPool;
  whiteTileInPool = false;
  return true;
}  // GameBoard::takeTilesFromPool
void GameBoard::returnTilesToBag(int numTiles, azool::TileColor color) {
  for (int ii = 0; ii < numTiles; ++ii) {
    tileBag.emplace_back(color);
  }
}  // GameBoard::returnTilesToBag

// random shuffle then read from the beginning of the vector?
void GameBoard::dealTiles() {
  tileFactories.clear();
  whiteTileInPool = true;
  int numFactories = std::min(static_cast<int>(tileBag.size()) / 4, maxNumFactories);
  if (tileBag.size() < 4*numFactories) {
    numFactories++;
  }
  std::shuffle(tileBag.begin(), tileBag.end(),  rng);
  auto itr = tileBag.begin();
  for (int ii = 0; ii < numFactories and itr != tileBag.end(); ++ii) {
    Factory fact;
    for (int jj = 0; jj < 4; jj++) {
      fact.tileCounts[*itr]++;
      itr++;
      if (itr == tileBag.end()) {
        break;
      }  // last factory may have less than 4 tiles
    }
    tileFactories.push_back(fact);
  }
  tileBag.erase(tileBag.begin(), itr);
}  // GameBoard::dealTiles

pt::ptree GameBoard::serializeBoard() const {
  pt::ptree outTree;
  // factories
  pt::ptree factTree;
  int factCt = 1;
  for (auto factory : tileFactories) {
    std::stringstream factSS;
    for (int ii = 0; ii < azool::NUMCOLORS; ++ii) {
      for (int jj = 0; jj < factory.tileCounts[ii]; ++jj) {
        factSS << azool::TileColorStrings[ii] << ",";
      }
    }
    factTree.put(std::to_string(factCt), factSS.str());
    factCt++;
  }
  outTree.add_child("factories", factTree);
  outTree.put("num_factories", tileFactories.size());
  // pool
  pt::ptree poolTree;
  int numTilesInPool = 0;
  for (int ii = 0; ii < azool::NUMCOLORS; ++ii) {
    poolTree.put(azool::TileColorStrings[ii], pool[ii]);
    numTilesInPool += pool[ii];
  }
  if (whiteTileInPool) {
    poolTree.put("penalty", -1);
  }
  else {
    poolTree.put("penalty", 0);
  }
  outTree.add_child("pool", poolTree);
  outTree.put("num_tiles_in_pool", numTilesInPool);
  outTree.put("end_of_round", endOfRound());
  return outTree;
}  // GameBoard::serializeBoard

std::string GameBoard::handleRequest(const std::string& instring) {
  std::stringstream iss(instring);
  pt::ptree inTree;
  pt::read_json(iss, inTree);
  pt::ptree outTree;
  std::string req_type = inTree.get<std::string>("req_type");
  // check type of request; check that required parameters are present; etc
  // return: json with results, status, error msg if any
  if (req_type.compare(azool::REQ_TYPE_DRAW_FROM_FACTORY) == 0) {
    int idx = inTree.get<int>("factory_idx", -1);
    int numTiles = 0;
    azool::TileColor color = static_cast<azool::TileColor>(inTree.get<int>("tile_color"));
    bool success = takeTilesFromFactory(idx, color, numTiles);
    outTree.put("status", success ? "success" : "fail");
    outTree.put("success", success);
    outTree.put("num_tiles_returned", numTiles);
    if (!success) {
      outTree.put("error_msg", "INVALID FACTORY REQUEST");
    }
  }
  else if (req_type.compare(azool::REQ_TYPE_DRAW_FROM_POOL) == 0) {
    azool::TileColor color = static_cast<azool::TileColor>(inTree.get<int>("tile_color"));
    int numTiles = 0;
    bool penalty = false;
    bool success = takeTilesFromPool(color, numTiles, penalty);
    outTree.put("success", success);
    outTree.put("status", success ? "success" : "fail");
    outTree.put("num_tiles_returned", numTiles);
    outTree.put("pool_penalty", penalty);
    if (!success) {
      outTree.put("error_msg", "INVALID POOL REQUEST");
    }
  }
  else if (req_type.compare(azool::REQ_TYPE_RETURN_TO_BAG) == 0) {
    int numTiles = inTree.get<int>("num_tiles_returned", 0);
    azool::TileColor color = static_cast<azool::TileColor>(inTree.get<int>("tile_color"));
    returnTilesToBag(numTiles, color);
    outTree.put("success", true);  // no reason for this to fail
    outTree.put("status", "success");
  }
  else if (req_type.compare(azool::REQ_TYPE_GET_BOARD) == 0) {
    outTree = serializeBoard();
  }
  else {
    // ERROR, BAD THINGS
    outTree.put("error_msg", "INVALID REQUEST TYPE");
  }
  outTree.put("req_type", req_type);  // include request type in returned data
  std::stringstream oss;
  pt::write_json(oss, outTree);
  std::string output = oss.str();
  return output;
  // send output string over socket
}  // GameBoard::handleRequest

void GameBoard::resetBoard() {
  memset(pool, 0, azool::NUMCOLORS*sizeof(int));
  tileBag.clear();
  tileBag.reserve(azool::NUMCOLORS * 20);
  for (int ii = 0; ii < azool::NUMCOLORS; ++ii) {
    // initialize tile bag to 20 of each color
    for (int jj = 0; jj < 20; jj++) {
      tileBag.emplace_back(static_cast<azool::TileColor>(ii));
    }
  }
  whiteTileInPool = true;
}   // GameBoard::resetBoard
