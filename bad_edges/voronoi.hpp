#ifndef VORONOI_HPP
#define VORONOI_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <iostream>
#include <boost/polygon/voronoi.hpp>


// Custom hash function for sf::Vector2f
namespace std {
    template<>
    struct hash<sf::Vector2f> {
        std::size_t operator()(const sf::Vector2f& vec) const noexcept {
            std::size_t h1 = std::hash<float>{}(vec.x);
            std::size_t h2 = std::hash<float>{}(vec.y);
            return h1 ^ (h2 << 1); // Combine the two hash values
        }
    };

    template<>
    struct equal_to<sf::Vector2f> {
        bool operator()(const sf::Vector2f& lhs, const sf::Vector2f& rhs) const {
            return lhs.x == rhs.x && lhs.y == rhs.y;
        }
    };
}

// Forward declarations
class VoronoiRegion;
struct Graph;

class VoronoiPoint {
public:
    sf::Vector2f location;
    VoronoiRegion* region;

    VoronoiPoint(float x, float y);
};

class VoronoiEdge {
public:
    VoronoiPoint* start;
    VoronoiPoint* end;
    sf::Vector2f startPoint;
    sf::Vector2f endPoint;
    float distance;

    VoronoiEdge(VoronoiPoint* s, VoronoiPoint* e, const sf::Vector2f& sp, const sf::Vector2f& ep,const float dist);
};

class VoronoiRegion {
public:
    VoronoiPoint* point;
    std::vector<VoronoiEdge*> edges;

    VoronoiRegion(VoronoiPoint* p);
};

class Voronoi {
public:
    Voronoi(int width, int height, int initialPoints);
    bool initialize();
    void run();
    std::vector<VoronoiPoint> points;
    std::vector<VoronoiRegion> regions;
    std::vector<VoronoiEdge> edges;

    void addPoint(float x, float y) {
        points.emplace_back(x, y);
        regions.emplace_back(&points.back());
        points.back().region = &regions.back();
        
    }

    void generateEdges() {
        
        edges.clear();
        for (size_t i = 0; i < points.size(); ++i) {
            
            for (size_t j = i + 1; j < points.size(); ++j) {
                float distance = std::hypot(points[i].location.x - points[j].location.x, points[i].location.y - points[j].location.y);

                sf::Vector2f midPoint = (points[i].location + points[j].location) / 2.f;
                sf::Vector2f direction = points[j].location - points[i].location;
                sf::Vector2f perpendicular = sf::Vector2f(-direction.y, direction.x);
                sf::Vector2f start = midPoint + perpendicular;
                sf::Vector2f end = midPoint - perpendicular;
                edges.emplace_back(&points[i], &points[j], start, end, distance);

                
                regions[i].edges.push_back(&edges.back());
                regions[j].edges.push_back(&edges.back());
                
            }
        }
        if(edges.empty()){
            std::cout<<"adios"<<std::endl;
        }
    }
    


    void generateGraph(Graph& graph);

private:
    void handleEvents();
    void update();
    void render();
    void addPoint(sf::Vector2f position);

    void generateGraph();
    std::vector<VoronoiEdge*> aStar(VoronoiRegion* start, VoronoiRegion* goal);
    void displayPath(const std::vector<VoronoiEdge*>& path);

    const int WIDTH;
    const int HEIGHT;
    int pointsNumber;
    const int MAX_POINTS_NUMBER = 512;

    sf::RenderWindow window;
    sf::Shader shader;
    std::vector<sf::Vector2f> coordinates;
    std::vector<std::pair<sf::CircleShape, bool>> circles;

    std::vector<VoronoiPoint*> voronoiPoints;
    std::vector<VoronoiRegion*> voronoiRegions;
    std::vector<VoronoiEdge*> voronoiEdges;

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

struct Vertex {
    sf::Vector2f position;
    std::vector<std::pair<Vertex*, float>> neighbors;
};

struct Graph {
    std::unordered_map<sf::Vector2f, Vertex> vertices;
};

void addEdge(Graph& graph, const sf::Vector2f& start, const sf::Vector2f& end);
std::vector<sf::Vector2f> aStar(Graph& graph, const sf::Vector2f& start, const sf::Vector2f& goal);

#endif // VORONOI_HPP
