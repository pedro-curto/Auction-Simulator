// nc -u <address> <port> ; -u (UDP)
#include "user.h"

char* connect_UDP(char* IP, char* port, char* request, char* buffer) {
    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    int allgood = 1; // TODO não gosto disto, podemos fazer como está no connect_TCP 
    // fica mais perceptível do que teres 1 erro que pode acontecer em qualquer sítio e o debug é no caralho

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) allgood = 0;
    
    memset(&hints, 0, sizeof hints);
    memset(&addr, 0, sizeof addr);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

    errcode = getaddrinfo(IP, port, &hints, &res);
    if (errcode != 0) allgood = 0; 
    n = sendto(fd, request, strlen(request), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) allgood = 0;

    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*) &addr, &addrlen);
    if (n == -1) allgood = 0;

    freeaddrinfo(res);
    close(fd);

    if(!allgood){
        return "error";
    } 
    return buffer;
}

char* connect_TCP(char* IP, char* port, char* request, char* buffer) {
    int fd, errcode;
    ssize_t n;
    //socklen_t addrlen;
    struct addrinfo hints, *res;
    //struct sockaddr_in addr;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) exit(1);
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    errcode = getaddrinfo(IP, port, &hints, &res);
    if (errcode != 0) perror("Error getting address info.");

    n = connect(fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) perror("Error connecting.");

    n = write(fd, request, strlen(request));
    if (n == -1) perror("Error writing.");

    n = read(fd, buffer, 128);
    if (n == -1) perror("Error reading.");

    freeaddrinfo(res);
    close(fd);

    return buffer;
}

void login(char* IP, char* port, char* uid, char* password) { // uses UDP protocol

    char buffer[128], login_request[100];
    printf("ur inside login func now \n");

    snprintf(login_request, sizeof(login_request), "LIN %s %s\n", uid, password);
    
    if (!strncmp(connect_UDP(IP, port, login_request, buffer), "error", 5)) return;

    if (!strncmp(buffer, "RLI OK", 6)){
        printf("User is now logged in.\n");
    } else if (!strncmp(buffer, "RLI REG", 7)) {
        printf("New user successfully created and logged in.\n");
    } else if (!strncmp(buffer, "RLI NOK", 7)) {
        printf("Password doesn't match.\n");
    } else { // não sei se este caso é necessário mas é mais fácil dar debug se der merda ig
        printf("Error logging in.\n");
    }
    
    printf("ur leaving login func now \n");
}

void logout(char* IP, char* port, char* uid, char* password) {
    printf("ur inside logout func now \n");
    char buffer[128], logout_request[100];
    snprintf(logout_request, sizeof(logout_request), "LOU %s %s\n", uid, password);
    if (!strncmp(connect_UDP(IP, port, logout_request, buffer), "error", 5)) {
        printf("Error connecting to log out.\n");
        return;
    }

    if(!strncmp(buffer, "RLO OK", 6)){
        printf("User is logged out.\n");
    } else if (!strncmp(buffer, "RLO NOK", 7)){
        printf("User was not logged in.\n");
    } else if(!strncmp(buffer, "RLO UNR", 7)) {
        printf("User was not registered.\n");
    }
    
    printf("ur leaving logout func now \n");
}

void unregister(char* IP, char* port, char* uid, char* password) {
    printf("! You're inside unregister function !\n");
    char buffer[128], unregister_request[100];
    snprintf(unregister_request, sizeof(unregister_request), "UNR %s %s\n", uid, password); 
    if (!strncmp(connect_UDP(IP, port, unregister_request, buffer), "error", 5)) {
        printf("Error connecting to unregister.\n");
        return;
    }





    printf("! You're leaving unregister function !\n");    
}



/*
void handle_exit(char* IP, char* port) {

}

void handle_open(char* IP, char* port) { 


}
    
void handle_close(char* IP, char* port) {

}

void my_auctions(char* IP, char* port) {

}

void my_bids(char* IP, char* port) {

}

void list(char* IP, char* port) {

}

void show_asset(char* IP, char* port) {

}

void bid(char* IP, char* port) {

}

void show_record(char* IP, char* port) {
    
}*/


int main(int argc, char *argv[]) {
    char IP[50] = SERVER_IP; 
    char* port = PORT;
    printf("argc: %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
    switch(argc) {  
        case 1: 
            printf("case 1\n");
            break;
        case 3: 
            printf("case 3\n");
            if(!strcmp(argv[1], "-n")) {
                strcpy(IP, argv[2]);
            } else if(!strcmp(argv[1], "-p")) {
                strcpy(port, argv[2]);
            } else {
                printf("Invalid arguments.\n");
                return -1;  
            }
            break;
        case 5:
            printf("case 5\n");
            if(!strcmp(argv[1],"-n") && !strcmp(argv[3],"-p")) {
                strcpy(IP, argv[2]);
                strcpy(port, argv[4]);
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
        char uid[UID_SIZE + 1], password[PASSWORD_SIZE + 1];
        // gets stdin into input buffer
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Error reading input");
            return -1;
        }
        if (input[strlen(input)-1] == '\n') input[strlen(input)-1] = '\0';
        sscanf(input, "%11s", command);
        // bid AID value (or b AID value); close AID; login UID password; logout; list (or l); exit; open name asset_fname start_value timeactive; myauctions; ma; mybids (or mb) ; show_asset AID (or sa AID);  show_record AID (or sr AID); unregister;


        if(!strcmp(command, "login")) {
            int valid = 1;
            sscanf(input, "login %s %s", uid, password); // * reads from stream and discards

            if (strlen(uid) != UID_SIZE || strlen(password) != PASSWORD_SIZE) valid = 0;
            for (int i = 0; i < UID_SIZE; i++) {
                if (!isdigit(uid[i])) valid = 0;
            }
            for (int i = 0; i < PASSWORD_SIZE; i++) {
                if (!isalnum(password[i])) valid = 0;
            }

            printf("command: %s\nuid: %s\npassword: %s\n",command, uid, password);

            if (valid) login(IP, port, uid, password);
            else printf("Invalid login attempt.\n");

        } else if (!strcmp(command, "logout")) {
            logout(IP, port, uid, password);
        } else if (!strcmp(command, "unregister")) {
            unregister(IP, port, uid, password);
        } /*else if (!strcmp(command, "exit")) {
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
            printf("invalid command!\n");
        }*/
    }
    
    return 0;
}
