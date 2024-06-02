CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -I/usr/include/SFML -DCOLORS -g

LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -L/usr/lib

TARGET = voronoi
OBJECTS = main.o voronoi.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

main.o: main.cpp voronoi.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

voronoi.o: voronoi.cpp voronoi.hpp
	$(CXX) $(CXXFLAGS) -c voronoi.cpp

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean

