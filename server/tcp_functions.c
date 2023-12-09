#include "server.h"

void handle_open(int tcp_socket) {
    // FIXME melhorar bastante as verificações, só quero testar se consigo abrir um auction bem
    char uid[7], password[9], name[11], asset_fname[25], start_valueStr[7], timeactiveStr[6], fsizeStr[9];
    //char uid[30], password[30], name[30], asset_fname[30], start_valueStr[30], timeactiveStr[30], fsizeStr[30];
    char status[50] = "ROA ";
    int start_value, timeactive, fsize, auction_id;
    // OPA uid password name start_value timeactive Fname Fsize
    /* a) OPA UID password name start_value timeactive Fname
    Fsize Fdata
    Following the open command the User application opens a TCP connection
    with the AS and asks to open a new auction. The information sent includes:
    • a short description name (a single word): name
    • the minimum selling value for the asset: start_value
    • the duration of the auction in minutes: timeactive
    • the filename where an image of the asst to be sold is included: Fname
    • the file size in bytes: Fsize */
    read_field(tcp_socket, uid, 6);
    read_field(tcp_socket, password, 8);
    read_field(tcp_socket, name, 10);
    read_field(tcp_socket, start_valueStr, 6);
    read_field(tcp_socket, timeactiveStr, 5);
    read_field(tcp_socket, asset_fname, 24);
    read_field(tcp_socket, fsizeStr, 8);
    start_value = atoi(start_valueStr);
    timeactive = atoi(timeactiveStr);
    fsize = atoi(fsizeStr);
    //printf("uid: %s\npassword: %s\nname: %s\nstart_value: %d\ntimeactive: %d\nasset_fname: %s\nfsize: %d\n", uid, password, name, start_value, timeactive, asset_fname, fsize);
    // TODO missing verifications
    if (!verify_user_exists(uid)) {
        strcat(status, "NOK\n");
    } else {
        if (!verify_password_correct(uid, password)) {
            strcat(status, "NOK\n");
        } else if (!is_user_login(uid)) {
            strcat(status, "NLG\n");
        } else {
            // OPA uid password name start_value timeactive Fname Fsize
            if ((auction_id = create_auction(tcp_socket, uid, name, asset_fname, start_value, timeactive, fsize))) {
                sprintf(status, "ROA OK %03d\n", auction_id);
            } else {
                strcat(status, "NOK\n");
            }
        }
    }

    //reply_msg(tcp_socket, status); O reply_msg aqui vai ser fazer um write do status no socket
    // FIXME colocar isto num loop
    if (write(tcp_socket, status, strlen(status)) == -1) {
        perror("TCP write error");
    }

}

void handle_show_asset(int tcp_socket) {
    char auc_id[5];
    int fsize;
    char status[50] = "RSA ";
    (void) fsize; (void) status;
    read_field(tcp_socket, auc_id, 3);
    
    if (!exists_auction(auc_id)){
        strcat(status, "NOK\n");
        write(tcp_socket, status, strlen(status));
        return;
    }
    strcat(status, "OK");
    get_auc_file_info(auc_id, status);
    strcat(status, " ");
    write(tcp_socket, status, strlen(status));
    send_auc_file(tcp_socket, auc_id);
}

void handle_bid(int tcp_socket) {
    /*g) BID UID password AID value
    Following the bid command the User application opens a TCP connection
    with the AS and sends the AS a request to place a bid, with value value, for
    auction AID.*/
    char uid[7], password[9], aucIdStr[4], valueStr[7];
    char status[50] = "RBD ";
    int value, ret_val, auction_id;
    read_field(tcp_socket, uid, 6);
    read_field(tcp_socket, password, 8);
    read_field(tcp_socket, aucIdStr, 3);
    read_field(tcp_socket, valueStr, 6);
    value = atoi(valueStr);
    auction_id = atoi(aucIdStr);
    printf("uid: %s\npassword: %s\nauction_id: %d\nvalue: %d\n", uid, password, auction_id, value);
    /* h) RBD status
    In reply to a BID request the AS reply status is NOK if auction AID is not
    active. If the user was not logged in the reply status is NLG. If auction AID is
    ongoing the reply status is ACC if the bid was accepted. The reply status is
    REF if the bid was refused because a larger bid has already been placed
    previously. The reply status is ILG if the user tries to make a bid in an
    auction hosted by himself.
    After receiving the reply message, the User closes the TCP connection with the
    AS.*/
    // TODO falta implementar o bid_accepted
    if (!is_user_login(uid)) { // must be logged in
        strcat(status, "NLG\n");
    } else if (!ongoing_auction(auction_id)) { // must be ongoing (active)
        strcat(status, "NOK\n");
    } else if (hosted_by_self(auction_id, uid)) { // must not be hosted by self
        strcat(status, "ILG\n");
    } else if ((ret_val = bid_accepted(auction_id, value, uid)) == 1) { // must be accepted (bigger than a previous one)
        strcat(status, "ACC\n");
        if (ret_val == 0) strcat(status, "REF\n");

    } else {
        strcat(status, "ERR\n");
    }

    write(tcp_socket, status, strlen(status));
}

void handle_close(int tcp_socket) {
    (void) tcp_socket;
    /*c) CLS UID password AID
    Following the close command the User application opens a TCP connection
    with the AS and sends a request to close the auction with identifier AID, which
    had been opened by the logged in user, whose ID is UID.




    d) RCL status
    In reply to a CLS request the AS replies informing whether it was able to close
    auction AID. The reply status is OK, if auction AID was ongoing, it was
    started by user UID, and could be successfully closed by the AS. If the user was
    not logged in the reply status is NLG. The status is EAU, if the auction
    AID does not exist. status is EOW, if the auction is not owned by user UID,
    and status is END, if auction AID owned by user UID has already finished.
    After receiving the reply message, the User closes the TCP connection with the
    AS.*/

    
}

