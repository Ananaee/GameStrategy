#include "voronoi.hpp"

int main(int argc, char const* argv[]) {
    Voronoi voronoi(1920, 1080, 30);

    if (!voronoi.initialize()) {
        return EXIT_FAILURE;
    }

    voronoi.run();

    return EXIT_SUCCESS;
}
