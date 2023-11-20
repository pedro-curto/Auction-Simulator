#ifndef SERVER_H
#define SERVER_H
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "58011"
//#define IP ""

//int GetBidList(int AID, BIDLIST *list);
void UDPServer();
void TCPServer();
void *GetInAddr(struct sockaddr *sa);

#endif