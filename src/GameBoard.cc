#include "GameBoard.h"
#include <algorithm>
#include <chrono>

GameBoard::GameBoard(int numPlayers) :
  tileFactories(),
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

bool GameBoard::validFactoryRequest(int factoryIdx, azool::TileColor color) {
  // check if color exists on specified factory
  bool retVal = factoryIdx < tileFactories.size() and
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
}
void GameBoard::returnTilesToBag(int numTiles, azool::TileColor color) {
  for (int ii = 0; ii < numTiles; ++ii) {
    tileBag.emplace_back(color);
  }
}

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
}  // GameBoard::dealTiles

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
