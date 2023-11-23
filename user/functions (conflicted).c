#include "user.h"


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
    
    if (!strncmp(connect_UDP(IP, port, login_request, buffer), "Error", 5)) return 0; //FIXME acho q o "else" la em baixo trata do assunto se der erro aqui

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
    if (!strncmp(connect_UDP(IP, port, logout_request, buffer), "error", 5)) {
        printf("Error logging out.\n"); // acho q isto nunca vai acontecer, o connect_UDP nao da returna  num "error" FIXME
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
    //printf("ur leaving logout func now \n");
    return 0;
}

int unregister(char* IP, char* port, char* uid, char* password) {
    // printf("! You're inside unregister function !\n");
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
    char buffer[1024], *list_request = "LST\n";
    // establishes UDP connection with server and sends request
    if (!strncmp(connect_UDP(IP, port, list_request, buffer), "error", 5)) {
        printf("Error while connecting to list all auctions.\n");
        return;
    }
    // checks server response
    if (!strncmp(buffer, "RLS OK", 6)) {
        printf("Auctions currently open:\n");
        char *token = strtok(buffer+7, "OK ");
        while (token != NULL) {
            printf("%s ", token);
            token = strtok(NULL, " ");
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
successful, and the assigned auction identifier, AID, a 3-digit number.
open name asset_fname start_value timeactive 
The User application establishes a TCP session with the AS and sends a message asking to
open a new auction, whose short description name is name, providing an image
of the asset to sell, stored in the file asset_fname, indicating the start value
for the auction, start_value, and the duration of the auction, timeactive.
In reply, the AS sends a message indicating whether the request was successful,
and the assigned auction identifier, AID, which should be displayed to the User.
After receiving the reply from the AS, the User closes the TCP connection.
OPA UID password name start_value timeactive Fname
Fsize Fdata
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
    char name[NAME_SIZE + 1], asset_fname[ASSET_FNAME_SIZE + 1];
    char buffer[1024], request_header[100];//, fsizeStr[9];
    int start_value, timeactive, aid;
    // printf("IP in main open: %s\nport in main open: %s\n", IP, port);
    sscanf(input, "%s %s %d %d", name, asset_fname, &start_value, &timeactive);
    int fsize = getFileSize(asset_fname);
    printf("name: %s\nasset_fname: %s\nstart_value: %d\ntimeactive: %d\nfile size: %d\n", name, asset_fname, start_value, timeactive, fsize);
    // necessary checks 
    if (fsize == -1) {
        printf("Error opening file.\n");
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

    //snprintf(fsizeStr, sizeof(fsizeStr), "%08jd", fsize);
    snprintf(request_header, sizeof(request_header), "OPA %s %s %s %d %d %s %d", uid, password, name, start_value, timeactive,
             asset_fname, fsize);
    //size_t open_request_size = 100 + strlen(uid) + strlen(password) + strlen(name) + strlen(asset_fname) + strlen(fsizeStr) + fsize;
    //char *open_request = (char*) malloc(open_request_size);
    // char open_request[open_request_size];
    //snprintf(open_request,  "OPA %s %s %s %d %d %s %s %s\n", 
             //uid, password, name, start_value, timeactive, asset_fname, fsizeStr, Fdata);

    if(!strncmp(connect_TCP(IP, port, request_header, buffer, sizeof(buffer)), "error", 5)) {
        printf("Error while trying to establish TCP connection to open auction.\n");
        //free(Fdata);
        //free(open_request);
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
    } else printf("Error opening auction.\n");

    //free(Fdata);
    //free(open_request);
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
    if (!strncmp(connect_TCP(IP, port, close_request, buffer, sizeof(buffer)), "error", 5)) {
        printf("Error while connecting to close an auction.\n");
        return;
    }
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
void myAuctions(char* IP, char* port, char* uid, char* password) {
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
    char buffer[1024], mybids_request[12]; // LMB UID---\n
    (void) IP; (void) port; (void) uid;
    snprintf(mybids_request, sizeof(mybids_request), "LMB %6s\n", uid);
    if (!strncmp(connect_UDP(IP, port, mybids_request, buffer), "error", 5)) {
        printf("Error while connecting to list user's bids.\n");
        return;
    }
    // handles server response
    if (!strncmp(buffer, "RMB NOK", 7)) {
        printf("Bid listing error: user has no ongoing bids.\n");
    } else if (!strncmp(buffer, "RMB NLG", 7)) {
        printf("Bid listing error: user is not logged in.\n");
    } else if (!strncmp(buffer, "RMB OK", 6)) {
        // FIXME
        printf("Auctions currently open:\n");
        char *token = strtok(buffer, " ");
        token = strtok(NULL, " ");
        while (token != NULL) {
            printf("%s", token);
            token = strtok(NULL, " ");
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

void showAsset(char* IP, char* port, int aid) {
    char buffer[1024], showasset_request[10]; // SAS AID\n
    snprintf(showasset_request, sizeof(showasset_request), "SAS %3d\n", aid);
    if (!strncmp(connect_TCP(IP, port, showasset_request, buffer, sizeof(buffer)), "error", 5)) {
        printf("Error while connecting to show asset.\n");
        return;
    }
    // handles server response
    if (!strncmp(buffer, "RSA NOK", 7)) {
        printf("Error showing asset: no such file.\n");
    } else if (!strncmp(buffer, "RSA OK", 6)) {
        // FIXME
        printf("Auctions currently open:\n");
        char *token = strtok(buffer, " "); // skips RSA
        token = strtok(NULL, " ");
        if (token != NULL) {
            // Extract filename, size, and data
            char *filename = strtok(NULL, " "); // fname
            char *size = strtok(NULL, " "); // fsize
            char *data = strtok(NULL, "\0"); // fdata
            printf("Filename: %s\nSize: %s\n", filename, size);
        }
    } else printf("Server responded with an error when trying to show asset.\n");

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
void bid(char* IP, char* port, int aid, int value) { // uses TCP protocol
    char buffer[1024], bid_request[16]; // BID AID value\n
    snprintf(bid_request, sizeof(bid_request), "BID %3d %6d\n", aid, value);
    if (!strncmp(connect_TCP(IP, port, bid_request, buffer, sizeof(buffer)), "error", 5)) {
        printf("Error while connecting to bid.\n");
        return;
    }
    // handles server response
    if (!strncmp(buffer, "RBD ACC", 7)) {
        printf("Your bid has been accepted!\n");
    } else if (!strncmp(buffer, "RBD NOK", 7)) {
        printf("Bid error: auction %3d is not active.\n", aid);
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
RRC status [host_UID auction_name asset_fname start_value
start_date-time timeactive]
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
    (void) IP; (void) port; (void) aid;
    char buffer[1024], showrecord_request[10]; // SRC AID\n

    snprintf(showrecord_request, sizeof(showrecord_request), "SRC %03d\n", aid);
    if (!strncmp(connect_UDP(IP, port, showrecord_request, buffer), "error", 5)) {
        printf("Error while connecting to show record.\n");
        return;
    }
    printf("showrecord_request: %s\n", showrecord_request);
    printf("buffer: %s\n", buffer);
    // handles server response
    if (!strncmp(buffer, "RRC NOK", 7)) {
        printf("Error showing record: no such auction.\n");
    } else if (!strncmp(buffer, "RRC OK", 6)) {
        // FIXME: precisa de ser feito corretamente
        printf("Auction record:\n");
        char *token = strtok(buffer, " "); // skips RRC
        token = strtok(NULL, " ");
        token = strtok(NULL, " ");
        if (token != NULL) {
            // Extract filename, size, and data
            char *filename = strtok(NULL, " "); // fname
            char *size = strtok(NULL, " "); // fsize
            char *data = strtok(NULL, "\0"); // fdata
            printf("Filename: %s\nSize: %s\n", filename, size);
        }
    } else printf("Server responded with an error when trying to show record.\n");
}
