#ifndef CLIENT_H
#define CLIENT_H

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
#include <sys/sendfile.h>

#define SERVER_name "tejo.tecnico.ulisboa.pt"
#define SERVER_IP "193.136.138.142"
#define SERVER_IP_TEJO "tejo"
#define LOCAL_SERVER_IP "192.168.1.1"
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
#define MAX_AUCTIME 99999
#define LST_BUFFER_SIZE // max is RLS OK and then 
#define MA_BUFFER_SIZE 6024// max is RMA OK\n (7) + 6 per auction, so 999*6 we round to 6024 because we can
#define SA_RESPONSE_HEADER 41 // RSA OK Fname------------------- Fsize--- 0\n
#define SA_BUFFER_SIZE 10 // RSA OK Fname------------------- Fsize--- 0\n
//#define SA_BUFFER_SIZE 8// RSA OK + espaço +\0 (8)// RSA OK Fname Fsize Fdata so 7 + 24 + 8 + 1 (we receive Fdata after)

int login(char* IP, char* port, char* uid, char* password, char* input);
int logout(char* IP, char* port, char* uid, char* password);
int unregister(char* IP, char* port, char *uid, char *password);
void openAuction(char* IP, char* port, char* uid, char* password, char* input);
void closeAuction(char* IP, char* port, char* uid, char* password, char* input);
void myAuctions(char* IP, char* port, char* uid);
void myBids(char* IP, char* port, char* uid);
void listAllAuctions(char* IP, char* port);
void showAsset(char* IP, char* port, int aid);
void bid(char* IP, char* port, char* uid, char *password, int aid, int value);
void showRecord(char* IP, char* port, int aid);
void connect_UDP(char* IP, char* port, char* request, char* buffer);
void connect_TCP(char* IP, char* port, char* request, char* buffer, size_t buffer_size);
void SA_connect_TCP(char* IP, char* port, char* request, char* buffer, size_t buffer_size);
int valid_filename(char *filename);
int getFileSize(char *filename);
//int readFile(char *filename, char *buffer, int size);
int sendImage(char *filename, char *buffer, int size);

#endif