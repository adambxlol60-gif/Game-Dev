CXX = g++
FLAGS = -I/opt/homebrew/Cellar/allegro/5.2.11.3/include \
        -L/opt/homebrew/Cellar/allegro/5.2.11.3/lib \
        -lallegro_ttf -lallegro_font -lallegro_image \
        -lallegro_primitives -lallegro_acodec -lallegro_audio \
        -lallegro_dialog -lallegro_main -lallegro

all:
	$(CXX) -o game main.cpp enemy.cpp $(FLAGS)

run: all
	./game

clean:
	rm -f game
