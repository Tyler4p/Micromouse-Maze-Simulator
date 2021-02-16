CC = gcc
OBJS = main.c maze.c mouse.c solver.c
CFLAGS = -w $(shell sdl2-config --cflags)
LDFLAGS = -lSDL2 -lSDL2_image $(shell sdl2-config --libs)
OBJFiles = maze.o main.o mouse.o solver.o
TARGET = simulation

all : $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(OBJS) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS) 

clean:
	rm -f $(OBJFILES) $(TARGET) *~
