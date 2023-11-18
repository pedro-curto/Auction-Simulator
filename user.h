#ifndef USER_H
#define USER_H

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#define SERVER_name "tejo.tecnico.ulisboa.pt"
#define SERVER_IP "193.136.138.142"
#define PORT "58011" 
#define UID_SIZE 6
#define PASSWORD_SIZE 8

void login(char* IP, char* port, char *uid, char *password);
void logout(char* IP, char* port, char* uid, char* password);
void unregister(char* IP, char* port, char *uid, char *password);
void handle_exit(char* IP, char* port);
void handle_open(char* IP, char* port);
void handle_close(char* IP, char* port);
void my_auctions(char* IP, char* port);
void my_bids(char* IP, char* port);
void list(char* IP, char* port);
void show_asset(char* IP, char* port);
void bid(char* IP, char* port);
void show_record(char* IP, char* port);
char* connect_TCP(char* IP, char* port, char* request, char* buffer);
char* connect_UDP(char* IP, char* port, char* request, char* buffer);

#endif