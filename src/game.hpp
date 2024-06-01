#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include "tile.hpp"
#include "player.hpp"

void movePlayers(std::vector<Player>& players, std::vector<Tile>& tiles, int movesPerPlayer);

#endif // GAME_HPP
