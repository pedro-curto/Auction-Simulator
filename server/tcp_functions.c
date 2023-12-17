#include "server.h"

void handle_open(int tcp_socket) {
    // FIXME melhorar bastante as verificações, só quero testar se consigo abrir um auction bem
    char uid[7], password[9], name[11], asset_fname[25], start_valueStr[7], timeactiveStr[6], fsizeStr[9];
    //char uid[30], password[30], name[30], asset_fname[30], start_valueStr[30], timeactiveStr[30], fsizeStr[30];
    char status[50] = "ROA ";
    int start_value, timeactive, fsize, auction_id;
    
    if (read_field(tcp_socket, uid, 6) < 1 || 
        read_field(tcp_socket, password, 8) < 1 ||
        read_field(tcp_socket, name, 10) < 1 ||
        read_field(tcp_socket, start_valueStr, 6) < 1 ||
        read_field(tcp_socket, timeactiveStr, 5) < 1 ||
        read_field(tcp_socket, asset_fname, 24) < 1 ||
        read_field(tcp_socket, fsizeStr, 8) < 1)  {

            strcat(status, "ERR\n");
            write_tcp(tcp_socket, status);
            return;
    }
    if (!verify_open_args(uid, password, name, start_valueStr, timeactiveStr, asset_fname, fsizeStr)){
        strcat(status, "ERR\n");
        write_tcp(tcp_socket, status);
        return;
    }

    start_value = atoi(start_valueStr);
    timeactive = atoi(timeactiveStr);
    fsize = atoi(fsizeStr);
    
    if (verbose_mode){
        printf("Request type: Open auction\nuid: %s\n", uid);
    }
    printf("uid: %s\npassword: %s\nname: %s\nstart_value: %d\ntimeactive: %d\nasset_fname: %s\nfsize: %d\n", uid, password, name, start_value, timeactive, asset_fname, fsize);
    //pthread_mutex_lock(&mutex);
    if (!verify_user_exists(uid)) {
        strcat(status, "NOK\n");
    } else {
        if (!verify_password_correct(uid, password)) {
            strcat(status, "NOK\n");
        } else if (!is_user_login(uid)) {
            strcat(status, "NLG\n");
        } else {
            // OPA uid password name start_value timeactive Fname Fsize
            // FIXME isto é sempre verdade
            printf("before create_auction\n");
            if ((auction_id = create_auction(tcp_socket, uid, name, asset_fname, start_value, timeactive, fsize)) != 0) {
                sprintf(status, "ROA OK %03d\n", auction_id);
            } else {
                printf("something not ok\n");
                strcat(status, "NOK\n");
            }
        }
    }
    //pthread_mutex_unlock(&mutex);

    //reply_msg(tcp_socket, status); O reply_msg aqui vai ser fazer um write do status no socket
    // FIXME colocar isto num loop
    write_tcp(tcp_socket, status);
}


void handle_show_asset(int tcp_socket) { 
    printf("entering SA\n");
    char auc_id[5];
    char status[200] = "RSA ";

    if (read_field(tcp_socket, auc_id, 3) != -1) {
        printf("server: error reading auc_id: %s\n", auc_id);
        strcat(status, "ERR\n");
        write_tcp(tcp_socket, status);
        return;
    }
    if (!verify_aid(auc_id)) {
        strcat(status, "NOK\n");
        write_tcp(tcp_socket, status);
        return;
    }

    if (verbose_mode) {
        printf("Request type: Show asset\nauc_id: %s\n", auc_id);
    }

    //pthread_mutex_lock(&mutex);
    printf("auc_id: %s\n", auc_id);
    if (!exists_auction(auc_id)) {
        strcat(status, "NOK\n");
        write_tcp(tcp_socket, status);
        return;
    }

    strcat(status, "OK");
    get_auc_file_info(auc_id, status);

    strcat(status, " ");
    write_tcp(tcp_socket, status);
    send_auc_file(tcp_socket, auc_id);
    //pthread_mutex_unlock(&mutex);
    // falta escrever o \n 
    write_tcp(tcp_socket, "\n");
}


void handle_bid(int tcp_socket) {
    /*g) BID UID password AID value
    Following the bid command the User application opens a TCP connection
    with the AS and sends the AS a request to place a bid, with value value, for
    auction AID.*/
    char uid[7], password[9], aucIdStr[4], valueStr[7];
    char status[50] = "RBD ";
    int value, auction_id;

    if (read_field(tcp_socket, uid, 6) < 1 || 
        read_field(tcp_socket, password, 8) < 1 ||
        read_field(tcp_socket, aucIdStr, 3) < 1 ||
        read_field(tcp_socket, valueStr, 6) != -1)  {

            strcat(status, "ERR\n");
            write_tcp(tcp_socket, status);
            return;
    }

    if (!verify_bid_args(uid, password, aucIdStr, valueStr)){
        strcat(status, "ERR\n");
        write_tcp(tcp_socket, status);
        return;
    }

    value = atoi(valueStr);
    auction_id = atoi(aucIdStr);
    
    if (verbose_mode){
        printf("Request type: Bid\nuid: %s\n", uid);
    }

    // FIXME está aqui algum erro de raciocínio? São estas todas as condições possíveis? Deve-se ver se o auction existe?
    //pthread_mutex_lock(&mutex);
    if (!is_user_login(uid)) { // must be logged in
        strcat(status, "NLG\n");
    } else if (!verify_password_correct(uid, password)) {
        strcat(status, "NOK\n");
    } else if (!ongoing_auction(auction_id)) { // must be ongoing (active)
        strcat(status, "NOK\n");
    } else if (hosted_by_self(auction_id, uid)) { // must not be hosted by self
        strcat(status, "ILG\n");
    } else {
        if (bid_accepted(auction_id, value, uid)) {
            strcat(status, "ACC\n");
        } else {
            strcat(status, "REF\n");
        }
    }

    //pthread_mutex_unlock(&mutex);
    write_tcp(tcp_socket, status);
}


void handle_close(int tcp_socket) {
    /*c) CLS UID password AID
    Following the close command the User application opens a TCP connection
    with the AS and sends a request to close the auction with identifier AID, which
    had been opened by the logged in user, whose ID is UID.*/
    char uid[7], password[9], aucIdStr[4];
    int auction_id;
    char status[50] = "RCL ";

    if (read_field(tcp_socket, uid, 6) < 1 || 
        read_field(tcp_socket, password, 8) < 1 ||
        read_field(tcp_socket, aucIdStr, 3) != -1)  {

            strcat(status, "ERR\n");
            write_tcp(tcp_socket, status);
            return;
    }

    if (!verify_close_args(uid, password, aucIdStr)){
        strcat(status, "ERR\n");
        write_tcp(tcp_socket, status);
        return;
    }

    auction_id = atoi(aucIdStr);
    
    if (verbose_mode){
        printf("Request type: Close auction\nuid: %s\n", uid);
    }

    //pthread_mutex_lock(&mutex);
    if (!is_user_login(uid)) { // must be logged in
        strcat(status, "NLG\n");
    } else if (!verify_password_correct(uid, password)) {
        strcat(status, "NOK\n");
    } else if (!exists_auction(aucIdStr)) { // must exist
        strcat(status, "EAU\n");
    } else if (!hosted_by_self(auction_id, uid)) { // must be hosted by self
        strcat(status, "EOW\n");
    } else if (!ongoing_auction(auction_id)) { // must be ongoing (active)
        strcat(status, "END\n");
    } else {
        if (close_auction(auction_id)) {
            strcat(status, "OK\n");
        } else {
            strcat(status, "NOK\n");
        }
    }

    //pthread_mutex_unlock(&mutex);
    write_tcp(tcp_socket, status);
}


int verify_open_args(char* uid, char* password, char* name, char* start_valueStr, char* timeactiveStr, char* asset_fname, char* fsizeStr) {

    if (!verify_uid(uid)) {
        return 0;
    }
    if (!verify_password(password)) {
        return 0;
    }
    if (!verify_name(name)) {
        return 0;
    }
    if (!verify_start_value(start_valueStr)) {
        return 0;
    }
    if (!verify_timeactive(timeactiveStr)) {
        return 0;
    }
    if (!verify_asset_fname(asset_fname)) {
        return 0;
    }
    if (!verify_asset_fsize(fsizeStr)) {
        return 0;
    }
    return 1;
}

int verify_bid_args(char* uid, char* password, char* aid, char* value) {

    if (!verify_uid(uid)) {
        return 0;
    }
    if (!verify_password(password)) {
        return 0;
    }
    if (!verify_aid(aid)) {
        return 0;
    }
    if (!verify_bid_value(value)) {
        return 0;
    }
    return 1;
}

int verify_close_args(char* uid, char* password, char* aid) {
    if (!verify_uid(uid)) {
        return 0;
    }
    if (!verify_password(password)) {
        return 0;
    }
    if (!verify_aid(aid)) {
        return 0;
    }
    return 1;
}
