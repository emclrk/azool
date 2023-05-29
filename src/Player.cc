#include "Player.h"
#include <iostream>
#include <cstring>

Player::Player(GameBoard* const board, std::string name) :
  myGrid(),
  myRows(),
  myBoardPtr(board),
  myName(name),
  myScore(0),
  myNumPenaltiesForTurn(0),
  myTookPoolPenaltyThisTurn(false) {
    int gridSize = azool::NUMCOLORS * azool::NUMCOLORS;
    memset(myGrid, 0, gridSize*sizeof(bool));
    for (int ii = 0; ii < azool::NUMCOLORS; ++ii) {
      myRows[ii].first = 0;
      myRows[ii].second = azool::NONE;
    }  // initialize rows
  }  // Player::Player

bool Player::checkValidMove(azool::TileColor color, int rowIdx) const {
  // check if valid move
  //  grid doesn't already have this color on that row,
  //  row is either empty or already has the same color
  if (color == azool::NONE) {
    std::cerr << "HEY THIS IS WEIRD -- "
              << "ASKING FOR COLOR = NONE?" << std::endl;
    return false;  // invalid and also probably shouldn't happen
  }
  int colIdx = (5 + static_cast<int>(color) - rowIdx) % 5;
  if (myGrid[rowIdx][colIdx]) {
    return false;  // already have that color on this row
  }
  if (!(myRows[rowIdx].second == color or
        myRows[rowIdx].second == azool::NONE)) {
    // TODO(implementation) to check for empty, should we check the color or # of tiles?
    return false;
  }
  return true;
}  // Player::checkValidMove

bool Player::takeTilesFromFactory(int factoryIdx, azool::TileColor color, int rowIdx) {
  // call game board -> takeTiles(factoryIdx, color)
  int numTiles = 0;
  if (checkValidMove(color, rowIdx) and
      myBoardPtr->takeTilesFromFactory(factoryIdx, color, numTiles)) {
    placeTiles(rowIdx, color, numTiles);
    return true;
  }
  return false;
}  // Player::takeTilesFromFactory

bool Player::takeTilesFromPool(azool::TileColor color, int rowIdx) {
  // call game board -> takeTilesFromPool
  int numTiles = 0;
  bool poolPenalty = false;
  if (!checkValidMove(color, rowIdx)) {
    return false;
  }
  if (!myBoardPtr->takeTilesFromPool(color, numTiles, poolPenalty)) {
    return false;  // couldn't get that tile from the pool
  }
  if (poolPenalty) {
    myTookPoolPenaltyThisTurn = poolPenalty;
    myNumPenaltiesForTurn++;
  }
  placeTiles(rowIdx, color, numTiles);
  return true;
}  // Player::takeTilesFromPool
void Player::placeTiles(int rowIdx, azool::TileColor color, int numTiles) {
  // increment row with # of new tiles
  myRows[rowIdx].first += numTiles;
  // TODO(debug) I can imagine a bug here where the row changes colors...make sure that's not possible
  myRows[rowIdx].second = color;
  int maxNumInRow = rowIdx + 1;
  // if tiles overflow the row, take penalty(ies)
  if (myRows[rowIdx].first > maxNumInRow) {
    myNumPenaltiesForTurn += (myRows[rowIdx].first - maxNumInRow);
    myRows[rowIdx].first = maxNumInRow;
  }
}  // Player::placeTiles

void Player::endRound(bool& fullRow) {
  // determine which rows are full of tiles
  // update the grid
  // send extra tiles back to the game board
  // find the score for each tile placed
  // color = row + column % 5
  // column = (row + 5 - color) % 5
  for (int rowIdx = 0; rowIdx < azool::NUMCOLORS; ++rowIdx) {
    if (myRows[rowIdx].first == (rowIdx+1)) {
      // filled a row. now place a tile on the grid
      // determine which column it belongs to
      // TODO(debug) -- possible bug -- what if color == -1?
      int col = (5 + myRows[rowIdx].second - rowIdx) % 5;
      myGrid[rowIdx][col] = true;
      myScore += scoreTile(rowIdx, col);
      // return extra tiles -- rowIdx = the number of leftover tiles
      myBoardPtr->returnTilesToBag(rowIdx, myRows[rowIdx].second);
      // reset rows for next turn
      myRows[rowIdx].first = 0;
      myRows[rowIdx].second = azool::NONE;
    }
  }
#ifdef DEBUG
  std::cout << "Had " << myNumPenaltiesForTurn << " penalties this turn" << std::endl;
#endif
  if (myNumPenaltiesForTurn >= PenaltyPoints.size()) {
    myScore -= PenaltyPoints[PenaltyPoints.size() - 1];
  }
  else {
    myScore -= PenaltyPoints[myNumPenaltiesForTurn];
  }
  // reset for next turn
  myTookPoolPenaltyThisTurn = false;
  myNumPenaltiesForTurn = 0;
  // Check if there's a full row on the grid
  for (int rowIdx = 0; rowIdx < azool::NUMCOLORS; ++rowIdx) {
    fullRow = true;
    for (int colIdx = 0; colIdx < azool::NUMCOLORS; ++colIdx) {
      if (!myGrid[rowIdx][colIdx]) {
        fullRow = false;
        break;
      }
    }  // iter over elements in row
    if (fullRow) {
      break;
      // found a full row; will signal end of game
      // break out of loop
    }
  }  // iter over rows in grid
}  // Player::endRound

int Player::scoreTile(int tileRow, int tileCol) {
  // search horizontally and vertically for points
  int tileScore = 1;
  // Get column score
#ifdef DEBUG
  std::cout << "Placing tile at " << tileRow << "," << tileCol << std::endl;
#endif
  for (int rowIdx = tileRow - 1; rowIdx > -1; --rowIdx) {
#ifdef DEBUG
    std::cout << " Checking grid at " << rowIdx << "," << tileCol << ": " << myGrid[rowIdx][tileCol] << std::endl;
#endif
    if (!myGrid[rowIdx][tileCol]) {
      break;
    }
    tileScore++;
  }  // iterate above current tileRow
  for (int rowIdx = tileRow + 1; rowIdx < azool::NUMCOLORS; ++rowIdx) {
#ifdef DEBBUG
    std::cout << " Checking grid at " << rowIdx << "," << tileCol << ": " << myGrid[rowIdx][tileCol] << std::endl;
#endif
    if (!myGrid[rowIdx][tileCol]) {
      break;
    }
    tileScore++;
  }  // iterate from current tileRow down
  // Get tileRow score
  for (int colIdx = tileCol - 1; colIdx > -1; --colIdx) {
#ifdef DEBUG
    std::cout << " Checking grid at " << tileRow << "," << colIdx << ": " << myGrid[tileRow][colIdx] << std::endl;
#endif
    if (!myGrid[tileRow][colIdx]) {
      break;
    }
    tileScore++;
  }  // iterate left of current column
  for (int colIdx = tileCol + 1; colIdx < azool::NUMCOLORS; ++colIdx) {
#ifdef DEBUG
    std::cout << " Checking grid at " << tileRow << "," << colIdx << ": " << myGrid[tileRow][colIdx] << std::endl;
#endif
    if (!myGrid[tileRow][colIdx]) {
      break;
    }
    tileScore++;
  }  // iterate from current column to right
  std::cout << " Final Tile Score: " << tileScore << std::endl;
  return tileScore;
}  // Player::scoreTile

void Player::finalizeScore() {
  int numRows = 0;
  int numCols = 0;
  // compute bonus for rows
  for (int rowIdx = 0; rowIdx < azool::NUMCOLORS; ++rowIdx) {
    bool fullRow = true;
    for (int colIdx = 0; colIdx < azool::NUMCOLORS; ++colIdx) {
      if (!myGrid[rowIdx][colIdx]) {
        fullRow = false;
        break;
      }
    }  // iterate over elements in row
    if (fullRow) {
      numRows++;
    }
  }  // iterate over rows
  myScore += (numRows*2);
  // compute bonuses for columns
  for (int colIdx = 0; colIdx < azool::NUMCOLORS; ++colIdx) {
    bool fullCol = true;
    for (int rowIdx = 0; rowIdx < azool::NUMCOLORS; ++rowIdx) {
      if (!myGrid[rowIdx][colIdx]) {
        fullCol = false;
      }
    }  // iterate over elements in column
    if (fullCol) {
      numCols++;
    }
  }  // iterate over columns
  myScore += (numCols*7);
  // compute bonuses for 5 of a kind
  int numFives = 0;
  for (int ii = 0; ii < azool::NUMCOLORS; ++ii) {
    bool allFive = true;
    // column = (row + 5 - color) % 5
    for (int rowIdx = 0; rowIdx < azool::NUMCOLORS; ++rowIdx) {
      int colIdx = (ii + 5 - rowIdx) % 5;
      if (!myGrid[rowIdx][colIdx]) {
        allFive = false;
        break;
      }
    }  // check each row for color ii
    if (allFive) {
      numFives++;
    }
  }  // iterate over all colors
  myScore += (numFives*10);
}  // Player::finalizeScore

void Player::printMyBoard() const {
  // TODO(implementation) - replace cout with ostream
  std::cout << "*******************************\n";
  std::cout << "PLAYER: " << myName << "\n";
  std::cout << *myBoardPtr << "\n\n";
  for (int ii = 0; ii < azool::NUMCOLORS; ++ii) {
    std::cout << ii+1 << ") ";
    for (int jj = azool::NUMCOLORS; jj > (ii+1); --jj) {
      std::cout << " ";
    }
    for (int jj = ii; jj > -1; --jj) {
      if (myRows[ii].second == azool::NONE or
          jj >= myRows[ii].first) {
        std::cout << "_";
      }
      else if (jj < myRows[ii].first) {
          std::cout << azool::TileColorSyms[myRows[ii].second];
      }
    }
    // print grid row
    std::cout << "   |";
    for (int jj = 0; jj < azool::NUMCOLORS; ++jj) {
      // color = row + column % 5
      char color = (ii + jj) % 5;
      if (myGrid[ii][jj]) {
        std::cout << static_cast<char>(azool::TileColorSyms[color] - 32) << "|";
      }
      else {
        std::cout << azool::TileColorSyms[color] << "|";
      }
    }
    std::cout << "\n";
  }  // iterate over rows
  std::cout << "Penalties: " << myNumPenaltiesForTurn << std::endl;
  std::cout << "Score: " << myScore << std::endl;
  // TODO(feature) - print penalty tiles (?)
}  // Player::printMyBoard

bool Player::discardFromFactory(int factoryIdx, azool::TileColor color) {
  int numTiles = -1;
  if (myBoardPtr->takeTilesFromFactory(factoryIdx, color, numTiles)) {
    myNumPenaltiesForTurn += numTiles;
    return true;
  }
  return false;
}  // Player::discardFromFactory

bool Player::discardFromPool(azool::TileColor color) {
  bool poolPenalty = false;
  int numTiles = -1;
  if (myBoardPtr->takeTilesFromPool(color, numTiles, poolPenalty)) {
    if (poolPenalty) {
      myNumPenaltiesForTurn++;
    }
    myNumPenaltiesForTurn += numTiles;
    return true;
  }
  return false;
}  // Player::discardFromPool

namespace {
  int promptForFactoryIdx(int maxNumFactories) {
    static const char* promptFactoryIdxDraw = "Which factory? enter index\n";
    char factInput[256];  // wayy more than we need
    std::cout << promptFactoryIdxDraw << std::flush;
    std::cin >> factInput;
    int factIdx = std::atoi(factInput);
    if (factIdx < 1 or factIdx > maxNumFactories) {
      return -1;
    }
    return factIdx;
  }
  azool::TileColor promptForColor() {
    static const char* promptColorDraw = "Which color? [r|b|g|y|k]\n";
    char colorInput = '\0';
    std::cout << promptColorDraw << std::flush;
    std::cin >> colorInput;
    switch(colorInput) {
      case 'r':
        return azool::RED;
        break;
      case 'b':
        return azool::BLUE;
        break;
      case 'g':
        return azool::GREEN;
        break;
      case 'y':
        return azool::YELLOW;
        break;
      case 'k':
        return azool::BLACK;
        break;
      default:
        return azool::NONE;
    }  // end switch
    return azool::NONE;
  }
  int promptForRow() {
    static const char* promptRowPlacement = "Place on which row? enter number [1-5]\n";
    char rowInput[256] = {0};
    std::cout << promptRowPlacement;
    std::cin >> rowInput;
    int rowIdx = std::atoi(rowInput);
    if (rowIdx < 1 or rowIdx > azool::NUMCOLORS) {
      return -1;
    }
    return rowIdx;
  }
}  // anonymous namespace

void Player::takeTurn() {
  // print game board, handle user input
  if (myBoardPtr->endOfRound()) return;
  printMyBoard();
  static const char* promptDrawInput = "What would you like to do?\n"
                                       "[f] take from factory [p] take from pool "
                                       "[d] discard tile(s) "
                                       "[P] print game board again\n";
  static const char* promptDiscardInput = "From factory or pool? [f|p]\n";
  // TODO(feature) -- remove options when they're not valid?
  // (ie don't print [f] factory when there are no factories left)
  static const char* invalidEntryMessage = "Invalid entry, try again.\n";
  static const char* invalidMoveMessage = "That move was invalid, try again.\n";
  bool fullInput = false;
  while (!fullInput) {
    std::cout << promptDrawInput << std::flush;
    char drawType;
    std::cin >> drawType;
    if (drawType == 'f') {
      int factIdx = promptForFactoryIdx(myBoardPtr->numFactories());
      // draw from factory
      if (factIdx == -1) {
        std::cout << invalidEntryMessage << std::flush;
        continue;
      }
      azool::TileColor colorSelection = promptForColor();
      if (colorSelection == azool::NONE) {
        std::cout << invalidEntryMessage << std::flush;
        continue;
      }
      int rowSelection = promptForRow();
      if (rowSelection == -1) {
        std::cout << invalidEntryMessage << std::flush;
        continue;
      }
      // user enters 1-5; we use 0 indexing internally
      if (!takeTilesFromFactory(factIdx - 1, colorSelection, rowSelection - 1)) {
        std::cout << invalidMoveMessage << std::flush;
        continue;
      }
      fullInput = true;
    }
    else if (drawType == 'p') {
      // draw from pool
      azool::TileColor colorSelection = promptForColor();
      if (colorSelection == azool::NONE) {
        std::cout << invalidEntryMessage << std::flush;
        continue;
      }
      int rowSelection = promptForRow();
      if (rowSelection == -1) {
        std::cout << invalidEntryMessage << std::flush;
        continue;
      }
      // user enters 1-5; we use 0 indexing internally
      if (!takeTilesFromPool(colorSelection, rowSelection - 1)) {
        std::cout << invalidMoveMessage << std::flush;
        continue;
      }
      fullInput = true;
    }
    else if (drawType == 'd') {
      // TODO(implementation) (make these strings more betterer)
      std::cout << promptDiscardInput << std::flush;
      char discardFrom = '\0';
      std::cin >> discardFrom;
      if (discardFrom == 'f') {
        int factIdx = promptForFactoryIdx(myBoardPtr->numFactories());
        // draw from factory
        if (factIdx == -1) {
          std::cout << invalidEntryMessage << std::flush;
          continue;
        }
        azool::TileColor colorSelection = promptForColor();
        if (colorSelection == azool::NONE) {
          std::cout << invalidEntryMessage << std::flush;
          continue;
        }
        int numTiles = -1;
        if (!discardFromFactory(factIdx - 1, colorSelection)) {
          std::cout << invalidMoveMessage << std::flush;
          continue;
        }
        fullInput = true;
      }  // from factory
      else if (discardFrom == 'p') {
        azool::TileColor colorSelection = promptForColor();
        if (colorSelection == azool::NONE) {
          std::cout << invalidEntryMessage << std::flush;
          continue;
        }
        if (!discardFromPool(colorSelection)) {
          std::cout << invalidMoveMessage << std::flush;
          continue;
        }
        fullInput = true;
      }  // discard from pool
    }
    else if (drawType == 'P') {
      printMyBoard();
    }
    else {
      std::cout << invalidEntryMessage << std::flush;
    }
  }  // while !fullinput
  // options: take tile from pool or take tile from factory
  // TODO(feature) - temporary...reprint board and hold briefly?
}  // Player::takeTurn
