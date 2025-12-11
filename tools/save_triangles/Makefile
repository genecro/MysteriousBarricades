CXX = g++
CXXFLAGS = -std=c++23 -Wall -O2 -I./tinygltf

all: save_triangles

save_triangles: save_triangles.cpp
	$(CXX) $(CXXFLAGS) save_triangles.cpp -o save_triangles

clean:
	rm -f save_triangles