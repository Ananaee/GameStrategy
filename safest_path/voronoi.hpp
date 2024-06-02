#ifndef VORONOI_HPP
#define VORONOI_HPP

#include <SFML/Graphics.hpp>
#include <boost/polygon/voronoi.hpp>
#include <vector>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <functional>

using namespace boost::polygon;
using namespace std;

struct VoronoiPoint {
    float x, y;
    int regionIndex;

    VoronoiPoint(float x, float y, int regionIndex) : x(x), y(y), regionIndex(regionIndex) {}
};

struct VoronoiEdge {
    sf::Vector2f start;
    sf::Vector2f end;
    float length;

    VoronoiEdge(sf::Vector2f start, sf::Vector2f end) 
        : start(start), end(end), length(sqrt(pow(end.x - start.x, 2) + pow(end.y - start.y, 2))) {}
};

struct VoronoiRegion {
    int pointIndex;
    std::vector<VoronoiEdge> edges;

    VoronoiRegion(int pointIndex) : pointIndex(pointIndex) {}
};

struct GraphNode {
    sf::Vector2f position;
    std::vector<std::pair<int, float>> neighbors; // Pair of neighbor index and distance

    GraphNode(sf::Vector2f pos) : position(pos) {}
};

class Voronoi {
public:
    Voronoi(int width, int height, int initialPoints);
    bool initialize();
    void run();

private:
    const int WIDTH, HEIGHT;

    const int MAX_POINTS_NUMBER = 512;
    int pointsNumber;
    int startNode = -1;
    int endNode = -1;
    bool selectingStartNode = true;
    sf::RenderWindow window;
    std::vector<sf::Vector2f> coordinates;
    std::vector<std::pair<sf::CircleShape, bool>> circles; 
    sf::Shader shader;
    sf::VertexArray edges;
    std::random_device dev;
    std::mt19937 gen;
    std::uniform_real_distribution<> wRand;
    std::uniform_real_distribution<> hRand;
#ifdef COLORS
    std::uniform_real_distribution<> frand;
    std::vector<sf::Vector3f> colors;
#endif

    std::vector<GraphNode> graphNodes; // For A* pathfinding

    void handleEvents();
    void update();
    void render();
    void generateVoronoi();
    void displayPath(const std::vector<int>& path);
    void addPoint(sf::Vector2f position);
    std::vector<int> aStar(int startNode, int endNode);
};

#endif // VORONOI_HPP
