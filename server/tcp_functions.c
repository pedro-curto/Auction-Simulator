#include "server.h"

void handle_open(int tcp_socket, char *buffer, int *auction_id) {
    (void) auction_id;
    //char uid[7], password[9], name[11], asset_fname[25], start_valueStr[7], timeactiveStr[6], fsizeStr[9];
    char uid[30], password[30], name[30], asset_fname[30], start_valueStr[30], timeactiveStr[30], fsizeStr[30];
    char status[50] = "ROA ";
    int start_value, timeactive, fsize;
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
    printf("uid: %s\npassword: %s\nname: %s\nstart_value: %d\ntimeactive: %d\nasset_fname: %s\nfsize: %d\n", uid, password, name, start_value, timeactive, asset_fname, fsize);




    /*if (!verify_user_exists(uid)) {
        strcat(status, "NOK\n");
    } else {
        if (!verify_password_correct(uid, password)) {
            strcat(status, "NOK\n");
        } else if (!is_user_login(uid)) {
            strcat(status, "NOK\n");
        } else {
            // OPA uid password name start_value timeactive Fname Fsize 
            sscanf(buffer, "OPA %6s %8s %10s %d %d %24s %d", uid, password, name, &start_value, &timeactive, asset_fname, &fsize);
            uid[strlen(uid)] = '\0';
            password[strlen(password)] = '\0';
            name[strlen(name)] = '\0';
            asset_fname[strlen(asset_fname)] = '\0';

            if (create_auction(uid, name, start_value, timeactive, asset_fname, fsize)) {
                strcat(status, "OK\n");
            } else {
                strcat(status, "NOK\n");
            }
        }
    }*/

    //reply_msg(tcp_socket, status); O reply_msg aqui vai ser fazer um write do status no socket
    if (write(tcp_socket, status, strlen(status)) == -1) {
        perror("TCP write error");
    }

}


