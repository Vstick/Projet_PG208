CXX = g++
CXXFLAGS = -Wall -Wextra -g

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:src/%.cpp=obj/%.o)

EXEC = bin/program

all: $(EXEC)

$(EXEC): $(OBJ) | bin
	$(CXX) -o $@ $^

obj/%.o: src/%.cpp | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<

bin obj:
	mkdir -p $@

clean:
	rm -rf obj bin
