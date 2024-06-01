#include "game.hpp"
#include "voronoi.hpp"
#include <iostream>

void printBoard(const std::vector<Player>& players, const std::vector<Tile>& tiles, int boardSize) {
    for (int y = 0; y < boardSize; ++y) {
        for (int x = 0; x < boardSize; ++x) {
            bool playerFound = false;
            for (const auto& player : players) {
                if (static_cast<int>(player.position.x) == x && static_cast<int>(player.position.y) == y) {
                    std::cout << player.symbol << ' ';
                    playerFound = true;
                    break;
                }
            }
            if (!playerFound) {
                const Tile& tile = tiles[y * boardSize + x];
                if (tile.site) {
                    if (tile.site->symbol == 'A') {
                        std::cout << 'X' << ' ';
                    } else if (tile.site->symbol == 'B') {
                        std::cout << 'O' << ' ';
                    }
                } else {
                    std::cout << ". ";
                }
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void movePlayers(std::vector<Player>& players, std::vector<Tile>& tiles, int movesPerPlayer) {
    int boardSize = 8;
    int totalMoves = movesPerPlayer * players.size();

    for (int moveCount = 0; moveCount < totalMoves; ++moveCount) {
        Player& currentPlayer = players[moveCount % players.size()];

        char direction;
        if (currentPlayer.symbol == 'B') {
            direction = currentPlayer.randomMove(boardSize);
            std::cout << "Player " << currentPlayer.symbol << " moves randomly: " << direction << std::endl;
        } else {
            std::cout << "Player " << currentPlayer.symbol << " move (U/D/L/R): ";
            std::cin >> direction;

            if (!currentPlayer.isValidMove(direction, boardSize)) {
                std::cout << "Invalid move! Try again." << std::endl;
                --moveCount;
                continue;
            }
        }

        currentPlayer.move(direction);

        // Recalculate Voronoi diagram
        fillVoronoi(tiles, players);

        // Print the board state
        printBoard(players, tiles, boardSize);
    }
}
