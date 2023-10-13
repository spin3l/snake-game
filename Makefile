CC = gcc
CFLAGS = -Wall -Wextra
INCLUDES = -I/usr/include/SDL2/
LIBS = -lSDL2 -lSDL2_ttf
SRCS = ./src/snake.c
OBJS = $(SRCS:.c=.o)
MAIN = snake

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LIBS)
	
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	
clean:
	$(RM) *.o *~ $(MAIN)
