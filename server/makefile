# server/Makefile

CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -pthread
SRC = *.c
TARGET = server

$(TARGET): $(SRC)
	rm -f $(TARGET)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

