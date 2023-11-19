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
        printf("user logged in: %d\n", user_loggedin);
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Error reading input");
            return -1;
        }
        if (input[strlen(input)-1] == '\n') input[strlen(input)-1] = '\0';
        sscanf(input, "%11s", command);
        if (!strcmp(command, "login")) {
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
                if (login(IP, port, uid, password)) user_loggedin = 1;
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
            if (user_loggedin) {
                printf("You are still logged in.\n");
                continue;
            }
            break;
        } else if (!strcmp(command, "open")) {
            char name[NAME_SIZE + 1], asset_fname[ASSET_FNAME_SIZE + 1];
            int start_value, timeactive;
            // printf("IP in main open: %s\nport in main open: %s\n", IP, port);
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
                printf("Invalid open attempt: minimum selling value for the asset and auction duration cannot be negative.\n");
                continue;
            }
            printf("antes openauction\n");
            openAuction(IP, port, uid, password, name, asset_fname, start_value, timeactive);
            printf("depois openauction\n");

        } else if (!strcmp(command, "list") || command[0] == 'l') { 
            listAllAuctions(IP, port);
        } else if (!strcmp(command, "close")) {
            int aid;
            // receives close AID, where AID must be a 3-digit number between 0 and 999
            sscanf(input, "close %d", &aid);
            /*if (!user_loggedin) {
                printf("You need to login first.\n");
                continue;
            }*/
            printf("AID: %d\n", aid);
            if (aid < 0 || aid > 999) {
                printf("Invalid close attempt: AID must be a 3-digit number between 0 and 999.\n");
                continue;
            }
            closeAuction(IP, port, uid, password, aid);
        
        /*myauctions or ma – the User application sends a message to the AS, using
        the UDP protocol, asking for a list of the auctions started by the logged in user,
        or auctions in which the logged in user has placed a bid.
        The AS will reply with the requested list, or an information that the user is not
        involved in any of the currently active auctions. This information should be
        displayed to the User.*/
        } else if (!strcmp(command, "myauctions") || !strcmp(command, "ma")) {
            myAuctions(IP, port, uid);

        /*
        mybids or mb – the User application sends a message to the AS, using the
        UDP protocol, asking for a list of the auctions for which the logged in user has
        placed a bid.
        The AS will reply with the requested list, or an information that the user has no
        active auction bids. This information should be displayed to the User.
        

        } /else if (!strcmp(command, "mybids") || !strcmp(command, "mb")) {
            my_bids(IP, port); */

        /*
        } else if (!strcmp(command, "show_asset") || !strcmp(command, "sa")) {
            show_asset(IP, port);
        } else if (!strcmp(command, "bid") || command == 'b'){
            bid(IP, port);
        } else if (!strcmp(command, "show_record") || !strcmp(command, "sr")) {
            show_record(IP, port);
        */} else {
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
    // TODO devemos permitir que a função continue se qualquer um dos perrors ocorrer?
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) perror("Error creating socket.");
    
    memset(&hints, 0, sizeof hints);
    memset(&addr, 0, sizeof addr);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

    errcode = getaddrinfo(IP, port, &hints, &res);
    if (errcode != 0) perror("Error getting address info."); 
    n = sendto(fd, request, strlen(request), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) perror("Error sending request.");

    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*) &addr, &addrlen);
    if (n == -1) perror("Error receiving response.");

    freeaddrinfo(res);
    close(fd);

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
    printf("IP in TCP: %s\nport in TCP: %s", IP, port);
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
    printf("Server response for logout: %s\n", buffer);
    if (!strncmp(buffer, "RLO OK", 6)) {
        printf("User was correctly logged out.\n");
        return 1;
    } else if (!strncmp(buffer, "RLO NOK", 7)){
        printf("Error logging out: user was not logged in.\n");
    } else if(!strncmp(buffer, "RLO UNR", 7)) {
        printf("User was not registered.\n");
    } else printf("Error \n");
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

/* Open a new auction. The User application sends a message to the AS asking to open
a new auction, providing a short description name (represented with up to 10
alphanumerical characters), an image (or other document file) of the asset to sell, the
start value (represented with up to 6 digits), and the duration of the auction in
seconds (represented with up to 5 digits). In reply, the AS informs if the request was
successful, and the assigned auction identifier, AID, a 3-digit number.*/

void openAuction(char* IP, char* port, char* uid, char* password, char* name, char* asset_fname, int start_value, int timeactive) {
    char buffer[1024];//, open_request[10000000]; 
    char fsizeStr[9]; //, Fdata[10000000];
    int AID;
    off_t fsize = get_file_size(asset_fname);
    printf("%ld\n", fsize);
    if (fsize == -1) {
        printf("Error opening file.\n");
        return;
    }
    char Fdata[fsize];
    if (fsize > MAX_FILESIZE) { // FIXME quero eventualmente que isto saia mas enquanto não sei o upper bound é lidar
        printf("File size exceeds allowed limit.\n");
        return;
    }
    if (read_file(asset_fname, Fdata, fsize) == -1) {
        printf("Error reading file.\n");
        return;
    }
    snprintf(fsizeStr, sizeof(fsizeStr), "%08jd", fsize);
    size_t open_request_size = 100 + strlen(uid) + strlen(password) + strlen(name) + strlen(asset_fname) + strlen(fsizeStr) + fsize;
    char *open_request = (char*) malloc(open_request_size);
    snprintf(open_request, sizeof(open_request), "OPA %s %s %s %d %d %s %s %s\n", 
             uid, password, name, start_value, timeactive, asset_fname, fsizeStr, Fdata);

    if (!strncmp(connect_TCP(IP, port, open_request, buffer), "error", 5)) {
        printf("Error connecting to unregister.\n");
        free(open_request);
        return;
    }
    // handles server response
    if (!strncmp(buffer, "ROA NOK", 7)) {
        printf("Auction could not be started.\n");
    } else if (!strncmp(buffer, "ROA NLG", 7)) {
        printf("User was not logged in.\n");
    } else if (!strncmp(buffer, "ROA OK", 6)) {
        sscanf(buffer, "ROA OK %d", &AID); 
        printf("Auction identifier: %d.\n", AID);
    } else printf("Error opening auction.\n");

    free(open_request);
}

/* close AID – the User application sends a message to the AS, using the TCP
protocol, asking to close an ongoing auction, with identifier AID, that had been
started by the logged in user.
The AS will reply informing whether the auction was successfully closed,
cancelling the sale, or if the auction time had already ended. This information
should be displayed to the User. After receiving the reply from the AS, the User
closes the TCP connection.
*/
void closeAuction(char* IP, char* port, char* uid, char* password, int aid) {
    char buffer[1024], close_request[25]; // close request is CLS user-- password AID\n
    snprintf(close_request, sizeof(close_request), "CLS %6s %8s %3d\n", uid, password, aid);
    if (!strncmp(connect_TCP(IP, port, close_request, buffer), "error", 5)) {
        printf("Error while connecting to close an auction.\n");
        return;
    }
    // handles server response
    if (!strncmp(buffer, "RCL OK", 6)) {
        printf("Auction successfully closed.\n");
    } else if (!strncmp(buffer, "RCL NLG", 7)) {
        printf("User was not logged in.\n");
    } else if (!strncmp(buffer, "RCL OAU", 6)) {
        printf("No such auction.\n");
    } else if (!strncmp(buffer, "RCL EOW", 7)) {
        printf("Auction is not owned by this user.\n");
    } else if (!strncmp(buffer, "RCL END", 7)) {
        printf("Your auction has already ended.\n");
    } else printf("Error opening auction.\n"); // in this case we got ERR
}


/* LMA UID
Following the myauctions command the User sends the AS a request to list
the auctions started by user UID. 
RMA status[ AID state]*
In reply to a LMA request the AS reply status is NOK if user UID has no
ongoing auctions. If the user is not logged in the reply status is NLG. If there
are ongoing auctions for user UID the reply status is OK and a list of the
identifiers AID and state for all ongoing auctions started by this user, separated by single spaces, 
is sent by the AS. state takes value 1 if the auction is active, or 0 otherwise.
*/
void myAuctions(char* IP, char* port, char* uid) {
    char buffer[1024], myauctions_request[12]; // LMA UID---\n
    snprintf(myauctions_request, sizeof(myauctions_request), "LMA %6s\n", uid);
    if (!strncmp(connect_UDP(IP, port, myauctions_request, buffer), "error", 5)) {
        printf("Error while connecting to list user's auctions.\n");
        return;
    }
    // handles server response
    if (!strncmp(buffer, "RMA NOK", 7)) {
        printf("User has no ongoing auctions.\n");
    } else if (!strncmp(buffer, "RMA NLG", 7)) {
        printf("User is not logged in.\n");
    } else if (!strncmp(buffer, "RMA OK", 6)) { 
    // FIXME isto precisa de ser repensado quando os opens começarem a funcionar e pudermos testar isto a sério
        printf("Auctions currently open:\n");
        char *token = strtok(buffer, " ");
        token = strtok(NULL, " ");
        while (token != NULL) {
            printf("%s", token);
            token = strtok(NULL, " ");
        }
    } else printf("Server responded with an error when trying to list auctions.\n"); // in this case we got ERR

}

/*
void myBids(char* IP, char* port) {

}


void showAsset(char* IP, char* port) {

}

void bid(char* IP, char* port) {

}

void showRecord(char* IP, char* port) {
    
}*/

// ---------------------------- Auxiliary functions ----------------------------

int valid_filename(char *filename) {

    if (strlen(filename) > ASSET_FNAME_SIZE) return -1;
    if (strspn(filename, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.")) return -1;

    char *extension = strrchr(filename, '.');
    if (!extension || strlen(extension) != 4) return -1;
    if (!isalpha(extension[1]) || !isalpha(extension[2]) || !isalpha(extension[3])) return -1;

    return 1;
}

off_t get_file_size(char *filename) {
    struct stat st;
    // stat returns 0 on success
    if (!stat(filename, &st)) {
        return st.st_size;
    }
    return -1;
}


int read_file(char *filename, char *buffer, off_t size) {
    //size_t bufferSize = 100;
    int fd = open(filename, O_RDONLY);
    if (fd == -1) { 
        perror("Error opening file");
        return -1;
    }
    ssize_t n = read(fd, buffer, size);
    close(fd);
    if (n == -1) {
        perror("Error reading file");
        return -1;
    }
    return 0;
}