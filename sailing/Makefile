TARGET = bin/sailing

LIBS = -lm -lbsd
GLIBL = $(shell pkg-config --libs glib-2.0)
PQLIBL = -L$(shell pg_config --libdir) -lpq
CC = gcc
CFLAGS = -g -Wall
GLIBC = $(shell pkg-config --cflags glib-2.0)
PQLIB = -I$(shell pg_config --includedir)

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = src/main.o src/llist.o src/test.o src/sailing.o

.PRECIOUS: $(TARGET) $(OBJECTS)

%.o: %.c %.h
	$(CC) $(CFLAGS) $(GLIBC) $(PQLIB) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) $(GLIBL) $(PQLIBL) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)

run: clean $(TARGET)
	./$(TARGET) $(ARGS)

test: clean $(TARGET)
	./$(TARGET) -test

demo: clean $(TARGET)
	./$(TARGET) -demo
