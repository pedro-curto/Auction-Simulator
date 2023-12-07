# server/Makefile

CC = gcc
#CFLAGS = -Wall -Wextra -Werror -pedantic
SRC =server.c udp_functions.c tcp_functions.c aux_functions.c
TARGET = server

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

