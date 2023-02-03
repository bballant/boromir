TARGET = bin/boromir

LIBS = -lm
CC = gcc
CFLAGS = -g -Wall

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = src/main.o src/tcp.o src/ds.o

.PRECIOUS: $(TARGET) $(OBJECTS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)

run: clean $(TARGET)
	./$(TARGET) $(ARGS)

run-client: clean $(TARGET)
	./$(TARGET) -client

run-server: clean $(TARGET)
	./$(TARGET) -server

run-test: clean $(TARGET)
	./$(TARGET) -test
