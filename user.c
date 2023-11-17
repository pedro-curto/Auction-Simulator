// nc -u <address> <port> ; -u (UDP)
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
#include "user.h"

void login(char* IP, char* port, char* uid, char* password) { // uses UDP protocol
    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128], login_request[100];

    snprintf(login_request, sizeof(login_request), "LIN %s %s", uid, password);
    
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) exit(1);
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt", port, &hints, &res);
    if (errcode != 0) exit(1);

    n = sendto(fd, login_request, strlen(login_request), 0 ,res->ai_addr, res->ai_addrlen);
    if (n == -1) exit(1);

    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, 128, 0,
                 (struct sockaddr*) &addr, &addrlen);
    if (n == -1) exit(1);

    printf("Echo from server: %.*s", (int) n, buffer);

    //write(1, "echo: ", 6);
    //write(1, buffer, n);
    freeaddrinfo(res);
    close(fd);
    
}

/*void logout(char* IP, uint16_t port) {

}

void unregister(char* IP, uint16_t port) {

}

void handle_exit(char* IP, uint16_t port) {

}

void handle_open(char* IP, uint16_t port) { 


}
    
void handle_close(char* IP, uint16_t port) {

}

void my_auctions(char* IP, uint16_t port) {

}

void my_bids(char* IP, uint16_t port) {

}

void list(char* IP, uint16_t port) {

}

void show_asset(char* IP, uint16_t port) {

}

void bid(char* IP, uint16_t port) {

}

void show_record(char* IP, uint16_t port) {
    
}*/


int main(int argc, char *argv[]) {
    char IP[50] = SERVER_IP; 
    char* port = PORT;
    printf("argc: %d\n", argc);
    printf("argv[1]: %s\n", argv[1]);
    printf
    switch(argc) {  
        case 1: 
            break;
        case 3:
            if(!strcmp(argv[1], "-n")) {
                strcpy(IP, argv[2]);
            } else if(!strcmp(argv[2], "-p")) {
                port = argv[5];
            } else {
                printf("Invalid arguments.\n");
                return -1;
            }
            break;
        case 5:
            if(!strcmp(argv[2],"-n") && !strcmp(argv[4],"-p")) {
                strcpy(IP, argv[2]);
                port = argv[5];
            } else {
                printf("Invalid arguments.\n");
                return -1;
            }
            break;
        default: 
            printf("Invalid arguments.\n");
            return -1;
    }

    while (1) {
        char input[100], command[12];
        // gets stdin into input buffer
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Error reading input");
            return -1;
        }
        if (input[strlen(input)-1] == '\n') input[strlen(input)-1] = '\0';
        sscanf(input, "%11s", command);
// bid AID value (or b AID value); close AID; login UID password; logout; list (or l); exit; open name asset_fname start_value timeactive; myauctions; ma; mybids (or mb) ; show_asset AID (or sa AID);  show_record AID (or sr AID); unregister;


        if(!strcmp(command, "login")) {
            char uid[100], password[100];
            int valid = 1;
            sscanf(command, "%*s %s %s", uid, password); // * reads from stream and discards
            if (strlen(uid) != 6 || strlen(password) != 8) valid = 0;
            for (int i = 0; i < 6; i++) {
                if (!isdigit(uid[i])) valid = 0;
            }
            for (int i = 0; i < 8; i++) {
                if (!isalnum(password[i])) valid = 0;
            }
            printf("uid: %s\npassword: %s\n", uid, password);
            if (valid) login(IP, port, uid, password);
            else printf("Incorrect login attempt.\n");

        } /*else if (!strcmp(command, "logout")) {
            logout(IP, port);
        } else if (!strcmp(command, "unregister")) {
            unregister(IP, port);
        } else if (!strcmp(command, "exit")) {
            handle_exit(IP, port);
        } else if (!strcmp(command, "open")) {

            handle_open(IP, port);
        } else if (!strcmp(command, "close")){
            handle_close(IP, port);
        } else if (!strcmp(command, "myauctions") || !strcmp(command, "ma")){
            my_auctions(IP, port);
        } else if (!strcmp(command, "mybids") || !strcmp(command, "mb")){
            my_bids(IP, port);
        } else if (!strcmp(command, "list") || command == 'l') { 
            list(IP, port);
        } else if (!strcmp(command, "show_asset") || !strcmp(command, "sa")){
            show_asset(IP, port);
        } else if (!strcmp(command, "bid") || command == 'b'){
            bid(IP, port);
        } else if (!strcmp(command, "show_record") || !strcmp(command, "sr")){
            show_record(IP, port);
        } else {
            print("invalid command!");
        }*/
    }
    
    return 0;
}
