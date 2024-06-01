#ifndef VORONOI_HPP
#define VORONOI_HPP

#include <vector>
#include "tile.hpp"
#include "player.hpp"

void fillVoronoi(std::vector<Tile>& tiles, const std::vector<Player>& players);

double distance(const Point& pointA, const Point& pointB);

#endif // VORONOI_HPP

