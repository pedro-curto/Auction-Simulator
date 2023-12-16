# user/Makefile
CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wunused -pedantic
SRC = *.c
TARGET = client

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)
clean:
	rm -f $(TARGET)