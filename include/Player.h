#ifndef PLAYER_H_
#define PLAYER_H_
#include "GameBoard.h"
#include "tile_utils.h"
#include <string>

class Player {
public:
  Player(GameBoard* const board, std::string name = "1");
  void takeTurn();
  bool takeTilesFromFactory(int factoryIdx, azool::TileColor color, int rowIdx);
  bool takeTilesFromPool(azool::TileColor color, int rowIdx);
  bool discardFromFactory(int factoryIdx, azool::TileColor color);
  bool discardFromPool(azool::TileColor color);
  void placeTiles(int rowIdx, azool::TileColor color, int numTiles);
  void endRound(bool& fullRow);
  void finalizeScore();
  int getScore() const {
    return myScore;
  }
  std::string printMyBoard() const;
  bool tookPenalty() const {
    return myTookPoolPenaltyThisRound;
  }
  const std::string getPlayerName() const {
    return myName;
  }

private:
  Player(const Player&) = delete;
  Player operator=(const Player&) = delete;

  bool checkValidMove(azool::TileColor color, int rowIdx) const;
  int scoreTile(int row, int col);

  // first - # of tiles on that row, second - color of tiles on row
  bool myGrid[azool::NUMCOLORS][azool::NUMCOLORS];
  typedef std::pair<int, azool::TileColor> TileRow;
  TileRow myRows[azool::NUMCOLORS];
  GameBoard* const myBoardPtr;
  int myScore;
  int myNumPenaltiesForRound;
  bool myTookPoolPenaltyThisRound;
  const std::vector<int> PenaltyPoints = {0, 1, 2, 3, 5, 7, 10, 13, 15};
  std::string myName;
};  // class Player
#endif  // PLAYER_H_
