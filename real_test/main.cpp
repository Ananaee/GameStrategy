#include "voronoi.hpp"



int main(int argc, char const* argv[]) {
    Voronoi voronoi(1200, 800, 20);
    
    if (!voronoi.initialize()) {
        return EXIT_FAILURE;
    }
    
    voronoi.run();

    return EXIT_SUCCESS;

}
