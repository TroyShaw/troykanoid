CC = g++
CFLAGS = -g -Wall -Wextra -pedantic
IPATH = -I/usr/X11/include -I/usr/pkg/include -I./include 
LPATH = -L/usr/X11/lib -L/usr/pkg/lib
LDPATH = -Wl,-R/usr/pkg/lib

LIBS = -lGL -lglut -lGLU -ljpeg -lpng -lm

BUILD = ./build/
SRC = ./src/

all: $(BUILD)Troykanoid

$(BUILD)Troykanoid: $(BUILD)pong.o $(BUILD)main.o $(BUILD)highscore.o $(BUILD)levels.o $(BUILD)physics.o  $(BUILD)powerups.o $(BUILD)renderer.o $(BUILD)sound.o
	$(CC) -o $@ $^ -lm -lGL -lglut -lGLU $(LPATH) $(LDPATH) $(LIBS)
 
$(BUILD)%.o: $(SRC)%.c
	$(CC) $(CFLAGS) -c -o $@ $^ $(IPATH) 

clean:
	$(RM) -f $(BUILD)*.o $(BUILD)*.gch $(BUILD)Troykanoid


