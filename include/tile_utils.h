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
}
#endif  // TILE_UTILS_H_
