# user/Makefile
CC = gcc
#CFLAGS = -Wall -Wextra -Werror -pedantic
SRC = client.c functions.c aux_functions.c
TARGET = user

$(TARGET): $(SRC)
#$(CC) $(CFLAGS) $(SRC) -o $(TARGET)
	$(CC) $(SRC) -o $(TARGET)
clean:
	rm -f $(TARGET)