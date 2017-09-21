CC=g++
CFLAGS=-c -Wall -pedantic -O3 -std=gnu++0x
LDFLAGS=
SOURCES=bfind.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=bfind

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o bfind
