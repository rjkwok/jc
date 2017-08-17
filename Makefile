
all: clean
	g++ -std=c++11 -static-libstdc++ -static-libgcc -Iinclude -Llib -D SFML_STATIC src/main.cpp -o debug -lBox2D -lsfml-audio-s -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lwinmm -lgdi32 -lFLAC -lfreetype -ljpeg -logg -lopenal32 -lvorbis -lvorbisenc -lvorbisfile

clean:
	-rm *.exe