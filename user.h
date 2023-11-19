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
#define NAME_SIZE 10
#define ASSET_FNAME_SIZE 24
#define MAX_FILENAME_LENGTH 24
#define MAX_NAME_LENGTH 10
#define MAX_START_VALUE_LENGTH 6
#define MAX_TIME_ACTIVE_LENGTH 5

int login(char* IP, char* port, char *uid, char *password);
int logout(char* IP, char* port, char* uid, char* password);
int unregister(char* IP, char* port, char *uid, char *password);
void handle_open(char* IP, char* port, char* uid, char* password, char* name, char* asset_fname, int start_value, int timeactive);
void handle_close(char* IP, char* port);
void my_auctions(char* IP, char* port);
void my_bids(char* IP, char* port);
void listAllAuctions(char* IP, char* port);
void show_asset(char* IP, char* port);
void bid(char* IP, char* port);
void show_record(char* IP, char* port);
char* connect_TCP(char* IP, char* port, char* request, char* buffer);
char* connect_UDP(char* IP, char* port, char* request, char* buffer);
int valid_filename(char *filename);

#endif