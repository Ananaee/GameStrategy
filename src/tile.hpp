#ifndef TILE_HPP
#define TILE_HPP

#include "point.hpp"
#include "player.hpp"

struct Tile {
    Point position;
    Player* site; // Pointer to the player controlling the tile

    Tile(double x, double y) : position(x, y), site(nullptr) {}
};

#endif // TILE_HPP

