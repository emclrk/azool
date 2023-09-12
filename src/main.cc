#include "GameBoard.h"
#include "Player.h"
#include <iostream>
#include <boost/lexical_cast.hpp>

// who manages turns and rounds? probably the main function

void testPrint(GameBoard* game) {
  game->dealTiles();
  Player p1(game);
  std::cout << p1.printMyBoard();
  return;
}

void playGame(GameBoard* game, int numPlayers=2) {
  std::vector<Player*> players;
  for (int ii = 0; ii < numPlayers; ++ii) {
    std::string name = "P" + boost::lexical_cast<std::string>(ii+1);
    players.push_back(new Player(game, name));
  }
  bool endOfGame = false;
  int firstPlayerIdx = 0;
  bool endGame = false;
  while (!endGame) {
    game->dealTiles();
    while (!game->endOfRound()) {
      for (int ii = 0; ii < numPlayers; ++ii) {
        int idx = (ii + firstPlayerIdx) % numPlayers;
        players[idx]->takeTurn();
        sleep(1);
      }
    }
    firstPlayerIdx = -1;
    for (int ii = 0; ii < numPlayers; ++ii) {
      if (players[ii]->tookPenalty()) {
        firstPlayerIdx = ii;
        break;
      }
    }
    if (firstPlayerIdx < 0) {
      std::cerr << "SOMETHING WEIRD - SoMeone has to go first...\n" << std::flush;
      firstPlayerIdx = 0;
    }
    std::cout << "End of round!" << std::endl;
    std::cout << "\033c";
    for (auto player : players) {
      bool fullRow = false;
      player->endRound(fullRow);
      if (fullRow) {
        endGame = true;
      }
    }
  }
  std::cout << " Final scores:\n";
  for (auto player : players) {
    player->finalizeScore();
    std::cout << player->getPlayerName() << ":  " << player->getScore() << "\n" << std::flush;
  }
  // separate loops b/c we want scores to print before printing boards
  for (auto player : players) {
    std::cout << player->printMyBoard() << std::flush;
    if (player) delete player;
  }
}

int main(int argc, char* argv[]) {
  int numPlayers = 2;
  static const std::string helpMsg =
    "USAGE: bin/azool [-p NUM_PLAYERS] [-h]\n"
    "-p     number of players between 2-4 (default=2)\n"
    "-h     print help and exit\n";
  for (int ii = 1; ii < argc; ++ii) {
    if (strcmp(argv[ii], "-p") == 0) {
      if (argc <= (ii+1)) {
        std::cerr << "ERROR: not enough arguments.\n" << helpMsg << std::flush;
        return 1;
      }
      // next argument is # of players. need to increment
      numPlayers = boost::lexical_cast<int>(argv[++ii]);
      if (numPlayers < 2 or numPlayers > 4) {
        std::cerr << "ERROR: invalid number of players!\n" << helpMsg << std::flush;
        return 1;
      }
    }
    else if (strcmp(argv[ii], "-h") == 0) {
      std::cout << helpMsg << std::flush;
      return 0;
    }
    else {
      std::cerr << "ERROR: Unrecognized argument" << std::endl;
        std::cerr << helpMsg << std::flush;
        return 1;
    }
  }  // iterate over arguments
  std::cout << "playing with " << numPlayers << " players" << std::endl;
  GameBoard* game = new GameBoard(numPlayers);
  playGame(game, numPlayers);
  if (game) delete game;
  return 0;
}
