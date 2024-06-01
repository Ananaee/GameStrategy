#include "voronoi.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

Voronoi::Voronoi(int width, int height, int initialPoints)
    : WIDTH(width), HEIGHT(height), pointsNumber(initialPoints),
      window(sf::VideoMode(width, height), "Voronoi Diagram", sf::Style::Close | sf::Style::Titlebar, sf::ContextSettings(0, 0, 8)),
      gen(dev()), wRand(30.0, width - 30.0), hRand(30.0, height - 30.0) {

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
    }
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

#ifdef COLORS
    colors.push_back(sf::Vector3f(frand(gen), frand(gen), frand(gen)));
#endif

    pointsNumber++;
}

void Voronoi::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            // for (auto& circle : circles) {
            //     if (circle.first.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(window)))) {
            //         circle.second = !circle.second;
            //         circle.first.setOutlineThickness(circle.second ? 2 : 0);
            //     }
            // }
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            addPoint(mousePos);
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

    window.display();
}
