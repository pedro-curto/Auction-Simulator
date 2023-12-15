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
#include <sys/stat.h>
#include <time.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <ctype.h>
#include <pthread.h>

#define MAX_BUFFER_SIZE 1024
#define PORT "58063"
#define SEPARATION_CHAR ' '
#define COMMAND_SIZE 3
#define UID_SIZE 6
#define PASSWORD_SIZE 8
#define AID_SIZE 3
#define VALUE_SIZE 6
#define TIMEACTIVE_SIZE 5
#define ASSET_FNAME_SIZE 24
#define MAX_ASSET_FSIZE 10000000
#define ASSET_FSIZE_SIZE 8
#define NAME_SIZE 10


//#define IP ""

// verbose_mode global variable
extern int verbose_mode;
//mutex
extern pthread_mutex_t mutex;

typedef struct {
    char uid[7];
    int value;
    char datetime[20];
    time_t bidtime; 
} bid;


typedef struct {
    bid bids[50];
    int num_bids;
} bidlist;


//int GetBidList(int AID, BIDLIST *list);
//void UDPServer();
//void TCPServer();
//void *GetInAddr(struct sockaddr *sa);
void process_udp_request(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len);
void process_tcp_request(int tcp_socket);
void* process_udp_thread(void* arg);
void* process_tcp_thread(void* arg);
void print_verbose_info(struct sockaddr_in client_addr, const char *protocol);
void handle_login(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len);
void handle_logout(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len);
void handle_unregister(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len);
void handle_myauctions(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len);
void handle_mybids(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len);
void handle_list(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len);
void handle_show_record(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len);
void handle_open(int tcp_socket);
void handle_close(int tcp_socket);
void handle_show_asset(int tcp_socket);
void handle_bid(int tcp_socket);
void reply_msg(int udp_socket, struct sockaddr_in client_addr,socklen_t client_addr_len, char* status);
int create_user(char* uid, char* password);
void reply_msg_tcp(int udp_socket, char* status);

//change the login info of user uid to status. status = 1 means login, status = 0 means logout
void change_user_login(char* uid);
int is_user_login(char* uid);
int verify_user_exists(char* uid);
int verify_password_correct(char* uid, char* password);
void delete_user(char* uid);
int user_auc_status(char* uid, char* status);
void fetch_auctions(char* path, char* status);
int is_auc_active(char* auc_uid);
int read_field(int tcp_socket, char *buffer, size_t size);
int store_file(int tcp_socket, int size, char* path);
int create_auction(int tcp_socket, char* uid, char* name, char* asset_fname, int start_value, int timeactive, int fsize);
int folder_exists(char* path);
void append_auctions(char* status);
int exists_auctions();
int exists_auction(char* auc_id);
int get_auc_file_info(char* auc_id, char* status);
int send_auc_file(int tcp_socket, char* auc_id);
int get_next_auction_id();
int bid_accepted(int auction_id, int value, char* uid);
int ongoing_auction(int auction_id);
int hosted_by_self(int auction_id, char* uid);
int create_bid_files(int auction_id, int value, char* uid, time_t start_fulltime);
int close_auction(int auction_id);
void write_tcp(int tcp_socket, char* status);
int is_directory_empty(char* path);
int user_bids_status(char* uid, char* status);
void get_auc_info(int auc_id, char* status);
int GetBidList();
int loadBid(char* path, bidlist *list);

//input_funcs
int read_command_udp(char* input, char* command);
int read_uid_udp(char* input, char* uid);
int read_password_udp(char* input, char* password);
int read_aid_udp(char* input, char* aid);
int verify_uid(char* uid);
int verify_password(char* password);
int verify_aid(char* aid);
int verify_name(char* name);
int verify_start_value(char* start_value);
int verify_timeactive(char* timeactive);
int verify_asset_fname(char* asset_fname);
int verify_asset_fsize(char* fsize);
int verify_bid_value(char* value);
// int verify_bid_datetime(char* datetime);
int verify_open_args(char* uid, char* password, char* name, char* start_valueStr, char* timeactiveStr, char* asset_fname, char* fsizeStr);
int verify_bid_args(char* uid, char* password, char* aid, char* value);
int verify_close_args(char* uid, char* password, char* aid);


#endif
