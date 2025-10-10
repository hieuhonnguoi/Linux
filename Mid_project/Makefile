CC = gcc
CFLAGS = -Wall -O2 -pthread
TARGET = mid_project
SRC = mid_project.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

run: all
	./$(TARGET) 5000

clean:
	rm -f $(TARGET) *.o

.PHONY: all run clean
