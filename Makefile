
all: clean
	g++ -std=c++11 -Iinclude -Llib src/main.cpp -o debug -lBox2D -lsfml-system -lsfml-window -lsfml-graphics -lsfml-audio

clean:
	-rm *.exe