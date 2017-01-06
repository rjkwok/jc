
all: clean
	g++ -std=c++11 -Iinclude -Llib -lBox2D -lsfml-main -lsfml-system -lsfml-window -lsfml-graphics -lsfml-audio -lsfml-network -ltinylib src/main.cpp -o debug

clean:
	-rm *.exe