#include "voronoi.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <limits>

Voronoi::Voronoi(int width, int height, int initialPoints)
    : WIDTH(width), HEIGHT(height), pointsNumber(initialPoints),
      window(sf::VideoMode(width, height), "Voronoi Diagram", sf::Style::Close | sf::Style::Titlebar, sf::ContextSettings(0, 0, 8)),
      gen(dev()), wRand(30.0, width - 30.0), hRand(30.0, height - 30.0), edges(sf::Lines) {

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

    generateVoronoi();

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
    generateVoronoi(); 
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
            float minDistance = std::numeric_limits<float>::infinity();
            int closestNode = -1;

            for (size_t i = 0; i < graphNodes.size(); ++i) {
                float distance = sqrt(pow(graphNodes[i].position.x - mousePos.x, 2) +
                                      pow(graphNodes[i].position.y - mousePos.y, 2));
                if (distance < minDistance) {
                    minDistance = distance;
                    closestNode = static_cast<int>(i);
                }
            }

            if (selectingStartNode) {
                startNode = closestNode;
                selectingStartNode = false;
                std::cout << "Start node selected: " << startNode << std::endl;
            } else {
                endNode = closestNode;
                selectingStartNode = true;
                std::cout << "End node selected: " << endNode << std::endl;
            }
        }

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A) {
            if (graphNodes.size() > 1 && startNode != -1 && endNode != -1) {
                std::vector<int> path = aStar(startNode, endNode);

                // Debugging: Print the path
                std::cout << "Path: ";
                for (int node : path) {
                    std::cout << node << " ";
                }
                std::cout << std::endl;

                displayPath(path);
            }
        }
    }
}



void Voronoi::update() {
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

    window.draw(edges);
    
    for (auto& c : circles) {
        window.draw(c.first);
    }

    window.display();
}

void Voronoi::generateVoronoi() {
    edges.clear();
    graphNodes.clear();

    std::vector<point_data<float>> inputPoints;
    for (const auto& point : coordinates) {
        inputPoints.emplace_back(point.x, point.y);
    }

    voronoi_diagram<double> vd;
    construct_voronoi(inputPoints.begin(), inputPoints.end(), &vd);

    std::unordered_map<const voronoi_diagram<double>::vertex_type*, int> vertexIndexMap;
    int vertexCount = 0;

    for (auto it = vd.vertices().begin(); it != vd.vertices().end(); ++it) {
        const voronoi_diagram<double>::vertex_type& vertex = *it;
        float vx = vertex.x();
        float vy = vertex.y(); 
        graphNodes.emplace_back(sf::Vector2f(vx, vy));
        vertexIndexMap[&vertex] = vertexCount++;
    }

    for (auto it = vd.cells().begin(); it != vd.cells().end(); ++it) {
        const voronoi_diagram<double>::cell_type& cell = *it;
        const voronoi_diagram<double>::edge_type* edge = cell.incident_edge();
        int site_index = cell.source_index();

        do {
            if (edge->is_primary()) {
                const voronoi_diagram<double>::vertex_type* v0 = edge->vertex0();
                const voronoi_diagram<double>::vertex_type* v1 = edge->vertex1();
                if (v0 && v1) {
                    int idx0 = vertexIndexMap[v0];
                    int idx1 = vertexIndexMap[v1];

                    // Compute distance from edge midpoint to Voronoi site (generator point)
                    float site_x = inputPoints[site_index].x();
                    float site_y = inputPoints[site_index].y();
                    float midpoint_x = (v0->x() + v1->x()) / 2.0;
                    float midpoint_y = (v0->y() + v1->y()) / 2.0;
                    float distance_to_site = sqrt(pow(midpoint_x - site_x, 2) + pow(midpoint_y - site_y, 2));

                    // Use distance to site as weight (store negative for max-heap behavior)
                    graphNodes[idx0].neighbors.emplace_back(idx1, -distance_to_site);
                    graphNodes[idx1].neighbors.emplace_back(idx0, -distance_to_site);

                    float v0_x = v0->x();
                    float v0_y = v0->y(); // Flip y-coordinate
                    float v1_x = v1->x();
                    float v1_y = v1->y(); // Flip y-coordinate
                    edges.append(sf::Vertex(sf::Vector2f(v0_x, v0_y), sf::Color::Red));
                    edges.append(sf::Vertex(sf::Vector2f(v1_x, v1_y), sf::Color::Red));
                }
            }
            edge = edge->next();
        } while (edge != cell.incident_edge());
    }
}



std::vector<int> Voronoi::aStar(int startNode, int endNode) {
    std::cout << "Starting A* from node " << startNode << " to node " << endNode << std::endl;

    std::vector<float> gScore(graphNodes.size(), std::numeric_limits<float>::infinity());
    std::vector<float> fScore(graphNodes.size(), std::numeric_limits<float>::infinity());
    std::vector<int> cameFrom(graphNodes.size(), -1);

    auto heuristic = [&](int node) {
        return sqrt(pow(graphNodes[node].position.x - graphNodes[endNode].position.x, 2) + 
                    pow(graphNodes[node].position.y - graphNodes[endNode].position.y, 2));
    };

    gScore[startNode] = 0.0f;
    fScore[startNode] = heuristic(startNode);

    using PQElement = std::pair<float, int>;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> openSet;
    openSet.emplace(fScore[startNode], startNode);

    std::unordered_set<int> openSetHash;
    openSetHash.insert(startNode);

    std::unordered_set<int> closedSet;

    while (!openSet.empty()) {
        int current = openSet.top().second;
        openSet.pop();

        if (closedSet.find(current) != closedSet.end()) {
            continue; // Skip this node if it has already been processed
        }
        
        closedSet.insert(current);

        std::cout << "Processing node " << current << std::endl;

        if (current == endNode) {
            std::vector<int> path;
            for (int at = endNode; at != -1; at = cameFrom[at]) {
                path.push_back(at);
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (auto& [neighbor, weight] : graphNodes[current].neighbors) {
            if (closedSet.find(neighbor) != closedSet.end()) {
                continue; // Skip neighbors that have already been processed
            }

            float tentative_gScore = gScore[current] + weight;
            if (tentative_gScore < gScore[neighbor]) {
                cameFrom[neighbor] = current;
                gScore[neighbor] = tentative_gScore;
                fScore[neighbor] = gScore[neighbor] + heuristic(neighbor);

                if (openSetHash.find(neighbor) == openSetHash.end()) {
                    openSet.emplace(fScore[neighbor], neighbor);
                    openSetHash.insert(neighbor);
                }
            }
        }
    }

    std::cerr << "No path found!" << std::endl;
    return {}; // Return empty path if no path found
}




void Voronoi::displayPath(const std::vector<int>& path) {
    sf::RenderWindow pathWindow(sf::VideoMode(WIDTH, HEIGHT), "A* Path", sf::Style::Close | sf::Style::Titlebar);
    pathWindow.setPosition(sf::Vector2i(0, 0));
    while (pathWindow.isOpen()) {
        sf::Event event;
        while (pathWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                pathWindow.close();
            }
        }

        pathWindow.clear(sf::Color::White);

        std::vector<sf::Vector2f> copy(coordinates.size());
        std::transform(coordinates.begin(), coordinates.end(), copy.begin(), [&](sf::Vector2f vec) {
            return sf::Vector2f(vec.x, pathWindow.getSize().y - vec.y);
        });

        shader.setUniform("size", pointsNumber);
        shader.setUniformArray("seeds", copy.data(), MAX_POINTS_NUMBER);

    #ifdef COLORS
        shader.setUniformArray("colors", colors.data(), MAX_POINTS_NUMBER);
    #endif

        pathWindow.draw(sf::RectangleShape(sf::Vector2f(WIDTH, HEIGHT)), &shader);


        // Draw points
        for (auto& c : circles) {
            pathWindow.draw(c.first);
        }



        // Draw the path
        for (size_t i = 1; i < path.size(); ++i) {
            sf::Vertex line[] = {
                sf::Vertex(graphNodes[path[i-1]].position, sf::Color::Red),
                sf::Vertex(graphNodes[path[i]].position, sf::Color::Red)
            };
            pathWindow.draw(line, 10, sf::Lines);
        }

        // Draw start and end nodes
        if (!path.empty()) {
            sf::CircleShape startCircle(5);
            startCircle.setFillColor(sf::Color::Blue);
            startCircle.setOrigin(5, 5);
            startCircle.setPosition(graphNodes[path.front()].position);
            pathWindow.draw(startCircle);

            sf::CircleShape endCircle(5);
            endCircle.setFillColor(sf::Color::Red);
            endCircle.setOrigin(5, 5);
            endCircle.setPosition(graphNodes[path.back()].position);
            pathWindow.draw(endCircle);
        }

        pathWindow.display();
    }
}

