CXX = g++
CXXFLAGS = -Wall -O2 -std=c++17

OBJ = Main.o Expression.o Tests.o

default: differentiator

%.o: %.cpp Expression.hpp Tests.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

differentiator: $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o differentiator

test: differentiator
	./differentiator test

clean:
	rm -f $(OBJ) *.exe differentiator
