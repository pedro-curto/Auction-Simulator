#include "server.h"

int create_user(char* uid, char* password){
    char path[50] = "users/";
    char path2[50];
    strcat(path, uid);
    strcpy(path2, path);

    printf("path1: %s\n", path);

    mkdir(path, 0777);

    strcat(path, "/pass.txt");
    strcat(path2, "/login.txt");
    FILE *pass_file = fopen(path, "w");
    if (pass_file == NULL) {
        perror("fopen error");
        return 0;
    }
    fprintf(pass_file, "%s", password);
    fclose(pass_file);

    FILE *login_file = fopen(path2, "w");
    if (login_file == NULL) {
        perror("fopen error");
        return 0;
    }
    fprintf(login_file, "%s", "1");
    fclose(login_file);
    return 1;
}

void reply_msg(int udp_socket, struct sockaddr_in client_addr,socklen_t client_addr_len, char* status){
    if (sendto(udp_socket, status, strlen(status), 0, (struct sockaddr *)&client_addr, client_addr_len) == -1) {
        perror("UDP send error");
    }
}

int verify_user_exists(char* uid){
    char path[50] = "users/";
    strcat(path, uid);
    DIR *dir = opendir(path);
    if (dir == NULL) {
        printf("User %s does not exist\n", uid);
        return 0;
    }
    closedir(dir);
    strcat(path, "/pass.txt");
    FILE *pass_file = fopen(path, "r");
    if (pass_file == NULL) {
        printf("User %s does not exist anymore\n", uid);
        return 0;
    }
    close(pass_file);
    return 1;
}

int is_user_login(char* uid){
    char path[50] = "users/";
    strcat(path, uid);
    strcat(path, "/login.txt");
    FILE *login_file = fopen(path, "r");
    if (login_file == NULL) {
        perror("fopen error");
        return -1;
    }
    char login;
    fscanf(login_file, "%c", &login);
    fclose(login_file);
    return login;
}

void change_user_login(char* uid, char status){
    char path[50] = "users/";
    strcat(path, uid);
    strcat(path, "/login.txt");
    FILE *login_file = fopen(path, "r");

    fprintf(login_file, "%c", status);
    fclose(login_file);
}


int verify_password_correct(char* uid, char* password){
    char path[50] = "users/";
    strcat(path, uid);
    strcat(path, "/pass.txt");
    FILE *pass_file = fopen(path, "r");
    if (pass_file == NULL) {
        perror("fopen error");
        fclose(pass_file);
        return 0;
    }
    char correct_pass[50];
    fscanf(pass_file, "%s", correct_pass);
    if (strcmp(correct_pass, password)){
        fclose(pass_file);
        return 0;
    }
    fclose(pass_file);
    return 1;
}

void delete_user(char* uid){
    char path[50] = "users/";
    char path2[50];
    strcat(path, uid);
    strcpy(path2, path);
    strcat(path, "/pass.txt");
    strcat(path2, "/login.txt");

    remove(path);
    remove(path2);
}