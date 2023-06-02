#include "GameBoard.h"
#include "Player.h"
#include <iostream>

// who manages turns and rounds? probably the main function

void testPrint(GameBoard* game) {
  game->dealTiles();
  Player p1(game);
  std::cout << p1.printMyBoard();
  return;
}

void playGame(GameBoard* game) {
  game->dealTiles();
  std::vector<Player*> players = {new Player(game, "P1"), new Player(game, "P2")};
  bool endOfGame = false;
  Player* firstPlayer = players[0];  // pointers to keep track of first and second player
  Player* secondPlayer = players[1];
  while (!endOfGame) {
    while (!game->endOfRound()) {
      // TODO figure out how order will work for > 2 players
      firstPlayer->takeTurn();
      secondPlayer->takeTurn();
    }
    std::cout << "End of round!" << std::endl;
    bool p0EndsGame = false;
    bool p1EndsGame = false;
    players[0]->endRound(p0EndsGame);
    players[1]->endRound(p1EndsGame);
    if (players[0]->tookPenalty()) {
      firstPlayer = players[0];
      secondPlayer = players[1];
    }
    else if (players[1]->tookPenalty()) {
      firstPlayer = players[1];
      secondPlayer = players[0];
    }
    else {
      std::cerr << "SOMETHING WEIRD - SoMeone has to go first...\n" << std::flush;
      firstPlayer = players[0];
      secondPlayer = players[1];
    }
    if (!(p0EndsGame or p1EndsGame)) {
      game->dealTiles();
    }
  }
  players[0]->finalizeScore();
  players[1]->finalizeScore();
  std::cout << " Final scores:\n"
            << players[0]->getScore() << "\n" << std::flush;
  std::cout << players[0]->printMyBoard();
  std::cout << players[0]->getScore() << "\n" << std::flush;
  std::cout << players[1]->printMyBoard();
  if (players[0]) delete players[0];
  if (players[1]) delete players[1];
}

int main() {
  GameBoard* game = new GameBoard();
  playGame(game);
  if (game) delete game;
  return 0;
}
