CC = gcc
OBJS = main.c maze.c
CFLAGS = -w $(shell sdl2-config --cflags)
LDFLAGS = -lSDL2 -lSDL2_image $(shell sdl2-config --libs)
OBJFiles = maze.o main.o
TARGET = editor

all : $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(OBJS) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS) 

clean:
	rm -f $(OBJFILES) $(TARGET) *~