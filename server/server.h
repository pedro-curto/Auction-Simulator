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
#include <sys/select.h>

#define MAX_BUFFER_SIZE 1024
#define PORT "58011"
//#define IP ""

//int GetBidList(int AID, BIDLIST *list);
void UDPServer();
void TCPServer();
void *GetInAddr(struct sockaddr *sa);
void process_udp_request(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len);
void process_tcp_request(int tcp_socket, char *buffer);
void print_verbose_info(struct sockaddr_in client_addr, const char *protocol);
void handle_login(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len);
void handle_logout(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len);
void handle_unregister(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len);
void handle_myauctions(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len);
void handle_mybids(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len);
void handle_list(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len);
void handle_show_record(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len);
void reply_msg(int udp_socket, struct sockaddr_in client_addr,socklen_t client_addr_len, char* status);
int create_user(char* uid, char* password);
#endif