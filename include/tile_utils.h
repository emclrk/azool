#ifndef TILE_UTILS_H_
#define TILE_UTILS_H_
namespace azool {
enum TileColor {
  NONE = -1,
  RED = 0,
  BLUE,
  GREEN,
  YELLOW,
  WHITE,
  NUMCOLORS
};
const std::string TileColorStrings[NUMCOLORS] =  {
  "\x1B[1;41mR\x1B[0m",//"red",
  "\x1B[1;44mB\x1B[0m",//"blue",
  "\x1B[1;42mG\x1B[0m",//"green",
  "\x1B[1;30;103mY\x1B[0m",//"yellow",
  "\x1B[1;30;107mW\x1B[0m",//"white"
};

const char TileColorSyms[NUMCOLORS] = { 'r', 'b', 'g', 'y', 'w' };
const int MAXPLAYERS = 4;

const std::string REQ_TYPE_DRAW_FROM_FACTORY    = "DRAW_FROM_FACTORY";
const std::string REQ_TYPE_DRAW_FROM_POOL       = "DRAW_FROM_POOL";
const std::string REQ_TYPE_RETURN_TO_BAG        = "RETURN_TO_BAG";
const std::string REQ_TYPE_GET_BOARD            = "GET_BOARD";
}
#endif  // TILE_UTILS_H_
