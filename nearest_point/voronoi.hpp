#ifndef VORONOI_HPP
#define VORONOI_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <memory>

struct Point {
    sf::Vector2f position;
    bool hasHealthPack;
};

class Quadtree {
public:
    Quadtree(int level, sf::FloatRect bounds);
    void clear();
    void split();
    int getIndex(const sf::Vector2f& position) const;
    void insert(const Point& point);
    void retrieve(std::vector<Point>& returnPoints, const sf::Vector2f& position);

private:
    static const int MAX_POINTS = 4;
    static const int MAX_LEVELS = 5;

    int level;
    sf::FloatRect bounds;
    std::vector<Point> points;
    std::unique_ptr<Quadtree> nodes[4];
};

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
    void removePoint(sf::Vector2f position);
    Point findNearestHealthPack(const sf::Vector2f& position);
    void visualizeNearestHealthPack(const sf::Vector2f& position);

    const int WIDTH;
    const int HEIGHT;
    int pointsNumber;
    const int MAX_POINTS_NUMBER = 512;

    sf::RenderWindow window;
    sf::Shader shader;
    std::vector<sf::Vector2f> coordinates;
    std::vector<std::pair<sf::CircleShape, bool>> circles;

    Quadtree quadtree;

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

    sf::CircleShape nearestPackCircle; // Cercle pour visualiser le point le plus proche
};

#endif // VORONOI_HPP
