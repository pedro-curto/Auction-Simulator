// nc -u <address> <port> ; -u (UDP)
#include "user.h"

int main(int argc, char *argv[]) {
    char IP[50] = SERVER_IP; 
    char port[10] = PORT;
    char input[100], command[12];
    char uid[UID_SIZE+1], password[PASSWORD_SIZE+1];
    int user_loggedin = 0;
    
    switch(argc) {
        case 1:
            break;
        case 3: 
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
        // gets stdin into input buffer
        //printf("user logged in: %d\n", user_loggedin);
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Error reading input");
            return -1;
        }
        if (input[strlen(input)-1] == '\n') input[strlen(input)-1] = '\0';
        sscanf(input, "%11s", command);
        if (!strcmp(command, "login")) {
            int correct_login;
            if (user_loggedin) {
                printf("You are already logged in.\n");
                continue;
            }
            int valid = 1;
            sscanf(input, "login %s %s", uid, password);

            if (strlen(uid) != UID_SIZE || strlen(password) != PASSWORD_SIZE) valid = 0;
            for (int i = 0; i < UID_SIZE; i++) {
                if (!isdigit(uid[i])) valid = 0;
            }
            for (int i = 0; i < PASSWORD_SIZE; i++) {
                if (!isalnum(password[i])) valid = 0;
            }
            //printf("command: %s\nuid: %s\npassword: %s\n",command, uid, password);
            if (valid) {
                correct_login = login(IP, port, uid, password); 
                if (correct_login) user_loggedin = 1;
            }
            else printf("Invalid login attempt.\n");

        } else if (!strcmp(command, "logout")) {
            if (!user_loggedin) {
                printf("You are not logged in.\n");
                continue;
            }
            if (logout(IP, port, uid, password)) {
                user_loggedin = 0;
                // logged out: clears uid and password of current user
                strcpy(uid, "");
                strcpy(password, "");
            }
        } else if (!strcmp(command, "unregister")) {
            if (!user_loggedin) {
                printf("You are not logged in.\n");
                continue;
            }
            if (unregister(IP, port, uid, password)) { 
                user_loggedin = 0;
                // unregistered: clears uid and password of user
                strcpy(uid, "");
                strcpy(password, "");
            } else printf("Error unregistering.\n");
            
        } else if (!strcmp(command, "exit")) {
            if(user_loggedin) {
                printf("You are still logged in.\n");
                continue;
            }
            break;
        } else if (!strcmp(command, "open")) {
            char name[NAME_SIZE + 1], asset_fname[ASSET_FNAME_SIZE + 1];
            int start_value, timeactive;
            sscanf(input, "open %s %s %d %d", name, asset_fname, &start_value, &timeactive);
            printf("command: %s\nname: %s\nasset_fname: %s\nstart_value: %d\ntimeactive: %d\n", command, name, asset_fname, start_value, timeactive);

            if (!user_loggedin) {
                printf("You are not logged in.\n");
                continue;
            }
            if (strlen(name) > NAME_SIZE) {
                printf("Invalid open attempt: name or asset filename too long.\n");
                continue;
            }
            if (!valid_filename(asset_fname)) {
                printf("Invalid open attempt: asset filename is invalid.\n");
                continue;
            }
            if (start_value < 0 || timeactive < 0) {
                printf("Invalid open attempt: start value or time active cannot be negative.\n");
                continue;
            }
            //handle_open(IP, port, uid, password, name, asset_fname, start_value, timeactive);

        } else if (!strcmp(command, "list") || command[0] == 'l') { 
            listAllAuctions(IP, port);
        }
        
        /* else if (!strcmp(command, "close")){
            handle_close(IP, port);
        } else if (!strcmp(command, "myauctions") || !strcmp(command, "ma")){
            my_auctions(IP, port);
        } else if (!strcmp(command, "mybids") || !strcmp(command, "mb")) {
            my_bids(IP, port);
        } else if (!strcmp(command, "show_asset") || !strcmp(command, "sa")){
            show_asset(IP, port);
        } else if (!strcmp(command, "bid") || command == 'b'){
            bid(IP, port);
        } else if (!strcmp(command, "show_record") || !strcmp(command, "sr")){
            show_record(IP, port);
        } */else {
            printf("invalid command!\n");
        }
    }
    
    return 0;
}

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

    n = read(fd, buffer, sizeof(buffer));
    if (n == -1) perror("Error reading.");

    freeaddrinfo(res);
    close(fd);

    return buffer;
}

// attempts to login the user; returns 1 if successful, 0 otherwise
int login(char* IP, char* port, char* uid, char* password) { // uses UDP protocol
    char buffer[128], login_request[100];
    //printf("ur inside login func now \n");
    snprintf(login_request, sizeof(login_request), "LIN %s %s\n", uid, password);
    
    if (!strncmp(connect_UDP(IP, port, login_request, buffer), "error", 5)) return 0;

    if (!strncmp(buffer, "RLI OK", 6)) {
        printf("User is now logged in.\n");
    } else if (!strncmp(buffer, "RLI REG", 7)) {
        printf("New user successfully created and logged in.\n");
    } else if (!strncmp(buffer, "RLI NOK", 7)) {
        printf("Password doesn't match.\n");
        return 0;
    } else { 
        printf("Error logging in.\n");
        return 0;
    }
    //printf("ur leaving login func now \n");
    return 1;
}

int logout(char* IP, char* port, char* uid, char* password) {
    //printf("ur inside logout func now \n");
    char buffer[128], logout_request[100];
    // composes request and sends to server through UDP
    snprintf(logout_request, sizeof(logout_request), "LOU %s %s\n", uid, password);
    if (!strncmp(connect_UDP(IP, port, logout_request, buffer), "error", 5)) {
        printf("Error connecting to log out.\n");
        return 0;
    }
    // checks server response
    if (!strncmp(buffer, "RLO OK", 6)) {
        printf("User was correctly logged out.\n");
        return 1;
    } else if (!strncmp(buffer, "RLO NOK", 7)){
        printf("Error logging out: user was not logged in.\n");
    } else if(!strncmp(buffer, "RLO UNR", 7)) {
        printf("User was not registered.\n");
    } else printf("Error logging out.\n");
    //printf("ur leaving logout func now \n");
    return 0;
}

int unregister(char* IP, char* port, char* uid, char* password) {
    printf("! You're inside unregister function !\n");
    char buffer[128], unregister_request[100];
    snprintf(unregister_request, sizeof(unregister_request), "UNR %s %s\n", uid, password);
    // establishes UDP connection with server and sends request
    if (!strncmp(connect_UDP(IP, port, unregister_request, buffer), "error", 5)) {
        printf("Error connecting to unregister.\n");
        return 0;
    }
    // checks server response
    if (!strncmp(buffer, "RUR OK", 6)) {
        printf("User was successfully unregistered.\n");
        return 1;
    } else if (!strncmp(buffer, "RUR NOK", 7)) {
        printf("Incorrect register attempt: the user was not logged in.\n");
    } else if (!strncmp(buffer, "RUR UNR", 7)) {
        printf("User was not registered.\n");
    } else printf("Error unregistering.\n");
    printf("! You're leaving unregister function !\n");
    return 0;
}

void listAllAuctions(char* IP, char* port) { // uses UDP protocol
    char buffer[1024], *list_request = "LST\n";
    // establishes UDP connection with server and sends request
    if (!strncmp(connect_UDP(IP, port, list_request, buffer), "error", 5)) {
        printf("Error while connecting to list all auctions.\n");
        return;
    }
    // checks server response
    if (!strncmp(buffer, "RLS OK", 6)) {
        printf("Auctions currently open:\n");
        char *token = strtok(buffer, " ");
        token = strtok(NULL, " ");
        while (token != NULL) {
            printf("%s\n", token);
            token = strtok(NULL, " ");
        }
    }
    else if (!strncmp(buffer, "RLS NOK", 7)) {
        printf("No auctions are currently open.\n");
    } else {
        printf("Unexpected server response: %s\n", buffer);
    }
}

void handle_open(char* IP, char* port, char* uid, char* password, char* name, char* asset_fname, int start_value, int timeactive) {
    printf("! You're inside unregister function !\n");
    char buffer[1024], open_request[1000];
    int Fsize = 0;
    char Fdata[100];
    int AID;

    snprintf(open_request, sizeof(open_request), "OPA %s %s %s %d %d %s %d %s\n", uid, password, name, start_value, timeactive, asset_fname, Fsize, Fdata);

    if (!strncmp(connect_TCP(IP, port, open_request, buffer), "error", 5)) {
        printf("Error connecting to unregister.\n");
        return;
    }

    if (!strncmp(buffer, "ROA NOK", 7)) {
        printf("Auction could not be started.\n");
    } else if (!strncmp(buffer, "ROA NLG", 7)) {
        printf("User was not logged in.\n");
    } else if (!strncmp(buffer, "ROA OK", 5)) {
        sscanf(buffer, "ROA OK %d", &AID); 

        printf("Auction identifier: %d.\n", AID);
    } else printf("Error opening auction.\n");

}
/*
void handle_close(char* IP, char* port) {

}

void my_auctions(char* IP, char* port) {

}

void my_bids(char* IP, char* port) {

}


void show_asset(char* IP, char* port) {

}

void bid(char* IP, char* port) {

}

void show_record(char* IP, char* port) {
    
}*/

int valid_filename(char *filename) {

    if (strlen(filename) > ASSET_FNAME_SIZE) return -1;
    if (strspn(filename, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.")) return -1;

    char *extension = strrchr(filename, '.');
    if (!extension || strlen(extension) != 4) return -1;
    if (!isalpha(extension[1]) || !isalpha(extension[2]) || !isalpha(extension[3])) return -1;

    return 1;
}
