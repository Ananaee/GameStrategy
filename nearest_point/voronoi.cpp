#include "voronoi.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

// Implémentation de la classe Quadtree
Quadtree::Quadtree(int level, sf::FloatRect bounds)
    : level(level), bounds(bounds) {}

void Quadtree::clear() {
    points.clear();
    for (auto& node : nodes) {
        if (node) {
            node->clear();
            node.reset();
        }
    }
}

void Quadtree::split() {
    float subWidth = bounds.width / 2.f;
    float subHeight = bounds.height / 2.f;
    float x = bounds.left;
    float y = bounds.top;

    nodes[0] = std::make_unique<Quadtree>(level + 1, sf::FloatRect(x, y, subWidth, subHeight));
    nodes[1] = std::make_unique<Quadtree>(level + 1, sf::FloatRect(x + subWidth, y, subWidth, subHeight));
    nodes[2] = std::make_unique<Quadtree>(level + 1, sf::FloatRect(x, y + subHeight, subWidth, subHeight));
    nodes[3] = std::make_unique<Quadtree>(level + 1, sf::FloatRect(x + subWidth, y + subHeight, subWidth, subHeight));
}

int Quadtree::getIndex(const sf::Vector2f& position) const {
    int index = -1;
    float verticalMidpoint = bounds.left + bounds.width / 2.f;
    float horizontalMidpoint = bounds.top + bounds.height / 2.f;

    bool topQuadrant = (position.y < horizontalMidpoint);
    bool bottomQuadrant = (position.y > horizontalMidpoint);

    if (position.x < verticalMidpoint) {
        if (topQuadrant) {
            index = 0;
        } else if (bottomQuadrant) {
            index = 2;
        }
    } else {
        if (topQuadrant) {
            index = 1;
        } else if (bottomQuadrant) {
            index = 3;
        }
    }

    return index;
}

void Quadtree::insert(const Point& point) {
    if (!nodes[0]) {
        if (points.size() < MAX_POINTS || level == MAX_LEVELS) {
            points.push_back(point);
            return;
        }

        split();
    }

    int index = getIndex(point.position);
    if (index != -1) {
        nodes[index]->insert(point);
    } else {
        points.push_back(point);
    }
}

void Quadtree::retrieve(std::vector<Point>& returnPoints, const sf::Vector2f& position) {
    int index = getIndex(position);
    if (index != -1 && nodes[0]) {
        nodes[index]->retrieve(returnPoints, position);
    }

    returnPoints.insert(returnPoints.end(), points.begin(), points.end());
}

// Implémentation de la classe Voronoi
Voronoi::Voronoi(int width, int height, int initialPoints)
    : WIDTH(width), HEIGHT(height), pointsNumber(initialPoints),
      quadtree(0, sf::FloatRect(0, 0, width, height)),
      window(sf::VideoMode(width, height), "Voronoi Diagram", sf::Style::Close | sf::Style::Titlebar, sf::ContextSettings(0, 0, 8)),
      gen(dev()), wRand(30.0, width - 30.0), hRand(30.0, height - 30.0) {

    window.setPosition(sf::Vector2i(0, 0));
    coordinates.resize(pointsNumber);
    std::generate(coordinates.begin(), coordinates.end(), [&]() { return sf::Vector2f(wRand(gen), hRand(gen)); });

#ifdef COLORS
    frand = std::uniform_real_distribution<>(70.0 / 255, 1.0);
    colors.resize(pointsNumber);
    std::generate(colors.begin(), colors.end(), [&]() { return sf::Vector3f(frand(gen), frand(gen), frand(gen)); });
#endif

    sf::CircleShape tempPoint(4, 100);
    tempPoint.setFillColor(sf::Color::Black);
    tempPoint.setOrigin(4, 4);
    tempPoint.setOutlineColor(sf::Color::Green);

    for (auto& coord : coordinates) {
        tempPoint.setPosition(coord);
        circles.push_back({tempPoint, false});
        quadtree.insert({coord, true});
    }


    nearestPackCircle.setRadius(6.0f);
    nearestPackCircle.setFillColor(sf::Color::Red);
    nearestPackCircle.setOrigin(6.0f, 6.0f);
}

bool Voronoi::initialize() {
    if (!sf::Shader::isAvailable()) {
        std::cerr << "Shaders are not available on this PC!" << std::endl;
        return false;
    }

    std::string shaderName = 
    #ifdef COLORS
        "voronoiColors.frag";
    #else 
        "voronoi.frag";
    #endif

    if (!shader.loadFromFile(shaderName, sf::Shader::Fragment)) {
        std::cerr << "Failed to load shader!" << std::endl;
        return false;
    }

    return true;
}

void Voronoi::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

void Voronoi::addPoint(sf::Vector2f position) {
    coordinates.push_back(position);
    sf::CircleShape tempPoint(4, 100);
    tempPoint.setFillColor(sf::Color::Black);
    tempPoint.setOrigin(4, 4);
    tempPoint.setOutlineColor(sf::Color::Green);
    tempPoint.setPosition(position);
    circles.push_back({tempPoint, false});
    quadtree.insert({position, true});

#ifdef COLORS
    colors.push_back(sf::Vector3f(frand(gen), frand(gen), frand(gen)));
#endif

    pointsNumber++;
}

void Voronoi::removePoint(sf::Vector2f position) {
    auto it = std::find_if(coordinates.begin(), coordinates.end(), [&](const sf::Vector2f& point) {
        return sqrt(pow(point.x - position.x, 2) + pow(point.y - position.y, 2)) < 5.0f; // tolérance de 5 pixels
    });

    if (it != coordinates.end()) {
        int index = std::distance(coordinates.begin(), it);
        coordinates.erase(it);
        circles.erase(circles.begin() + index);
#ifdef COLORS
        colors.erase(colors.begin() + index);
#endif
        pointsNumber--;
        quadtree.clear();
        for (const auto& coord : coordinates) {
            quadtree.insert({coord, true});
        }
    }
}

Point Voronoi::findNearestHealthPack(const sf::Vector2f& position) {
    std::vector<Point> possiblePoints;
    quadtree.retrieve(possiblePoints, position);

    Point nearestPack;
    float minDistance = std::numeric_limits<float>::infinity();

    for (const auto& point : possiblePoints) {
        if (point.hasHealthPack) {
            float distance = sqrt(pow(point.position.x - position.x, 2) + pow(point.position.y - position.y, 2));
            if (distance < minDistance) {
                minDistance = distance;
                nearestPack = point;
            }
        }
    }

    return nearestPack;
}

void Voronoi::visualizeNearestHealthPack(const sf::Vector2f& position) {
    Point nearestPack = findNearestHealthPack(position);
    nearestPackCircle.setPosition(nearestPack.position);
    std::cout << "Nearest health pack at: (" << nearestPack.position.x << ", " << nearestPack.position.y << ")" << std::endl;
}

void Voronoi::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            addPoint(mousePos);
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            removePoint(mousePos);
            visualizeNearestHealthPack(mousePos); // Visualiser le point le plus proche après suppression
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            bool outOfCircle = std::none_of(circles.begin(), circles.end(), [&](const auto& circle) {
                return circle.first.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(window)));
            });

            if (outOfCircle) {
                coordinates.push_back(sf::Vector2f(sf::Mouse::getPosition(window)));
                sf::CircleShape tempPoint(4, 100);
                tempPoint.setFillColor(sf::Color::Black);
                tempPoint.setOrigin(4, 4);
                tempPoint.setOutlineColor(sf::Color::Green);
                tempPoint.setPosition(coordinates.back());
                circles.push_back({tempPoint, false});
                quadtree.insert({coordinates.back(), true}); // Insérer le nouveau point dans le quadtree

#ifdef COLORS
                colors.push_back(sf::Vector3f(frand(gen), frand(gen), frand(gen)));
#endif

                pointsNumber++;
            }
        }
    }
}

void Voronoi::update() {
    for (size_t i = 0; i < pointsNumber; i++) {
        auto mousePos = sf::Mouse::getPosition(window);
        float radius = (std::hypot(mousePos.x - coordinates[i].x, mousePos.y - coordinates[i].y) <= 10.0) ? 6.0 : 4.0;
        circles[i].first.setRadius(radius);
        circles[i].first.setOrigin(radius, radius);

        if (circles[i].second) {
            coordinates[i] = sf::Vector2f(sf::Mouse::getPosition(window));
            circles[i].first.setPosition(coordinates[i]);
        }
    }
}

void Voronoi::render() {
    window.clear(sf::Color::White);

    std::vector<sf::Vector2f> copy(coordinates.size());
    std::transform(coordinates.begin(), coordinates.end(), copy.begin(), [&](sf::Vector2f vec) {
        return sf::Vector2f(vec.x, window.getSize().y - vec.y);
    });

    shader.setUniform("size", pointsNumber);
    shader.setUniformArray("seeds", copy.data(), MAX_POINTS_NUMBER);

#ifdef COLORS
    shader.setUniformArray("colors", colors.data(), MAX_POINTS_NUMBER);
#endif

    window.draw(sf::RectangleShape(sf::Vector2f(WIDTH, HEIGHT)), &shader);

    for (auto& c : circles) {
        window.draw(c.first);
    }

    window.draw(nearestPackCircle); // Dessiner le cercle du point le plus proche

    window.display();
}