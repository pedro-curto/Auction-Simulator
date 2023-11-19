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
#include <fcntl.h>
#include <sys/stat.h>

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
#define MAX_FILESIZE 10000000 // 10 MB

int login(char* IP, char* port, char* uid, char* password, char* input);
int logout(char* IP, char* port, char* uid, char* password);
int unregister(char* IP, char* port, char *uid, char *password);
void openAuction(char* IP, char* port, char* uid, char* password, char* input);
void closeAuction(char* IP, char* port, char* uid, char* password, char* input);

/*myauctions or ma – the User application sends a message to the AS, using
the UDP protocol, asking for a list of the auctions started by the logged in user,
or auctions in which the logged in user has placed a bid.
The AS will reply with the requested list, or an information that the user is not
involved in any of the currently active auctions. This information should be
displayed to the User.*/
void myAuctions(char* IP, char* port, char* uid, char* password, int aid);

/*mybids or mb – the User application sends a message to the AS, using the
UDP protocol, asking for a list of the auctions for which the logged in user has
placed a bid.
The AS will reply with the requested list, or an information that the user has no
active auction bids. This information should be displayed to the User. */
void myBids(char* IP, char* port, char* uid, char* password);


void listAllAuctions(char* IP, char* port);
void showAsset(char* IP, char* port);
void bid(char* IP, char* port);
void showRecord(char* IP, char* port);
char* connect_TCP(char* IP, char* port, char* request, char* buffer);
char* connect_UDP(char* IP, char* port, char* request, char* buffer);
int valid_filename(char *filename);
off_t get_file_size(char *filename);
int read_file(char *filename, char *buffer, off_t size);

#endif