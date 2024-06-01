#include "voronoi.hpp"
#include <cmath>
#include <limits>

double distance(const Point& pointA, const Point& pointB) {
    double dx = pointA.x - pointB.x;
    double dy = pointA.y - pointB.y;
    return std::abs(dx) + std::abs(dy);
}

void fillVoronoi(std::vector<Tile>& tiles, const std::vector<Player>& players) {
    for (auto& tile : tiles) {
        tile.site = nullptr;
        double minDistance = std::numeric_limits<double>::max();
        for (const auto& player : players) {
            double tmpDist = distance(tile.position, player.position);
            if (tmpDist < minDistance) {
                minDistance = tmpDist;
                tile.site = const_cast<Player*>(&player);
            }
        }
    }
}
