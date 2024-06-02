#ifndef VORONOI_HPP
#define VORONOI_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

class Voronoi {
public:
    Voronoi(int width, int height, int maxTurns);
    bool initialize();
    void run();

private:
    void handleEvents();
    void update();
    void render();
    void addPoint(sf::Vector2f position);
    void switchPlayer();
    void calculateWinner();
    void calculateAreas(std::vector<int>& areas); 

    const int WIDTH;
    const int HEIGHT;
    int maxTurns;
    int currentPlayer;
    int turnCount;
    bool gameEnded;
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
    // std::uniform_real_distribution<float> frand;

    std::vector<int> playerScores;
#ifdef COLORS
    std::uniform_real_distribution<> frand;
#endif
};

#endif // VORONOI_HPP
