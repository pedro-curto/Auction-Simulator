#Makefile
CC = gcc
user: user.c
	$(CC) user.c -o user
 
1: main.c functions.c aux_functions.c
	$(CC) main.c functions.c aux_functions.c -o user1


#CFLAGS = -Wall -Wextra -Werror -pedantic 
#	$(CC) $(CFLAGS) user.c -o user