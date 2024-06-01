#include <iostream>
#include "player.hpp"
#include "tile.hpp"
#include "game.hpp"

int main() {
    std::vector<Player> players = {
        Player(0, 3, 'A'),
        Player(7, 3, 'B')
    };

    std::vector<Tile> tiles;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            tiles.emplace_back(x, y);
        }
    }

    int movesPerPlayer = 2;
    movePlayers(players, tiles, movesPerPlayer);

    return 0;
}
