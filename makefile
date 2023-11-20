#Makefile
CC = gcc
user: main.c functions.c aux_functions.c
	$(CC) main.c functions.c aux_functions.c -o user


#CFLAGS = -Wall -Wextra -Werror -pedantic 
#	$(CC) $(CFLAGS) user.c -o user