CXX = g++
CXXFLAGS = $(shell pkg-config --cflags allegro-5 allegro_font-5 allegro_ttf-5 allegro_image-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 allegro_dialog-5 allegro_main-5)
LIBS = $(shell pkg-config --libs allegro-5 allegro_font-5 allegro_ttf-5 allegro_image-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 allegro_dialog-5 allegro_main-5)

SRC = main.cpp enemy.cpp
TARGET = game

all:
	$(CXX) -o $(TARGET) $(SRC) $(CXXFLAGS) $(LIBS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
