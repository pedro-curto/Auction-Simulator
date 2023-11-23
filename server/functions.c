#include "server.h"

void handle_login(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len){
    char uid[100], password[100];
    char *status = "RLI ";
    sscanf(buffer, "LIN %s %s", uid, password);

    FILE *file = fopen("files/users", "r");
    if (file == NULL) {
        strcat(status, "NOK");
        reply_msg(udp_socket, client_addr, client_addr_len, status);
        return;
    }

    int n_users;
    fscanf(file, "%d", &n_users);
    if (n_users >= 999){
        strcat(status, "NOK");
        reply_msg(udp_socket, client_addr, client_addr_len, status);
        return;
    }

    char file_user[7], file_pass[9];
    int found_user = 0;
    while (fscanf(file, "%s %s", file_user, file_pass) != EOF){
        if (!strcmp(file_user, uid)){
            found_user = 1;
            if (!strcmp(file_pass, password)){
                strcat(status, "OK");
                break;
            } else {
                strcat(status, "NOK");
                break;
            }
        }
    }

    if (!found_user){
        if(create_user(uid, password)){
            strcat(status, "REG");
        } else {
            strcat(status, "NOK");
        }
    }

    reply_msg(udp_socket, client_addr, client_addr_len, status);

}

int create_user(char* uid, char* password){
    FILE *file = fopen("files/users", "rw");
    if (file == NULL) {
        return 0;
    }

    int num_users = 0;
    fscanf(file, "%d", &num_users); //   TODO corrigir isto
    // if (num_users == NULL){
    //     num_users = 0;
    // }
    num_users++;

    fseek(file, 0, SEEK_SET);
    fprintf(file, "%d\n", num_users);

    fseek(file, 0, SEEK_END);
    fprintf(file, "%s %s\n", uid, password);

    fclose(file);
    return 1;
}

void reply_msg(int udp_socket, struct sockaddr_in client_addr,socklen_t client_addr_len, char* status){
    if (sendto(udp_socket, status, strlen(status), 0, (struct sockaddr *)&client_addr, client_addr_len) == -1) {
        perror("UDP send error");
    }
}

void handle_logout(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len){

}
void handle_unregister(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len){

}
void handle_myauctions(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len){

}
void handle_mybids(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len){

}
void handle_list(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len){

}
void handle_show_record(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len){
    
}