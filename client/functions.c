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
    
    //if (!strncmp(connect_UDP(IP, port, login_request, buffer), "Error", 5)) return 0; //FIXME acho q o "else" la em baixo trata do assunto se der erro aqui
    connect_UDP(IP, port, login_request, buffer);
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
    /*if (!strncmp(connect_UDP(IP, port, logout_request, buffer), "error", 5)) {
        printf("Error logging out.\n"); // acho q isto nunca vai acontecer, o connect_UDP nao da returna  num "error" FIXME
        return 0;
    }*/
    connect_UDP(IP, port, logout_request, buffer);
    // checks server response
    if (!strncmp(buffer, "RLO OK", 6)) {
        printf("User was correctly logged out.\n");
        return 1;
    } else if (!strncmp(buffer, "RLO NOK", 7)){
        printf("Error logging out: user was not logged in.\n");
    } else if(!strncmp(buffer, "RLO UNR", 7)) {
        printf("User was not registered.\n");
    } else printf("Error logging out\n");
    //printf("ur leaving logout func now \n");
    return 0;
}

int unregister(char* IP, char* port, char* uid, char* password) {
    // printf("! You're inside unregister function !\n");
    char buffer[128], unregister_request[100];
    snprintf(unregister_request, sizeof(unregister_request), "UNR %s %s\n", uid, password);
    // establishes UDP connection with server and sends request
    /*if (!strncmp(connect_UDP(IP, port, unregister_request, buffer), "error", 5)) {
        printf("Error connecting to unregister.\n");
        return 0;
    }*/
    connect_UDP(IP, port, unregister_request, buffer);
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


/* 
list or l – the User application sends a message to the AS, using the UDP
protocol, asking for a list of the currently active auctions.
The AS will reply with the requested list, or an information that no auctions are
currently active. This information should be displayed to the User.
LST
Following the list command the User sends the AS an auction list request.
RLS status[ AID state]*
In reply to a LST request the AS reply status is NOK if no auction was yet
started. If there are ongoing auctions the reply status is OK and a list of the
identifiers AID and state for all auctions, separated by single spaces, is sent
by the AS. state takes value 1 if the auction is active, or 0 otherwise. 
*/
void listAllAuctions(char* IP, char* port) { // uses UDP protocol
    char buffer[8096], *list_request = "LST\n"; // FIXME buffer size? 4096?
    // memset(.., .., n*sizeof(char)) substituir por isto?
    memset(buffer, 0, sizeof(buffer));
    // establishes UDP connection with server and sends request
    connect_UDP(IP, port, list_request, buffer);
    //printf("buffer: %s\n", buffer);
    // checks server response
    if (!strncmp(buffer, "RLS OK", 6)) {
        printf("Auctions currently open:\n");
        char *token = strtok(buffer+7, "OK ");
        // FIXME comparar as duas implementações abaixo. a primeira tem dois \ns e a segunda um, not sure
        /*while (token != NULL) {
            printf("%s ", token);
            token = strtok(NULL, " ");
            printf("%s\n", token);
            if (token != NULL)
            token = strtok(NULL, " ");
        }*/
        while (token != NULL) {
            printf("%s ", token);
            // Move to the next token
            token = strtok(NULL, " \n");
            if (token != NULL) {
                printf("%s\n", token);
            }
            // Move to the next token
            token = strtok(NULL, " \n");
        }
    }
    else if (!strncmp(buffer, "RLS NOK", 7)) {
        printf("No auctions are currently open.\n");
    } else {
        printf("Unexpected server response: %s\n", buffer); // case RLS ERR
    }
}

/* Open a new auction. The User application sends a message to the AS asking to open
a new auction, providing a short description name (represented with up to 10
alphanumerical characters), an image (or other document file) of the asset to sell, the
start value (represented with up to 6 digits), and the duration of the auction in
seconds (represented with up to 5 digits). In reply, the AS informs if the request was
successful, and the assigned auction identifier, AID, a 3-digit number.
open name asset_fname start_value timeactive 
The User application establishes a TCP session with the AS and sends a message asking to
open a new auction, whose short description name is name, providing an image
of the asset to sell, stored in the file asset_fname, indicating the start value
for the auction, start_value, and the duration of the auction, timeactive.
In reply, the AS sends a message indicating whether the request was successful,
and the assigned auction identifier, AID, which should be displayed to the User.
After receiving the reply from the AS, the User closes the TCP connection.
OPA UID password name start_value timeactive Fname Fsize Fdata
Following the open command the User application opens a TCP connection
with the AS and asks to open a new auction. The information sent includes:
• a short description name (a single word): name
• the minimum selling value for the asset: start_value
• the duration of the auction in minutes: timeactive
• the filename where an image of the asst to be sold is included: Fname
• the file size in bytes: Fsize
• the contents of the selected file: Fdata
*/

// escrever o cabeçalho todo, e só depois enviar os dados através de uma função à parte em que envio 512 a 512
// exemplo de função para enviar o ficheiro depois à parte: sendfile() (procurar a entry do man)

void openAuction(char* IP, char* port, char* uid, char* password, char* input) {
    char name[NAME_SIZE + 1], asset_fname[ASSET_FNAME_SIZE + 1]; //uid[7], password[9];
    char buffer[1024], request_header[100];//, fsizeStr[9];
    int start_value, timeactive, aid;
    sscanf(input, "%s %s %d %d", name, asset_fname, &start_value, &timeactive);
    int fsize = getFileSize(asset_fname);
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
    printf("buffer: %s\n", buffer);
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


/* close AID – the User application sends a message to the AS, using the TCP
protocol, asking to close an ongoing auction, with identifier AID, that had been
started by the logged in user.
The AS will reply informing whether the auction was successfully closed,
cancelling the sale, or if the auction time had already ended. This information
should be displayed to the User. After receiving the reply from the AS, the User
closes the TCP connection.
*/
void closeAuction(char* IP, char* port, char* uid, char* password, char* input) {
    char buffer[1024], close_request[25]; // close request is CLS user-- password AID\n
    int aid;
    // receives close AID, where AID must be a 3-digit number between 0 and 999
    printf("INPUT in closeAuction: %s", input);
    sscanf(input, "%d", &aid);
    printf("AID: %d\n", aid);
    if (aid < 0 || aid > 999) {
        printf("Invalid close attempt: AID must be a 3-digit number between 0 and 999.\n");
        return;
    }
    snprintf(close_request, sizeof(close_request), "CLS %6s %8s %03d\n", uid, password, aid);
    /*if (!strncmp(connect_TCP(IP, port, close_request, buffer, sizeof(buffer)), "error", 5)) {
        printf("Error while connecting to close an auction.\n");
        return;
    }*/
    connect_TCP(IP, port, close_request, buffer, sizeof(buffer));
    // handles server response
    //printf("Server response buffer: %s\n", buffer);
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
    char buffer[MA_BUFFER_SIZE], myauctions_request[12]; // LMA UID---\n
    printf("uid: %s\n", uid);
    snprintf(myauctions_request, sizeof(myauctions_request), "LMA %6s\n", uid);
    // FIXME este strncmp não faz sentido nenhum, o connect_UDP e connect_TCP deveriam funcionar sendo void
    connect_UDP(IP, port, myauctions_request, buffer);
    // handles server response
    if (!strncmp(buffer, "RMA NOK", 7)) {
        printf("User has no ongoing auctions.\n");
    } else if (!strncmp(buffer, "RMA NLG", 7)) {
        printf("User is not logged in.\n");
    } else if (!strncmp(buffer, "RMA OK", 6)) { 
    // FIXME isto precisa de ser repensado quando os opens começarem a funcionar e pudermos testar isto a sério
    // não sei se tá certo mas quis skippar o OK e assim parece estar a funcionar (?)
        printf("User %6s auctions:\n", uid);
        char *token = strtok(buffer, "RMA OK");
        //token = strtok(NULL, " ");
        //token = strtok(NULL, " ");
        while (token != NULL) {
            printf("%s ", token);
            // Move to the next token
            token = strtok(NULL, " \n");
            if (token != NULL) {
                printf("%s\n", token);
            }
            // Move to the next token
            token = strtok(NULL, " \n");
        }
    } else printf("Server responded with an error when trying to list auctions.\n"); // in this case we got ERR

}


/* mybids or mb – the User application sends a message to the AS, using the
UDP protocol, asking for a list of the auctions for which the logged in user has
placed a bid.
The AS will reply with the requested list, or an information that the user has no
active auction bids. This information should be displayed to the User. 
LMB UID
Following the mybids command the User sends the AS a request to list the
auctions for which the user UID has made bids.
RMB status[ AID state]*
In reply to a LMB request the AS reply status is NOK if user UID has no
ongoing bids. If the user is not logged in the reply status is NLG. If there are
ongoing bids for user UID the reply status is OK and a list of the identifiers
AID and state for all ongoing auctions for which this user has placed bids,
separated by single spaces, is sent by the AS. state takes value 1 if the auction
is active, or 0 otherwise.*/
void myBids(char* IP, char* port, char* uid) { // uses UDP protocol
    // FIXME parsing and printing need a bit of fixing
    char buffer[1024], mybids_request[12]; // LMB UID---\n
    snprintf(mybids_request, sizeof(mybids_request), "LMB %6s\n", uid);
    connect_UDP(IP, port, mybids_request, buffer);
    // handles server response
    if (!strncmp(buffer, "RMB NOK", 7)) {
        printf("You have no ongoing bids!\n");
    } else if (!strncmp(buffer, "RMB NLG", 7)) {
        printf("Bid listing error: user is not logged in.\n");
    } else if (!strncmp(buffer, "RMB OK", 6)) {
        // FIXME
        printf("Your current bids:\n");
        char *token = strtok(buffer + 3, " "); //RMB OK?
        while ((token = strtok(NULL, " ")) != NULL) {
            printf("%s", token);
            token = strtok(NULL, " ");
            printf(" %s\n", token);
        }
    } else printf("Server responded with an error when trying to list bids.\n");
}


/*show_asset AID or sa AID – the User establishes a TCP session with
the AS and sends a message asking to receive the image file of the asset in sale
for auction number AID.
In reply, the AS sends the required file, or an error message. The file is stored
and its name and the directory of storage are displayed to the User. After
receiving the reply from the AS, the user closes the TCP connection.
SAS AID
Following the show_asset command the User application opens a TCP
connection with the AS and asks to receive the image illustrating the asset for
sale in auction AID.
RSA status [Fname Fsize Fdata]
In reply to a SAS request the AS replies with status = OK and sends a file
containing the image illustrative of the asset for sale. The information sent
includes:
• the filename Fname;
• the file size Fsize, in bytes;
• the contents of the selected file (Fdata).
The file is locally stored using the filename Fname.
The User displays the name and size of the stored file.
If there is no file to be sent, or some other problem, the AS replies with
status = NOK.
After receiving the reply message, the User closes the TCP connection with the
AS.

*/
// Falta guardar o ficheiro
void showAsset(char* IP, char* port, int aid) {
    char showasset_request[SA_BUFFER_SIZE];//, char *buffer; | SAS AID\n
    char buffer[1000], asset_fname[ASSET_FNAME_SIZE + 1];
    //int fsize, fd;
    memset(buffer, 0, sizeof(buffer));
    
    snprintf(showasset_request, sizeof(showasset_request), "SAS %03d\n", aid);

    int tcp_socket = connect_tcp(IP, port);
    // sends request to server
    if (write(tcp_socket, showasset_request, strlen(showasset_request)) < 0) {
        printf("Error sending request to server.\n");
        return;
    }

    // receives server response
    char status1[5], status2[5], fname[ASSET_FNAME_SIZE + 1], fsize[9], fdata[512];
    char path[50] = "assets/";
    //mudar isto sff TODO
    if (!read_field(tcp_socket, status1, 3)) {
        printf("Error reading server response.\n");
        return;
    }
    if (!read_field(tcp_socket, status2, 3)) {
        printf("field1: %s\n", status1);
        printf("field2: %s\n", status2);
        printf("Error reading server response.\n");
        return;
    }
    printf("status1: %s\nstatus2: %s\n", status1, status2);
    if (!strncmp(status2, "NOK", 3)) {
        printf("Error showing asset: no such auction.\n");
        return;
    } else if (!strncmp(status2, "OK", 2)) {
        printf("Asset successfully shown!\n");
    } else if (!strncmp(status2, "ERR", 3)) {
        printf("Server responded with an error when trying to show asset.\n");
        return;
    } else printf("Badly formatted server response: %s\n", status1);

    if (!read_field(tcp_socket, fname, sizeof(fname))) {
        printf("Error reading server response.\n");
        return;
    }
    if (!read_field(tcp_socket, fsize, sizeof(fsize))) {
        printf("Error reading server response.\n");
        return;
    }
    strcat(path, fname);
    printf("fname: %s\n", fname);
    printf("fsize: %s\n", fsize);
    read_file(tcp_socket, atoi(fsize), path);

}

/*bid AID value or b AID value – the User application sends a message
to the AS, using the TCP protocol, asking to place a bid for auction AID of value
value.
The AS will reply reporting the result of the bid: accepted, refused (if value is
not larger than the previous highest bid), or informing that the auction is no
longer active. The user is not allowed to bid in an auction hosted by him. This
information should be displayed to the User. After receiving the reply from the
AS, the User closes the TCP connection.
BID UID password AID value
Following the bid command the User application opens a TCP connection
with the AS and sends the AS a request to place a bid, with value value, for
auction AID.
RBD status
In reply to a BID request the AS reply status is NOK if auction AID is not
active. If the user was not logged in the reply status is NLG. If auction AID is
ongoing the reply status is ACC if the bid was accepted. The reply status is
REF if the bid was refused because a larger bid has already been placed
previously. The reply status is ILG if the user tries to make a bid in an
auction hosted by himself.
After receiving the reply message, the User closes the TCP connection with the
AS. 
*/
void bid(char* IP, char* port, char *uid, char* password, int aid, int value) { // uses TCP protocol
    char buffer[30000], bid_request[32]; // BID UID-- password AID value-\n (31+1)
    snprintf(bid_request, sizeof(bid_request), "BID %6s %8s %03d %06d\n", uid, password, aid, value);
    connect_TCP(IP, port, bid_request, buffer, sizeof(buffer));
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

/* show_record AID or sr AID – the User application sends a message to
the AS, using the UDP protocol, asking to see the record of auction AID.
The AS will reply with the auction details, including the list of received bids and
information if the auction is already closed. This information should be
displayed to the User.
SRC AID
Following the show_record command the User sends the AS a request for
the record of auction AID.
RRC status [host_UID auction_name asset_fname start_value start_date-time timeactive]
[ B bidder_UID bid_value bid_date-time bid_sec_time]*
[ E end_date-time end_sec_time]
In reply to a SRC request the AS reply status is NOK if the auction AID does
not exist. Otherwise the reply status is OK followed by information about the
ID host_UID of the user that started the auction, the auction name
auction_name and the name of the file asset_fname with information
about the item being sold, the minimum bid value start_value, and the start
date and time start_date-time of the auction in the format YYYY-MMDD HH:MM:SS (19 bytes), as well as the duration of the auction timeactive
in seconds (represented using 6 digits).
If this auction has received bids then a description of each bid is presented in a
separate line starting with B and including: the ID of the user that place this bid
bidder_UID, the bid value bid_value, the bid date and time
bid_date-time in the format YYYY-MM-DD HH:MM:SS (19 bytes), as
well as the number of seconds elapsed since the beginning of the auction until
the bid was made bid_sec_time (represented using 6 digits).
In case the auction is already closed there is one last line added to the reply
including the date and time of the auction closing end_date-time in the
format YYYY-MM-DD HH:MM:SS (19 bytes), as well as the number of
seconds elapsed since the beginning of the auction until the bid was made
end_sec_time.*/
void showRecord(char* IP, char* port, int aid) {
    // FIXME properly calculate the size of the buffer
    char buffer[1024], showrecord_request[10]; // SRC AID\n
    char token[25]; //max token size is Fname = 24chars + 1 for \0
    char token2[25];
    int message_part = 0;
    snprintf(showrecord_request, sizeof(showrecord_request), "SRC %03d\n", aid);
    connect_UDP(IP, port, showrecord_request, buffer);
    
    // handles server response
    if (!strncmp(buffer, "RRC NOK", 7)) {
        printf("Error showing record: no such auction.\n");
    } else if (!strncmp(buffer, "RRC OK ", 7)) {
        // FIXME: precisa de ser feito corretamente
        printf("Auction record----------------------------\n");
        for (int i = 7; i > 0; i++) { //7 = strlen("RRC OK ")
            memset(token, 0, sizeof(token));
            i = read_buffer_token(buffer, token, i);
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
                    i = read_buffer_token(buffer, token2, ++i);
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
                        message_part = 8;
                    } else {
                        printf("Error parsing server response.\n");
                        i = -1;
                    }
                    break;
                case 7: // bids information: only if there are bids
                    printf("Bids--------------------------------------\n");
                    printf("Bidder UID: %s\n", token);
                    i = read_buffer_token(buffer, token, ++i);
                    printf("Bid Value: %s\n", token);
                    i = read_buffer_token(buffer, token, ++i);
                    i = read_buffer_token(buffer, token2, ++i);
                    printf("Bid Date-Time: %s %s\n", token, token2);
                    i = read_buffer_token(buffer, token, ++i);
                    printf("Bid Sec Time: %s\n", token);
                    message_part = 6;
                    break;
                case 8: // Closing information: only if the auction is closed
                    printf("Closing Information-----------------------\n");
                    i = read_buffer_token(buffer, token2, ++i);
                    printf("End Date-Time: %s %s\n", token, token2);
                    memset(token2, 0, sizeof(token2));
                    i = read_buffer_token(buffer, token, ++i);
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

int read_buffer_token(char* buffer, char* token, int start_pos){
    int i,j;
    for (i = start_pos, j = 0; buffer[i] != ' ' && i < 1024 && i > -1; i++,j++){
        if (buffer[i] == '\n'){
            token[j] = '\0';
            return -1;
        }
        token[j] = buffer[i];
    }
    token[j] = '\0';
    if (i == 1024) return -1;
    return i;
}



        /*
        printf("buffer: %s\n", buffer);
        char *token = strtok(buffer, " "); // skips RRC
        printf("skipped token1: %s\n", token);
        token = strtok(NULL, " "); // skips OK
        printf("skipped token2: %s\n", token);
        // Auction details
        printf("Auction details:\n");
        printf("Host UID: %s\n", strtok(NULL, " "));
        printf("Auction Name: %s\n", strtok(NULL, " "));
        printf("Asset Filename: %s\n", strtok(NULL, " "));
        printf("Start Value: %s\n", strtok(NULL, " "));
        printf("Start Date-Time: %s %s\n", strtok(NULL, " "), strtok(NULL, " "));
        printf("Time Active: %s\n", strtok(NULL, " "));
        // Bids
        // FIXME parsing is being done incorrectly! check this. Consider the token "B"
        printf("Bids:\n");
        while (1) {
            // Format is: [ B bidder_UID bid_value bid_date-time bid_sec_time]* so I need to skip the initial "B"
            token = strtok(NULL, " "); // skips B
            if (token == NULL || !strcmp(token, "E")) {
                break; // Exit the loop when 'E' is encountered or when there are no more tokens
            }
            printf("Bidder UID: %s\n", strtok(NULL, " "));
            //printf("Bidder UID: %s\n", token);
            printf("Bid Value: %s\n", strtok(NULL, " "));
            printf("Bid Date-Time: %s %s\n", strtok(NULL, " "), strtok(NULL, " "));
            printf("Bid Sec Time: %s\n", strtok(NULL, " "));
        }
        // Closing information: only makes sense if the auction is closed
        if (token != NULL && !strcmp(token, "E")) {
            printf("Closing Information:\n");
            printf("End Date-Time: %s %s\n", strtok(NULL, " "), strtok(NULL, " "));
            printf("End Sec Time: %s\n", strtok(NULL, " "));
        } else printf("Auction is still ongoing!\n");*/
            