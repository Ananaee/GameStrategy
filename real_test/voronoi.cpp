#include "voronoi.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <queue>
#include <unordered_map>
#include <functional>
#include <boost/polygon/voronoi.hpp>



Voronoi::Voronoi(int width, int height, int initialPoints)
    : WIDTH(width), HEIGHT(height), pointsNumber(initialPoints),
      window(sf::VideoMode(width, height), "Voronoi Diagram", sf::Style::Close | sf::Style::Titlebar, sf::ContextSettings(0, 0, 8)),
      gen(dev()), wRand(30.0, width - 30.0), hRand(30.0, height - 30.0) {

    coordinates.resize(pointsNumber);
    std::generate(coordinates.begin(), coordinates.end(), [&]() { return sf::Vector2f(wRand(gen), hRand(gen)); });

    for (auto& coord : coordinates) {
        voronoiPoints.push_back(new VoronoiPoint(coord.x, coord.y));
        addPoint(coord);
    }

    // generateGraph();
    generateEdges();
    

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
    generateEdges();
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
    // generateEdges();
    while (window.isOpen()) {
        
        handleEvents();
        update();
        render();
    }
}

void Voronoi::addPoint(sf::Vector2f position) {
    coordinates.push_back(position);
    VoronoiPoint* newPoint = new VoronoiPoint(position.x, position.y);
    voronoiPoints.push_back(newPoint);
    addPoint(position.x, position.y);
    // generateGraph();

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

void Voronoi::generateGraph(Graph& graph) {
    for (const auto& edge : edges) {
        addEdge(graph, edge.startPoint, edge.endPoint);
    }
}
VoronoiPoint::VoronoiPoint(float x, float y) : location(x, y), region(nullptr) {}

VoronoiEdge::VoronoiEdge(VoronoiPoint* s, VoronoiPoint* e, const sf::Vector2f& sp, const sf::Vector2f& ep,const float dist)
    : start(s), end(e), startPoint(sp), endPoint(ep), distance(std::hypot(sp.x - ep.x, sp.y - ep.y)) {}

VoronoiRegion::VoronoiRegion(VoronoiPoint* p) : point(p) {}

void addEdge(Graph& graph, const sf::Vector2f& start, const sf::Vector2f& end) {
    auto& startVertex = graph.vertices[start];
    auto& endVertex = graph.vertices[end];
    startVertex.position = start;
    endVertex.position = end;

    float distance = std::hypot(end.x - start.x, end.y - start.y);
    startVertex.neighbors.emplace_back(&endVertex, distance);
    endVertex.neighbors.emplace_back(&startVertex, distance);
}




// void addEdge(Graph& graph, const sf::Vector2f& start, const sf::Vector2f& end) {
//     float distance = std::hypot(start.x - end.x, start.y - end.y);

//     if (graph.vertices.find(start) == graph.vertices.end()) {
//         graph.vertices[start] = Vertex{ start };
//     }

//     if (graph.vertices.find(end) == graph.vertices.end()) {
//         graph.vertices[end] = Vertex{ end };
//     }

//     graph.vertices[start].neighbors.push_back({ &graph.vertices[end], distance });
//     graph.vertices[end].neighbors.push_back({ &graph.vertices[start], distance });
// }
// std::vector<sf::Vector2f> aStar(Graph& graph, const sf::Vector2f& start, const sf::Vector2f& goal) {
//     auto heuristic = [](const sf::Vector2f& a, const sf::Vector2f& b) {
//         return std::hypot(a.x - b.x, a.y - b.y);
//     };

//     std::priority_queue<std::pair<float, sf::Vector2f>, std::vector<std::pair<float, sf::Vector2f>>, std::greater<>> openSet;
//     std::unordered_map<sf::Vector2f, sf::Vector2f> cameFrom;
//     std::unordered_map<sf::Vector2f, float> gScore;
//     std::unordered_map<sf::Vector2f, float> fScore;

//     for (const auto& vertex : graph.vertices) {
//         gScore[vertex.first] = std::numeric_limits<float>::infinity();
//         fScore[vertex.first] = std::numeric_limits<float>::infinity();
//     }

//     gScore[start] = 0;
//     fScore[start] = heuristic(start, goal);
//     openSet.push({ fScore[start], start });

//     while (!openSet.empty()) {
//         sf::Vector2f current = openSet.top().second;
//         openSet.pop();

//         if (current == goal) {
//             std::vector<sf::Vector2f> path;
//             while (cameFrom.find(current) != cameFrom.end()) {
//                 path.push_back(current);
//                 current = cameFrom[current];
//             }
//             path.push_back(start);
//             std::reverse(path.begin(), path.end());
//             return path;
//         }

//         for (const auto& neighbor : graph.vertices[current].neighbors) {
//             float tentative_gScore = gScore[current] + neighbor.second;

//             if (tentative_gScore < gScore[neighbor.first->position]) {
//                 cameFrom[neighbor.first->position] = current;
//                 gScore[neighbor.first->position] = tentative_gScore;
//                 fScore[neighbor.first->position] = gScore[neighbor.first->position] + heuristic(neighbor.first->position, goal);

//                 openSet.push({ fScore[neighbor.first->position], neighbor.first->position });
//             }
//         }
//     }

//     return {}; // Return an empty path if there is no path to the goal
// }
std::vector<VoronoiEdge*> Voronoi::aStar(VoronoiRegion* start, VoronoiRegion* goal) {
    auto heuristic = [](VoronoiRegion* a, VoronoiRegion* b) {
        return std::hypot(a->point->location.x - b->point->location.x, a->point->location.y - b->point->location.y);
    };

    std::unordered_map<VoronoiRegion*, float> gScore;
    std::unordered_map<VoronoiRegion*, float> fScore;
    std::unordered_map<VoronoiRegion*, VoronoiEdge*> cameFrom;
    std::priority_queue<std::pair<float, VoronoiRegion*>, std::vector<std::pair<float, VoronoiRegion*>>, std::greater<>> openSet;

    for (auto& region : regions) {
        gScore[&region] = std::numeric_limits<float>::infinity();
        fScore[&region] = std::numeric_limits<float>::infinity();
    }

    gScore[start] = 0;
    fScore[start] = heuristic(start, goal);
    openSet.emplace(fScore[start], start);

    while (!openSet.empty()) {
        VoronoiRegion* current = openSet.top().second;
        openSet.pop();

        if (current == goal) {
            std::vector<VoronoiEdge*> path;
            while (cameFrom.find(current) != cameFrom.end()) {
                path.push_back(cameFrom[current]);
                current = cameFrom[current]->start->region == current ? cameFrom[current]->end->region : cameFrom[current]->start->region;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (VoronoiEdge* edge : current->edges) {
            VoronoiRegion* neighbor = edge->start->region == current ? edge->end->region : edge->start->region;
            float tentative_gScore = gScore[current] + edge->distance;

            if (tentative_gScore < gScore[neighbor]) {
                cameFrom[neighbor] = edge;
                gScore[neighbor] = tentative_gScore;
                fScore[neighbor] = gScore[neighbor] + heuristic(neighbor, goal);
                openSet.emplace(fScore[neighbor], neighbor);
            }
        }
    }

    return {}; // Return an empty path if there is no path to the goal
}

void Voronoi::handleEvents() {
    sf::Event event;
    static VoronoiRegion* startRegion = nullptr;
    static VoronoiRegion* goalRegion = nullptr;

    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            for (auto& point : voronoiPoints) {
                float distance = std::hypot(point->location.x - mousePos.x, point->location.y - mousePos.y);
                if (distance < 5.0f) { // Adjust the threshold for selection
                    if (!startRegion) {
                        startRegion = point->region;
                        // Optionally, visualize the selected region
                        // You can draw something around the selected point here
                        // Update the color of the point to indicate selection
                        // point->color = sf::Color::Blue;
                    } else if (!goalRegion && point->region != startRegion) {
                        goalRegion = point->region;
                        // Optionally, visualize the selected region
                        // You can draw something around the selected point here
                        // Update the color of the point to indicate selection
                        // point->color = sf::Color::Red;

                        // Compute the shortest path
                        std::vector<VoronoiEdge*> path = aStar(startRegion, goalRegion);
                        
                        if (path.empty()) {
                            std::cerr << "No path found between the selected regions." << std::endl;
                        } else {
                            // Display the path in a new window
                            displayPath(path);
                        }

                        // Reset the selection
                        startRegion = nullptr;
                        goalRegion = nullptr;
                        for (auto& point : voronoiPoints) {
                            // Reset the color of all points to black
                            // point->color = sf::Color::Black;
                        }
                    }
                    break;
                }
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            bool outOfCircle = std::none_of(voronoiPoints.begin(), voronoiPoints.end(), [&](const auto& point) {
                return point->location.x == event.mouseButton.x && point->location.y == event.mouseButton.y;
            });

            if (outOfCircle) {
                addPoint(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
            }
        }
    }
}





// void Voronoi::handleEvents() {
//     sf::Event event;
//     static VoronoiRegion* startRegion = nullptr;
//     static VoronoiRegion* goalRegion = nullptr;

//     while (window.pollEvent(event)) {
//         if (event.type == sf::Event::Closed) {
//             window.close();
//         }

//         if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
//             sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
//             for (auto& circle : circles) {
//                 if (circle.first.getGlobalBounds().contains(mousePos)) {
//                     if (!startRegion) {
//                         startRegion = voronoiPoints[&circle - &circles[0]]->region;
//                         circle.first.setFillColor(sf::Color::Blue);  // Indicate start point
//                     } else if (!goalRegion) {
//                         goalRegion = voronoiPoints[&circle - &circles[0]]->region;
//                         circle.first.setFillColor(sf::Color::Red);  // Indicate goal point
                        
//                         // Compute the shortest path
//                         std::vector<VoronoiEdge*> path = aStar(startRegion, goalRegion);
                        
//                         if (path.empty()) {
//                             std::cerr << "No path found between the selected regions." << std::endl;
//                         } else {
//                             // Display the path in a new window
//                             displayPath(path);
//                         }

//                         // Reset the selection
//                         startRegion = nullptr;
//                         goalRegion = nullptr;
//                         for (auto& circle : circles) {
//                             circle.first.setFillColor(sf::Color::Black);
//                         }
//                     }
//                     break;
//                 }
//             }
//         }

//         if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
//             bool outOfCircle = std::none_of(circles.begin(), circles.end(), [&](const auto& circle) {
//                 return circle.first.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(window)));
//             });

//             if (outOfCircle) {
//                 addPoint(sf::Vector2f(sf::Mouse::getPosition(window)));
//             }
//         }
//     }
// }


//         if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
//             bool outOfCircle = std::none_of(circles.begin(), circles.end(), [&](const auto& circle) {
//                 return circle.first.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(window)));
//             });

//             if (outOfCircle) {
//                 addPoint(sf::Vector2f(sf::Mouse::getPosition(window)));
//             }
//         }
//     }
// }

// void Voronoi::handleEvents() {
//     sf::Event event;
//     while (window.pollEvent(event)) {
//         if (event.type == sf::Event::Closed) {
//             window.close();
//         }

//         if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
//             // for (auto& circle : circles) {
//             //     if (circle.first.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(window)))) {
//             //         circle.second = !circle.second;
//             //         circle.first.setOutlineThickness(circle.second ? 2 : 0);
//             //     }
//             // }
//             sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
//             addPoint(mousePos);
//         }

//         if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
//             bool outOfCircle = std::none_of(circles.begin(), circles.end(), [&](const auto& circle) {
//                 return circle.first.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(window)));
//             });

//             if (outOfCircle) {
//                 coordinates.push_back(sf::Vector2f(sf::Mouse::getPosition(window)));
//                 sf::CircleShape tempPoint(4, 100);
//                 tempPoint.setFillColor(sf::Color::Black);
//                 tempPoint.setOrigin(4, 4);
//                 tempPoint.setOutlineColor(sf::Color::Green);
//                 tempPoint.setPosition(coordinates.back());
//                 circles.push_back({tempPoint, false});

// #ifdef COLORS
//                 colors.push_back(sf::Vector3f(frand(gen), frand(gen), frand(gen)));
// #endif

//                 pointsNumber++;
//             }
//         }
//     }
// }




void Voronoi::update() {
    for (int i = 0; i < pointsNumber; i++) {
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

// void Voronoi::render() {
//     window.clear(sf::Color::White);

//     // Draw the original Voronoi diagram
//     std::vector<sf::Vector2f> copy(coordinates.size());
//     std::transform(coordinates.begin(), coordinates.end(), copy.begin(), [&](sf::Vector2f vec) {
//         return sf::Vector2f(vec.x, window.getSize().y - vec.y);
//     });

//     shader.setUniform("size", pointsNumber);
//     shader.setUniformArray("seeds", copy.data(), MAX_POINTS_NUMBER);

// #ifdef COLORS
//     shader.setUniformArray("colors", colors.data(), MAX_POINTS_NUMBER);
// #endif

//     window.draw(sf::RectangleShape(sf::Vector2f(WIDTH, HEIGHT)), &shader);

//     // Draw the circles (Voronoi points)
//     for (auto& point : voronoiPoints) {
//         sf::CircleShape circle(5.f);
//         circle.setOrigin(5.f, 5.f);
//         circle.setPosition(point->location);
//         // Assign a default color to the points
//         circle.setFillColor(sf::Color::Black);
//         window.draw(circle);
//     }

//     // Draw the edges with weighted colors
//     for (VoronoiEdge* edge : voronoiEdges) {
//         float distanceToGuard = edge->distance;
//         sf::Color edgeColor;

//         if (distanceToGuard < 100) {
//             edgeColor = sf::Color::Red;
//         } else if (distanceToGuard < 200) {
//             edgeColor = sf::Color(255, 165, 0); // Orange
//         } else if (distanceToGuard < 300) {
//             edgeColor = sf::Color::Yellow;
//         } else {
//             edgeColor = sf::Color::Green;
//         }

//         sf::Vertex line[] = {
//             sf::Vertex(sf::Vector2f(edge->start->location.x, edge->start->location.y), edgeColor),
//             sf::Vertex(sf::Vector2f(edge->end->location.x, edge->end->location.y), edgeColor)
//         };
//         window.draw(line, 100, sf::Lines);
//     }

//     window.display();
// }




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
    
    // Draw the circles (Voronoi points)
    for (auto& c : circles) {
        window.draw(c.first);
    }
    
    const float RED_THRESHOLD = 10.0;     // Red color for edges within 50 units of a guard tower
    const float ORANGE_THRESHOLD = 500.0; // Orange color for edges within 100 units of a guard tower
    const float YELLOW_THRESHOLD = 100.0;
    // if (voronoiEdges.empty()){
    //     std::cout << "kikou" << std::endl;
    // }
    // Draw the edges with color coding based on their distances to the guard towers
    for (const VoronoiEdge& edge : edges) {
        sf::Vertex line[] = {
            sf::Vertex(edge.startPoint, sf::Color::Black),
            sf::Vertex(edge.endPoint, sf::Color::Black)
        };
        window.draw(line, 2, sf::Lines);
    }

    window.display();
}


// void Voronoi::displayPath(const std::vector<VoronoiEdge*>& path) {
//     sf::RenderWindow pathWindow(sf::VideoMode(WIDTH, HEIGHT), "Shortest Path", sf::Style::Close | sf::Style::Titlebar, sf::ContextSettings(0, 0, 8));

//     while (pathWindow.isOpen()) {
//         sf::Event pathEvent;
//         while (pathWindow.pollEvent(pathEvent)) {
//             if (pathEvent.type == sf::Event::Closed) {
//                 pathWindow.close();
//             }
//         }

//         pathWindow.clear(sf::Color::White);
        
//         // Draw the original Voronoi diagram
//         std::vector<sf::Vector2f> copy(coordinates.size());
//         std::transform(coordinates.begin(), coordinates.end(), copy.begin(), [&](sf::Vector2f vec) {
//             return sf::Vector2f(vec.x, pathWindow.getSize().y - vec.y);
//         });

//         shader.setUniform("size", pointsNumber);
//         shader.setUniformArray("seeds", copy.data(), MAX_POINTS_NUMBER);

// #ifdef COLORS
//         shader.setUniformArray("colors", colors.data(), MAX_POINTS_NUMBER);
// #endif

//         pathWindow.draw(sf::RectangleShape(sf::Vector2f(WIDTH, HEIGHT)), &shader);

//         // Draw the circles (Voronoi points)
//         for (auto& c : circles) {
//             pathWindow.draw(c.first);
//         }

//         // Draw the edges and their weights
//         for (VoronoiEdge* edge : voronoiEdges) {
//             sf::Vertex line[] = {
//                 sf::Vertex(sf::Vector2f(edge->x1, edge->y1), sf::Color::Black),
//                 sf::Vertex(sf::Vector2f(edge->x2, edge->y2), sf::Color::Black)
//             };
//             pathWindow.draw(line, 2, sf::Lines);

//             // Calculate the weight as the distance to the closest Voronoi point
//             float weight = std::min(std::hypot(edge->x1 - edge->pointA->x, edge->y1 - edge->pointA->y),
//                                     std::hypot(edge->x2 - edge->pointB->x, edge->y2 - edge->pointB->y));

//             sf::Text weightText;
//             // weightText.setFont(sf::Font::getDefaultFont()); // Use default font
//             weightText.setString(std::to_string(static_cast<int>(weight))); // Convert weight to int for display
//             weightText.setCharacterSize(12);
//             weightText.setFillColor(sf::Color::Black);
//             weightText.setPosition((edge->x1 + edge->x2) / 2, pathWindow.getSize().y - (edge->y1 + edge->y2) / 2);
//             pathWindow.draw(weightText);
//         }

//         // Draw the path
//         for (VoronoiEdge* edge : path) {
//             sf::Vertex line[] = {
//                 sf::Vertex(sf::Vector2f(edge->x1, edge->y1), sf::Color::Green),
//                 sf::Vertex(sf::Vector2f(edge->x2, edge->y2), sf::Color::Green)
//             };
//             pathWindow.draw(line, 2, sf::Lines);
//         }

//         pathWindow.display();
//     }
// }

void Voronoi::displayPath(const std::vector<VoronoiEdge*>& path) {
    sf::RenderWindow pathWindow(sf::VideoMode(WIDTH, HEIGHT), "Shortest Path", sf::Style::Close | sf::Style::Titlebar, sf::ContextSettings(0, 0, 8));

    while (pathWindow.isOpen()) {
        sf::Event pathEvent;
        while (pathWindow.pollEvent(pathEvent)) {
            if (pathEvent.type == sf::Event::Closed) {
                pathWindow.close();
            }
        }

        pathWindow.clear(sf::Color::White);
        
        // Draw the original Voronoi diagram
        shader.setUniform("size", pointsNumber);
        shader.setUniformArray("seeds", coordinates.data(), MAX_POINTS_NUMBER);

#ifdef COLORS
        shader.setUniformArray("colors", colors.data(), MAX_POINTS_NUMBER);
#endif

        pathWindow.draw(sf::RectangleShape(sf::Vector2f(WIDTH, HEIGHT)), &shader);

        // Draw the circles (Voronoi points)
        for (const auto& point : voronoiPoints) {
            sf::CircleShape circle(3);
            circle.setPosition(point->location);
            circle.setFillColor(sf::Color::Black);
            pathWindow.draw(circle);
        }

        // Draw the edges and their weights
        for (const auto& edge : voronoiEdges) {
            sf::Vertex line[] = {
                sf::Vertex(edge->startPoint, sf::Color::Black),
                sf::Vertex(edge->endPoint, sf::Color::Black)
            };
            pathWindow.draw(line, 2, sf::Lines);

            // Calculate the weight as the distance to the closest Voronoi point
            float weight = edge->distance;

            sf::Text weightText;
            weightText.setString(std::to_string(static_cast<int>(weight))); // Convert weight to int for display
            weightText.setCharacterSize(12);
            weightText.setFillColor(sf::Color::Black);
            weightText.setPosition((edge->startPoint + edge->endPoint) / 2.f);
            pathWindow.draw(weightText);
        }

        // Draw the path
        for (const auto& edge : path) {
            sf::Vertex line[] = {
                sf::Vertex(edge->startPoint, sf::Color::Green),
                sf::Vertex(edge->endPoint, sf::Color::Green)
            };
            pathWindow.draw(line, 2, sf::Lines);
        }

        pathWindow.display();
    }
}


