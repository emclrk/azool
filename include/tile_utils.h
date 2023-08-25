#ifndef TILE_UTILS_H_
#define TILE_UTILS_H_
namespace azool {
enum TileColor {
  NONE = -1,
  RED = 0,
  BLUE,
  GREEN,
  YELLOW,
  BLACK,
  NUMCOLORS
};
const std::string TileColorStrings[NUMCOLORS] =  {
  "red",
  "blue",
  "green",
  "yellow",
  "black"
};

const char TileColorSyms[NUMCOLORS] = { 'r', 'b', 'g', 'y', 'k' };
const int MAXPLAYERS = 4;

const std::string REQ_TYPE_DRAW_FROM_FACTORY    = "DRAW_FROM_FACTORY";
const std::string REQ_TYPE_DRAW_FROM_POOL       = "DRAW_FROM_POOL";
const std::string REQ_TYPE_RETURN_TO_BAG        = "RETURN_TO_BAG";
const std::string REQ_TYPE_GET_BOARD            = "GET_BOARD";
}
#endif  // TILE_UTILS_H_
