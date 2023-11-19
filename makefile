#Makefile
CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic 

user: user.c user.h
	$(CC) $(CFLAGS) user.c -o user
