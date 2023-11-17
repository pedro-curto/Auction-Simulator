// nc -u <address> <port> ; -u (UDP)
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#define GN 53 // não sei se é este, já se altera
#define PORT (58000 + GN) // isto tá mal

//FAZER FUNCOES PARA CADA COMANDO

void login(char* IP, char* port) {
    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];
    
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) exit(1);
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt", PORT, &hints, &res);
    if (errcode != 0) exit(1);

    n = sendto(fd, "Hello!\n", 7, 0 ,res->ai_addr, res->ai_addrlen);
    if (n == -1) exit(1);

    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, 128, 0,
                 (struct sockaddr*) &addr, &addrlen);
    if (n == -1) exit(1);

    write(1, "echo: ", 6);
    write(1, buffer, n);
    freeaddrinfo(res);
    close(fd);
    
}

void logout(char* IP, char* port){

}

void unregister(char* IP, char* port){

}

void exit(char* IP, char* port){

}

void handle_open(char* IP, char* port) { 


}
    
void handle_close(char* IP, char* port) {

}

void my_auctions(char* IP, char* port) {

}

void my_bids(char* IP, char* port) {

}

void list(char* IP, char* port){

}

void show_asset(char* IP, char* port){

}

void bid(char* IP, char* port){

}

void show_record(char* IP, char* port){
    
}


int main(int argc, char *argv[]) {
    char *IP;
    char *port;
    
    switch(argc) {  // TODO
        case 0:
            //char *IP = machineIP;
            //char *port = 58000+GN;
            break;
        case 2:
            if(!strcmp(argv[2],"-n")) {
                IP = argv[3];
                //char *port = 58000+GN;
            }
            if(!strcmp(argv[2],"-p")) {
                //char *IP = machineIP;
                port = argv[5];
            }
            break;

        case 4:
            if(!strcmp(argv[2],"-n") && !strcmp(argv[4],"-p")) {
                IP = argv[3];
                port = argv[5];
            }
            break;

        default: //error
    }

    char input[100];
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
            char username[100], password[100];
            int valid = 1;
            sscanf("%*s %s %s", username, password); // * reads from stream and discards
            // checks if username is strictly 6 digits from 0 to 9
            if (strlen(username) != 6) valid = 0;
            for (int i = 0; i <= 5; i++) {
                if(username[i] < '0' || username[i] > '9') valid = 0;
            }
            if (valid) login(IP, port);
            else printf("Incorrect login attempt.\n");

        } else if(!strcmp(command, "logout")) {
            logout(IP, port);
        } else if(!strcmp(command, "unregister")){
            unregister(IP, port);
        } else if(!strcmp(command, "exit")){
            exit(IP, port);
        } else if(!strcmp(command, "open")) {
            char *name = scanf(input);
            char *asset_nam

            handle_open(IP, port);
        } else if(!strcmp(command, "close")){
            handle_close(IP, port);
        } else if(!strcmp(command, "myauctions") || !strcmp(command, "ma")){
            my_auctions(IP, port);
        } else if(!strcmp(command, "mybids") || !strcmp(command, "mb")){
            my_bids(IP, port);
        } else if(!strcmp(command, "list") || command == 'l') { 
            list(IP, port);
        } else if(!strcmp(command, "show_asset") || !strcmp(command, "sa")){
            show_asset(IP, port);
        } else if(!strcmp(command, "bid") || command == 'b'){
            bid(IP, port);
        } else if(!strcmp(command, "show_record") || !strcmp(command, "sr")){
            show_record(IP, port);
        } else{
            print("invalid command!");
        }
    }
    
    return 0;
}


