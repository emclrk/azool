#ifndef GAMEBOARD_H_
#define GAMEBOARD_H_
#include <vector>
#include <ostream>
#include <cstring>
#include <random>
#include "tile_utils.h"

class GameBoard {
public:
  struct Factory {
    Factory() : tileCounts() {
      memset(tileCounts, 0, azool::NUMCOLORS*sizeof(int));
    }
    int tileCounts[azool::NUMCOLORS];
  };  // struct Factory

  GameBoard(int nPlayers=2);
  friend std::ostream& operator<<(std::ostream& out, const GameBoard& board);
  bool validFactoryRequest(int factoryIdx, azool::TileColor color);
  bool takeTilesFromFactory(int factoryIdx, azool::TileColor color, int& numTiles);
  bool takeTilesFromPool(azool::TileColor color, int& numTiles, bool& poolPenalty);
  void returnTilesToBag(int numTiles, azool::TileColor color);
  void dealTiles();
  int numFactories() {
    return tileFactories.size();
  }
  bool endOfRound() {
    // round ends when the pool and tile factories are empty
    for (int ii = 0; ii < azool::NUMCOLORS; ++ii) {
      if (pool[ii] > 0) return false;
    }
    return tileFactories.empty();
  }
private:
  GameBoard(const GameBoard&) = delete;
  GameBoard operator=(const GameBoard&) = delete;
  void resetBoard();
  // - vector of factories?
  std::vector<Factory> tileFactories;
  int maxNumFactories;
  int pool[azool::NUMCOLORS];  // stores the count of each color currently in the pool; initialize to 0s
  bool whiteTileInPool;  // initialize to true
  std::vector<azool::TileColor> tileBag;  // initialize to 20 of each color
  bool lastRound; // initialize to false
  std::default_random_engine rng;
};
#endif // GAMEBOARD_H_
