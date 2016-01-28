CC = g++
CFLAGS = -Wall
PROG = carrom

SRCS = carrom.cpp
LIBS = -lglut -lGL -lGLU -lm -fpermissive -std=c++11 -Wno-int-to-pointer-cast

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)
