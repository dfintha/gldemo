BINARY=bin/gldemo
OBJECTS=obj/program.o 				\
		obj/engine/camera.o 		\
		obj/engine/shader.o 		\
		obj/engine/shader_program.o	\
		obj/engine/texture.o		\
		obj/geometry/vector.o 		\
		obj/geometry/matrix.o 		\
		obj/objects/cylinder.o 	    \
		obj/objects/rectangle.o 	\
		obj/objects/sphere.o 		\
		obj/objects/obj_model.o     \
		obj/objects/objects.o

CXX=g++
CXXFLAGS=-std=c++14 -O2 -Wall -Wextra -Werror -pedantic -ffast-math -I./src
CXXFLAGS+=`pkg-config --cflags gl glew freeglut`
LDFLAGS=`pkg-config --libs gl glew freeglut`

.PHONY: all, clean

all: $(BINARY)

loc:
	@printf "The program consists of %s lines of code.\n" \
			`find . -iname '*.[ch]pp' | xargs cat | wc -l`

run: $(BINARY)
	@$(BINARY)

$(BINARY): $(OBJECTS)
	@mkdir -p bin
	@printf "Linking executable $@.\n"
	@$(CXX) $(OBJECTS) -o $(BINARY) $(LDFLAGS)
	@printf "Stripping external symbols from executable $@.\n"
	@strip $(BINARY)
	@printf "Build succeeded!\n"

obj/%.o: src/%.cpp
	@mkdir -p obj
	@printf "Compiling C/C++ source $<.\n"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

obj/engine/%.o: src/engine/%.cpp
	@mkdir -p obj/engine
	@printf "Compiling C/C++ source $<.\n"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

obj/geometry/%.o: src/geometry/%.cpp
	@mkdir -p obj/geometry
	@printf "Compiling C/C++ source $<.\n"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

obj/objects/%.o: src/objects/%.cpp
	@mkdir -p obj/objects
	@printf "Compiling C/C++ source $<.\n"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@printf "Removing temporary files and build executable.\n"
	@rm -rf bin obj
