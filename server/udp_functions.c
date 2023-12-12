#include "server.h"

void handle_login(int udp_socket, struct sockaddr_in client_addr, char* buffer, socklen_t client_addr_len) {
    char status[50] = "RLI ";
    char uid[UID_SIZE];
    char password[PASSWORD_SIZE];

    // read_uid_udp(buffer, uid);
    // read_password_udp(buffer, password);
    // if (strlen(uid) == 0 || strlen(password) == 0) {
    //     strcat(status, "ERR\n");
    //     reply_msg(udp_socket, client_addr, client_addr_len, status);
    //     return;
    // }

    sscanf(buffer, "LIN %s %s", uid, password);
    uid[strlen(uid)] = '\0';

    if (!verify_user_exists(uid)) {
        if (create_user(uid,password)) {
            strcat(status, "REG\n");
        } else {
            strcat(status, "NOK\n");
        }
    } else {
        //printf("before password\n");
        if (!verify_password_correct(uid, password)) {
            strcat(status, "NOK\n");
        } else if (is_user_login(uid)) {
            printf("User %s already logged in\n", uid);
            strcat(status, "NOK\n");
        } else {
            //printf("before change_user_login\n");
            change_user_login(uid);
            strcat(status, "OK\n");
        }
    }
    reply_msg(udp_socket, client_addr, client_addr_len, status);
}


void handle_logout(int udp_socket, struct sockaddr_in client_addr, char* buffer, socklen_t client_addr_len) {
    char uid[20], password[20];
    char status[50] = "RLO ";
    sscanf(buffer, "LOU %s %s", uid, password);
    uid[strlen(uid)] = '\0';

    if(!verify_user_exists(uid)){
        strcat(status, "UNR\n");
    } else{
        if (!verify_password_correct(uid, password)){
            strcat(status, "NOK\n");
        } else if (!is_user_login(uid)){
            strcat(status, "NOK\n");
        } else{
            change_user_login(uid);
            strcat(status, "OK\n");
        }
    }
    reply_msg(udp_socket, client_addr, client_addr_len, status);
}


void handle_unregister(int udp_socket, struct sockaddr_in client_addr, char* buffer, socklen_t client_addr_len) {
    char uid[100], password[50];
    char status[50] = "RUR ";
    sscanf(buffer, "UNR %s %s", uid, password);
    uid[strlen(uid)] = '\0';

    if (!verify_user_exists(uid)) {
        strcat(status, "NOK\n");
    } else {
        if (!verify_password_correct(uid, password)) {
            strcat(status, "NOK\n");
        } else if (!is_user_login(uid)) {
            strcat(status, "NOK\n");
        } else {
            delete_user(uid);
            strcat(status, "OK\n");
        }
    }
    reply_msg(udp_socket, client_addr, client_addr_len, status);
}



void handle_myauctions(int udp_socket, struct sockaddr_in client_addr, char* buffer, socklen_t client_addr_len) {
    char uid[7];
    char status[9999] = "RMA ";
    // we need to guarantee that we read exactly three characters and a space afterwards
    if (sscanf(buffer, "LMA %6[0-9]\n", uid) != 1) {
        strcat(status, "ERR\n"); // message wrongly formatted
        reply_msg(udp_socket, client_addr, client_addr_len, status);
        return;
    }
    printf("uid: %s\n", uid);
    if (!is_user_login(uid)) {
        strcat(status, "NLG\n");
    } else {
        strcat(status, "OK");
        if (!user_auc_status(uid, status)) {
            sprintf(status, "NOK");
        }
        strcat(status, "\n");
    }
    printf("status: %s\n", status);
    reply_msg(udp_socket, client_addr, client_addr_len, status);
}


void handle_mybids(int udp_socket, struct sockaddr_in client_addr, char* buffer, socklen_t client_addr_len) {
    char uid[7];
    char status[9999] = "RMB ";
    // we need to guarantee that we read exactly three characters and a space afterwards
    sscanf(buffer, "LMB %s\n", uid);
    // if (sscanf(buffer, "LMA %s\n", uid) != 1) {
    //     strcat(status, "ERR\n"); // message wrongly formatted
    //     reply_msg(udp_socket, client_addr, client_addr_len, status);
    //     return;
    // }
    if (!is_user_login(uid)) {
        strcat(status, "NLG\n");
    } else {
        strcat(status, "OK");
        if (!user_bids_status(uid, status)) {
            sprintf(status, "RMB NOK");
        }
        strcat(status, "\n");
    }
    printf("status: %s\n", status);
    reply_msg(udp_socket, client_addr, client_addr_len, status);
}


void handle_list(int udp_socket, struct sockaddr_in client_addr, char* buffer, socklen_t client_addr_len) {
    char uid[100];
    char status[9999] = "RLS ";

    sscanf(buffer, "UNR %s", uid);
    uid[strlen(uid)] = '\0';
    if (access("auctions", F_OK) == -1) {
        strcat(status, "NOK\n");
    } else {
        strcat(status, "OK");
        append_auctions(status);
        strcat(status, "\n");
    }

    // FIXME epa acho que o access() faz literalmente o mesmo que tu fazes no exists_auctions()
    /*if (!exists_auctions()) {
        strcat(status, "NOK\n");
    } else {
        strcat(status, "OK");
        append_auctions(status);
    }*/

    reply_msg(udp_socket, client_addr, client_addr_len, status);
}

void handle_show_record(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len) {
    char auc_id[5];
    char status[200] = "RRC ";
    sscanf(buffer, "SRC %s", auc_id);
    printf("auc_id: %s\n", auc_id);
    
    if (!exists_auction(auc_id)) {
        strcat(status, "NOK\n");
        reply_msg(udp_socket, client_addr, client_addr_len, status);
        return;
    }
    strcat(status, "OK");
    get_auc_info(auc_id, status);
    reply_msg(udp_socket, client_addr, client_addr_len, status);
}

