CXX = g++
CXXFLAGS = -Wall -O2 -std=c++17

OBJ = Main.o Expression.o

default: Executable

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

Executable: $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o Executable

runmain: Executable
	./Executable

# %:
#	@:

clean:
	rm -f $(OBJ) *.exe Executable