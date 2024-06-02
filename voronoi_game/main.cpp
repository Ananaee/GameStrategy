#include "voronoi.hpp"

int main() {
    const int WIDTH = 1200;
    const int HEIGHT = 800;
    const int MAX_TURNS = 10; // Nombre de tours par joueur

    Voronoi game(WIDTH, HEIGHT, MAX_TURNS);
    game.run();

    return 0;
}
