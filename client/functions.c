#include "client.h"


// attempts to login the user; returns 1 if successful, 0 otherwise
int login(char* IP, char* port, char* uid, char* password, char* input) { // uses UDP protocol
    char buffer[128], login_request[100];
    int valid = 1;

    sscanf(input, "%s %s", uid, password);

    if (strlen(uid) != UID_SIZE || strlen(password) != PASSWORD_SIZE) valid = 0;
    for (int i = 0; i < UID_SIZE; i++) {
        if (!isdigit(uid[i])) valid = 0;
    }
    for (int i = 0; i < PASSWORD_SIZE; i++) {
        if (!isalnum(password[i])) valid = 0;
    }

    if (!valid) {
        printf("Invalid login attempt.\n");
        return 0;
    }

    snprintf(login_request, sizeof(login_request), "LIN %s %s\n", uid, password);

    connect_UDP(IP, port, login_request, buffer);

    if (!verify_buffer(buffer, strlen(buffer))) {
        printf("Error reading server response.\n");
        return 0;
    }

    if (!strncmp(buffer, "RLI OK", 6)) {
        printf("User is now logged in.\n");
        return 1;
    } else if (!strncmp(buffer, "RLI REG", 7)) {
        printf("New user successfully created and logged in.\n");
        return 1;
    } else if (!strncmp(buffer, "RLI NOK", 7)) {
        printf("Password doesn't match.\n");
    } else { 
        printf("Error logging in.\n");
    }
    return 0;
}


int logout(char* IP, char* port, char* uid, char* password) {
    char buffer[128], logout_request[100];
    // composes request and sends to server through UDP
    snprintf(logout_request, sizeof(logout_request), "LOU %s %s\n", uid, password);
    connect_UDP(IP, port, logout_request, buffer);

    if (!verify_buffer(buffer, strlen(buffer))) {
        printf("Error reading server response.\n");
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
    } else printf("Error logging out\n");
    return 0;
}

int unregister(char* IP, char* port, char* uid, char* password) {
    char buffer[128], unregister_request[100];
    snprintf(unregister_request, sizeof(unregister_request), "UNR %s %s\n", uid, password);
    connect_UDP(IP, port, unregister_request, buffer);

    if (!verify_buffer(buffer, strlen(buffer))) {
        printf("Error reading server response.\n");
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
    return 0;
}


void listAllAuctions(char* IP, char* port) { // uses UDP protocol
    char buffer[6100], *list_request = "LST\n";
    memset(buffer, 0, sizeof(buffer));
    // establishes UDP connection with server and sends request
    connect_UDP(IP, port, list_request, buffer);

    if (!verify_buffer(buffer, strlen(buffer))) {
        printf("Error reading server response.\n");
        return;
    }
    
    // checks server response
    if (!strncmp(buffer, "RLS OK", 6)) {
        printf("Auctions currently open:\n");
        char token[4];
        for (int i = 7; i > 0; i++) { // 7 = strlen("RMB OK ")
            i = read_buffer_token(buffer, token, sizeof(token), i);
            printf("%s ", token);
            i = read_buffer_token(buffer, token, sizeof(token), ++i);
            printf("%s\n", token);
        }
    }
    else if (!strncmp(buffer, "RLS NOK", 7)) {
        printf("No auctions are currently open.\n");
    } else {
        printf("Unexpected server response: %s\n", buffer); // case RLS ERR
    }
}

void openAuction(char* IP, char* port, char* uid, char* password, char* input) {
    char name[NAME_SIZE + 1], asset_fname[ASSET_FNAME_SIZE + 1];
    char buffer[1024], request_header[100], path[50];
    int start_value, timeactive, aid;
    sscanf(input, "%s %s %d %d", name, asset_fname, &start_value, &timeactive);
    sprintf(path, "client/local_assets/%s", asset_fname);
    int fsize = getFileSize(path);
    // necessary checks
    if (fsize == -1) {
        printf("Error opening file; couldn't get file size.\n");
        return;
    }
    if (fsize > MAX_FILESIZE) {
        printf("File size exceeds allowed limit.\n");
        return;
    }
    if (strlen(name) > NAME_SIZE) {
        printf("Invalid open attempt: auction description name too long.\n");
        return;
    }
    if (!valid_filename(asset_fname)) {
        printf("Invalid open attempt: asset filename is invalid.\n");
        return;
    }
    if (start_value < 0 || timeactive < 0) {
        printf("Invalid open attempt: minimum selling value for the asset and auction duration cannot be negative.\n");
        return;
    }
    if (timeactive > MAX_AUCTIME) {
        printf("Invalid open attempt: auction duration is too large.\n");
        return;
    }
    snprintf(request_header, sizeof(request_header), "OPA %s %s %s %d %d %s %d ",
            uid, password, name, start_value, timeactive, asset_fname, fsize);
    // establishes TCP connection
    connect_TCP(IP, port, request_header, buffer, sizeof(buffer));
    
    if (!verify_buffer(buffer, strlen(buffer))) {
        printf("Error reading server response.\n");
        return;
    }

    // handles server response
    if (!strncmp(buffer, "ROA NOK", 7)) {
        printf("Auction could not be started.\n");
    } else if (!strncmp(buffer, "ROA NLG", 7)) {
        printf("User was not logged in.\n");
    } else if (!strncmp(buffer, "ROA OK", 6)) {
        sscanf(buffer, "ROA OK %d", &aid);
        printf("Auction succesfully open! Your identifier is %d.\n", aid);
    } else printf("Error opening auction.\n"); // case ROA ERR
}


void closeAuction(char* IP, char* port, char* uid, char* password, char* input) {
    char buffer[1024], close_request[25]; // close request is CLS user-- password AID\n
    int aid;
    // receives close AID, where AID must be a 3-digit number between 0 and 999
    sscanf(input, "%d", &aid);
    if (aid < 0 || aid > 999) {
        printf("Invalid close attempt: AID must be a 3-digit number between 0 and 999.\n");
        return;
    }
    snprintf(close_request, sizeof(close_request), "CLS %6s %8s %03d\n", uid, password, aid);
    connect_TCP(IP, port, close_request, buffer, sizeof(buffer));

    if (!verify_buffer(buffer, strlen(buffer))) {
        printf("Error reading server response.\n");
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
    } else printf("Server responded with ERR while trying to close the auction.\n"); // in this case we got ERR
}

void myAuctions(char* IP, char* port, char* uid) {
    char buffer[MA_BUFFER_SIZE], myauctions_request[12]; // LMA UID---\n
    snprintf(myauctions_request, sizeof(myauctions_request), "LMA %6s\n", uid);
    connect_UDP(IP, port, myauctions_request, buffer);

    if (!verify_buffer(buffer, strlen(buffer))) {
        printf("Error reading server response.\n");
        return;
    }

    // handles server response
    if (!strncmp(buffer, "RMA NOK", 7)) {
        printf("User has no ongoing auctions.\n");
    } else if (!strncmp(buffer, "RMA NLG", 7)) {
        printf("User is not logged in.\n");
    } else if (!strncmp(buffer, "RMA OK", 6)) { 
        printf("User %6s auctions:\n", uid);
        char token[4];
        for (int i = 7; i > 0; i++) { // 7 = strlen("RMB OK ")
            i = read_buffer_token(buffer, token, sizeof(token), i);
            printf("%s ", token);
            i = read_buffer_token(buffer, token, sizeof(token), ++i);
            printf("%s\n", token);
        }
    } else printf("Server responded with an error when trying to list auctions.\n"); // in this case we got ERR

}

void myBids(char* IP, char* port, char* uid) { // uses UDP protocol

    char buffer[1024], mybids_request[12]; // LMB UID---\n
    snprintf(mybids_request, sizeof(mybids_request), "LMB %6s\n", uid);
    connect_UDP(IP, port, mybids_request, buffer);

    if (!verify_buffer(buffer, strlen(buffer))) {
        printf("Error reading server response.\n");
        return;
    }

    // handles server response
    if (!strncmp(buffer, "RMB NOK", 7)) {
        printf("You have no ongoing bids!\n");
    } else if (!strncmp(buffer, "RMB NLG", 7)) {
        printf("Bid listing error: user is not logged in.\n");
    } else if (!strncmp(buffer, "RMB OK", 6)) {
        printf("Your current bids:\n");
        char token[4];
        for (int i = 7; i > 0; i++) { // 7 = strlen("RMB OK ")
            i = read_buffer_token(buffer, token, sizeof(token), i);
            printf("%s ", token);
            i = read_buffer_token(buffer, token, sizeof(token), ++i);
            printf("%s\n", token);
        }
    } else printf("Server responded with an error when trying to list bids.\n");
}


void showAsset(char* IP, char* port, int aid) {
    char showasset_request[SA_BUFFER_SIZE];// SAS AID\n
    char buffer[1000];
    memset(buffer, 0, sizeof(buffer));
    
    snprintf(showasset_request, sizeof(showasset_request), "SAS %03d\n", aid);

    int tcp_socket = connect_tcp(IP, port);
    // sends request to server
    write_tcp(tcp_socket, showasset_request);

    // receives server response
    char status1[5], status2[5], fname[ASSET_FNAME_SIZE + 1], fsize[9];
    char path[50] = "client/assets/";
    
    if (!read_field(tcp_socket, status1, 3)) {
        printf("Error reading server response.\n");
        return;
    }
    if (!read_field(tcp_socket, status2, 3)) {
        printf("Error reading server response.\n");
        return;
    }
    if (!strncmp(status2, "NOK", 3)) {
        printf("Error showing asset: no such auction.\n");
        return;
    } else if (!strncmp(status2, "OK", 2)) {
        if (!read_field(tcp_socket, fname, sizeof(fname))) {
            printf("Error reading server response.\n");
            return;
        }
        if (!read_field(tcp_socket, fsize, sizeof(fsize))) {
            printf("Error reading server response.\n");
            return;
        }
        strcat(path, fname);
        read_file(tcp_socket, atoi(fsize), path);
        printf("Asset %s with size %s succesfully stored in the assets folder!\n", fname, fsize);

    } else if (!strncmp(status2, "ERR", 3)) {
        printf("Server responded with an error when trying to show asset.\n");
        return;
    } else printf("Badly formatted server response: %s\n", status1);

}


void bid(char* IP, char* port, char *uid, char* password, int aid, int value) { // uses TCP protocol
    char buffer[30000], bid_request[32]; // BID UID-- password AID value-\n (31+1)
    snprintf(bid_request, sizeof(bid_request), "BID %6s %8s %03d %06d\n", uid, password, aid, value);
    connect_TCP(IP, port, bid_request, buffer, sizeof(buffer));

    if (!verify_buffer(buffer, strlen(buffer))) {
        printf("Error reading server response.\n");
        return;
    }

    // handles server response
    if (!strncmp(buffer, "RBD ACC", 7)) {
        printf("Your bid has been accepted!\n");
    } else if (!strncmp(buffer, "RBD NOK", 7)) {
        printf("Bid error: auction %03d is not active.\n", aid);
    } else if (!strncmp(buffer, "RBD NLG", 7)) {
        printf("Bid error: user is not logged in.\n");
    }  else if (!strncmp(buffer, "RBD REF", 7)) {
        printf("Bid refused: a larger bid has already been placed previously.\n");
    } else if (!strncmp(buffer, "RBD ILG", 7)) {
        printf("Bid refused: you cannot bid on an auction hosted by yourself!\n");
    } else printf("Server responded with an error when trying to bid.\n");

} 


void showRecord(char* IP, char* port, int aid) {
    char buffer[2000], showrecord_request[10]; // SRC AID\n
    char token[25]; // max token size is Fname = 24chars + 1 for \0
    char token2[25];
    int message_part = 0, first_bid = 1;
    snprintf(showrecord_request, sizeof(showrecord_request), "SRC %03d\n", aid);
    connect_UDP(IP, port, showrecord_request, buffer);

    if (!verify_buffer(buffer, strlen(buffer))) {
        printf("Error reading server response.\n");
        return;
    }

    // handles server response
    if (!strncmp(buffer, "RRC NOK", 7)) {
        printf("Error showing record: no such auction.\n");
    } else if (!strncmp(buffer, "RRC OK ", 7)) {
        
        printf("----------------Auction record---------------\n");
        for (int i = 7; i > 0; i++) { //7 = strlen("RRC OK ")
            memset(token, 0, sizeof(token));
            i = read_buffer_token(buffer, token, sizeof token, i);
            switch(message_part) {
                case 0: // Auction details
                    printf("Host UID: %s\n", token);
                    message_part++;
                    break;
                case 1:
                    printf("Auction Name: %s\n", token);
                    message_part++;
                    break;
                case 2:
                    printf("Asset Filename: %s\n", token);
                    message_part++;
                    break;
                case 3:
                    printf("Start Value: %s\n", token);
                    message_part++;
                    break;
                case 4:
                    i = read_buffer_token(buffer, token2, sizeof token2, ++i);
                    printf("Start Date-Time: %s %s\n", token, token2);
                    memset(token2, 0, sizeof(token2));
                    message_part++;
                    break;
                case 5:
                    printf("Time Active: %s\n", token);
                    message_part++;
                    break;
                case 6: // Bids
                    if (strlen(token) != 1){
                        printf("Error parsing server response.\n");
                        i = -1;
                        break;
                    }
                    if (token[0] == 'B') {
                        message_part = 7;
                    } else if (token[0] == 'E') {
                        message_part = 11;
                    } else {
                        printf("Error parsing server response.\n");
                        i = -1;
                    }
                    break;
                case 7: // bids information: only if there are bids
                    if (first_bid) {
                        printf("--------------------BIDS---------------------\n");
                        first_bid = 0;
                    } else printf("\n");

                    printf("Bidder UID: %s\n", token);
                    message_part++;
                    break;
                case 8:
                    printf("Bid Value: %s\n", token);
                    message_part++;
                    break;
                case 9:
                    i = read_buffer_token(buffer, token2, sizeof token2, ++i);
                    printf("Bid Date-Time: %s %s\n", token, token2);
                    memset(token2, 0, sizeof(token2));
                    message_part++;
                    break;
                case 10:
                    printf("Bid Sec Time: %s\n", token);
                    message_part = 6;
                    break;
                case 11: // Closing information: only if the auction is closed
                    printf("------------Closing Information--------------\n");
                    i = read_buffer_token(buffer, token2, sizeof token2, ++i);
                    printf("End Date-Time: %s %s\n", token, token2);
                    memset(token2, 0, sizeof(token2));
                    message_part++;
                    break;
                case 12:
                    printf("End Sec Time: %s\n", token);
                    i = -1;
                    break;
                default:
                    printf("Error parsing server response.\n");
                    break;
            }
        }

    } else printf("Server responded with an error when trying to show record.\n");
}
