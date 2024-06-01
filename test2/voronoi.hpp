#ifndef VORONOI_HPP
#define VORONOI_HPP

#include </home/cytech/nsdhw_24sp/project/Ananaee/test2/SFML/SFML2/include/SFML/Graphics.hpp>
#include <vector>
#include <random>

class Voronoi {
public:
    Voronoi(int width, int height, int initialPoints);
    bool initialize();
    void run();

private:
    void handleEvents();
    void update();
    void render();
    void addPoint(sf::Vector2f position);


    const int WIDTH;
    const int HEIGHT;
    int pointsNumber;
    const int MAX_POINTS_NUMBER = 512;

    sf::RenderWindow window;
    sf::Shader shader;
    std::vector<sf::Vector2f> coordinates;
    std::vector<std::pair<sf::CircleShape, bool>> circles;

#ifdef COLORS
    std::vector<sf::Vector3f> colors;
#endif

    std::random_device dev;
    std::default_random_engine gen;
    std::uniform_real_distribution<> wRand;
    std::uniform_real_distribution<> hRand;

#ifdef COLORS
    std::uniform_real_distribution<> frand;
#endif
};

#endif // VORONOI_HPP
