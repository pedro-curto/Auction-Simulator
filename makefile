#Makefile

user: user.c user.h
	gcc user.c user.h -o user
	./user