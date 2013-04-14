molecules.o: main.cpp
	g++ main.cpp -framework OpenGL -framework GLUT -o ./molecules.out
	./molecules.out