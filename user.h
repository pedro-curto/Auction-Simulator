#ifndef USER_H
#define USER_H

#define SERVER_IP "tejo.tecnico.ulisboa.pt"
#define GN 63 
#define PORT "58063" 

void login(char* IP, char* port, char *username, char *password);
void logout(char* IP, uint16_t port);
void unregister(char* IP, uint16_t port);
void handle_exit(char* IP, uint16_t port);
void handle_open(char* IP, uint16_t port);
void handle_close(char* IP, uint16_t port);
void my_auctions(char* IP, uint16_t port);
void my_bids(char* IP, uint16_t port);
void list(char* IP, uint16_t port);
void show_asset(char* IP, uint16_t port);
void bid(char* IP, uint16_t port);
void show_record(char* IP, uint16_t port);

#endif