#include "voronoi.hpp"

int main(int argc, char const* argv[]) {
    Voronoi voronoi(1920, 1080, 20);
    
    if (!voronoi.initialize()) {
        return EXIT_FAILURE;
    }
    
    voronoi.run();

    return EXIT_SUCCESS;
}
