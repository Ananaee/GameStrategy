#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "point.hpp"
#include <cstdlib>
#include <ctime>

struct Player {
    Point position;
    char symbol;

    Player(double x, double y, char sym) : position(x, y), symbol(sym) {
        std::srand(std::time(0)); // Initialize random seed
    }

    void move(char direction) {
        switch (direction) {
            case 'U': position.y -= 1; break;
            case 'D': position.y += 1; break;
            case 'L': position.x -= 1; break;
            case 'R': position.x += 1; break;
        }
    }

    bool isValidMove(char direction, int boardSize) const {
        switch (direction) {
            case 'U': return position.y > 0;
            case 'D': return position.y < boardSize - 1;
            case 'L': return position.x > 0;
            case 'R': return position.x < boardSize - 1;
            default: return false;
        }
    }

    char randomMove(int boardSize) {
        char directions[] = {'U', 'D', 'L', 'R'};
        while (true) {
            char direction = directions[std::rand() % 4];
            if (isValidMove(direction, boardSize)) {
                return direction;
            }
        }
    }
};

#endif // PLAYER_HPP
