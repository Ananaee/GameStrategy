#include "voronoi.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

Voronoi::Voronoi(int width, int height, int maxTurns)
    : WIDTH(width), HEIGHT(height), maxTurns(maxTurns), currentPlayer(0), turnCount(0), gameEnded(false),
      window(sf::VideoMode(WIDTH, HEIGHT), "Voronoi Game", sf::Style::Close | sf::Style::Titlebar, sf::ContextSettings(0, 0, 8)) {
    window.setFramerateLimit(60);
    playerScores.resize(2, 0);
    window.setPosition(sf::Vector2i(0, 0));
    // colors.reserve(maxTurns); // Réservez suffisamment d'espace pour le vecteur colors
}


bool Voronoi::initialize() {
    // Initialisation du shader
    if (!sf::Shader::isAvailable()) {
        std::cerr << "Shaders are not available on this PC!" << std::endl;
        return false;
    }

    if (!shader.loadFromFile("voronoiColors.frag", sf::Shader::Fragment)) {
        std::cerr << "Failed to load shader!" << std::endl;
        return false;
    }

    std::string shaderName = 
#ifdef COLORS
    "voronoiColors.frag";
#else 
    "voronoi.frag";
#endif
    std::cout << "Initialisation du jeu..." << std::endl;
    return true; // Retourne true si l'initialisation réussit
}

void Voronoi::run() {
    if (!initialize()) {
        return;
    }

    while (window.isOpen()) {
        handleEvents();
        if (!gameEnded) {
            update();
        }
        render();
    }
}

void Voronoi::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && turnCount < maxTurns) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            addPoint(mousePos);
        }
    }
}

void Voronoi::update() {
    // Update game state if necessary
    if (turnCount == maxTurns) {
        calculateWinner();
        gameEnded = true;
    }
}

void Voronoi::calculateAreas(std::vector<int>& areas) {
    areas.clear();
    areas.resize(2, 0); // On a deux joueurs, donc deux aires à calculer

    // Parcourez chaque pixel de la fenêtre
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            sf::Vector2f point(x, HEIGHT - y);
            float minDist = std::numeric_limits<float>::max();
            int closestSite = -1;

            // Trouvez le site de Voronoi le plus proche
            for (int i = 0; i < coordinates.size(); ++i) {
                float dist = std::pow(coordinates[i].x - point.x, 2) + std::pow(coordinates[i].y - point.y, 2);
                if (dist < minDist) {
                    minDist = dist;
                    closestSite = i;
                }
            }

            // Augmentez l'aire du joueur correspondant
            if (closestSite != -1) {
                if (colors[closestSite] == sf::Vector3f(1.0f, 0.0f, 0.0f)) { // Rouge
                    areas[0]++;
                } else if (colors[closestSite] == sf::Vector3f(0.0f, 0.0f, 1.0f)) { // Bleu
                    areas[1]++;
                }
            }
        }
    }
}


void Voronoi::render() {
    window.clear(sf::Color::White);

    std::vector<sf::Vector2f> copy(coordinates.size());
    std::transform(coordinates.begin(), coordinates.end(), copy.begin(), [&](sf::Vector2f vec) {
        return sf::Vector2f(vec.x, window.getSize().y - vec.y);
    });

    shader.setUniform("size", static_cast<int>(coordinates.size()));
    shader.setUniformArray("seeds", copy.data(), static_cast<int>(coordinates.size()));
    shader.setUniformArray("colors", colors.data(), static_cast<int>(colors.size()));

    window.draw(sf::RectangleShape(sf::Vector2f(WIDTH, HEIGHT)), &shader);

    for (auto& c : circles) {
        window.draw(c.first);
    }

    window.display();
}

void Voronoi::addPoint(sf::Vector2f position) {
    coordinates.push_back(position);
    sf::CircleShape tempPoint(4, 100);
    tempPoint.setFillColor(sf::Color::Black);
    tempPoint.setOrigin(4, 4);
    tempPoint.setOutlineColor(sf::Color::Green);
    tempPoint.setPosition(position);
    circles.push_back({tempPoint, false});

    // Add color for the current player
    if (currentPlayer == 0) {
        colors.push_back(sf::Vector3f(1.0f, 0.0f, 0.0f)); // Rouge
    } else {
        colors.push_back(sf::Vector3f(0.0f, 0.0f, 1.0f)); // Bleu
    }

    playerScores[currentPlayer]++;
    turnCount++;
    switchPlayer();
}

void Voronoi::switchPlayer() {
    currentPlayer = (currentPlayer + 1) % 2;
}

void Voronoi::calculateWinner() {
    std::vector<int> areas;
    calculateAreas(areas);

    int winner = (areas[0] > areas[1]) ? 0 : 1;
    std::cout << "Player " << winner + 1 << " wins!" << std::endl;
    std::cout << "Player 1 area: " << areas[0] << std::endl;
    std::cout << "Player 2 area: " << areas[1] << std::endl;
}
