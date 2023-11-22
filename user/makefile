# user/Makefile

CC = gcc
# CFLAGS = -Wall -Wextra -Werror -pedantic
SRC = user.c functions.c aux_functions.c
TARGET = user

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

