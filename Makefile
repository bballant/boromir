TARGET = bin/boromir

LIBS = -lm
GLIBL = $(shell pkg-config --libs glib-2.0)
CC = gcc
CFLAGS = -g -Wall 
GLIBC = $(shell pkg-config --cflags glib-2.0)

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = src/main.o src/tcp.o src/ds.o src/llist.o src/hlist.o src/test.o src/bstree.o

.PRECIOUS: $(TARGET) $(OBJECTS)

%.o: %.c %.h
	$(CC) $(CFLAGS) $(GLIBC) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) $(GLIBL) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)

run: clean $(TARGET)
	./$(TARGET) $(ARGS)

run-client: clean $(TARGET)
	./$(TARGET) -client

run-server: clean $(TARGET)
	./$(TARGET) -server

test: clean $(TARGET)
	./$(TARGET) -test
