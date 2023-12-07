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
        return 0;
    }
    fprintf(pass_file, "%s", password);
    fclose(pass_file);

    FILE *login_file = fopen(path2, "w");
    fclose(login_file);
    return 1;
}

void reply_msg(int udp_socket, struct sockaddr_in client_addr,socklen_t client_addr_len, char* status){
    if (sendto(udp_socket, status, strlen(status), 0, (struct sockaddr *)&client_addr, client_addr_len) == -1) {
        perror("UDP send error");
    }
}

int verify_user_exists(char* uid) {
    char path[50] = "users/";
    struct stat st;
    strcat(path, uid);
    DIR *dir = opendir(path);
    if (dir == NULL) {
        printf("User %s does not exist\n", uid);
        return 0;
    }
    closedir(dir);
    strcat(path, "/pass.txt");
    if (!stat(path, &st)) { //0 = file exists
        return 1;
    }
    return 0;
}



int is_user_login(char* uid) {
    char path[50] = "users/";
    struct stat st;
    strcat(path, uid);
    strcat(path, "/login.txt");
    if (!stat(path, &st)) { //0 = file exists
        return 1;
    }
    return 0;
}

void change_user_login(char* uid) {
    char path[50] = "users/";
    strcat(path, uid);
    strcat(path, "/login.txt");
    FILE *login_file = fopen(path, "r");
    if (login_file == NULL) {
        login_file = fopen(path, "w");
        fclose(login_file);
    } else {
        remove(path);
    }
}


int verify_password_correct(char* uid, char* password) {
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
    if (strcmp(correct_pass, password)) {
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

void user_auc_status(char* uid, char* status){
    char user_auctions[9999];
    char path[50] = "users/";

    strcat(path, uid);
    fetch_auctions(path, user_auctions);

    char* auc_uid = strtok(user_auctions, " ");
    while (auc_uid != NULL){
        strcat(status, " ");
        strcat(status, auc_uid);
        if (is_auc_active(auc_uid)){
            strcat(status, " 1");
        }
        else{
            strcat(status, " 0");
        }
        auc_uid = strtok(NULL, " ");
    }
}

void fetch_auctions(char* path, char* auctions) {
    char auc_uid[50];
    DIR* hosted_dir = opendir(path);

    struct dirent* hosted_file;
    while ((hosted_file = readdir(hosted_dir)) != NULL) {
        scanf(hosted_file->d_name, "%s.txt", auc_uid);
        strcat(auctions, auc_uid);
        strcat(auctions, " ");
        memset(auc_uid, 0, sizeof(auc_uid));
    }
    auctions[strlen(auctions) - 1] = '\n';
    closedir(hosted_dir);
}

int is_auc_active(char* auc_uid){
    char path[50] = "auctions/";
    struct stat st;
    strcat(path, auc_uid);
    strcat(path, "/active.txt");
    if (!stat(path, &st)) {
        return 1;
    }
    return 0;
}


// usa isto
int read_field(int tcp_socket, char *buffer, size_t size) {
    size_t bytes_read = 0;
    ssize_t n;
    // check if the first character read is a space
    
    while (bytes_read <= size) {
        n = read(tcp_socket, buffer + bytes_read, 1); // read one byte at a time
        if (n <= 0) {
            perror("TCP read error");
            return 0;
        }
        bytes_read += n;
        // at any time, if we read a space we stop
        if (buffer[bytes_read-1] == ' ') { // 103091  11111111 abcdefgh
            break;
        } 
    }
    buffer[bytes_read-1] = '\0';
    return bytes_read;
}


int read_file(int tcp_socket, size_t size, char* path) {
    char buffer[1024];
    size_t bytes_read = 0;
    ssize_t n;
    
    while (bytes_read < size) {
        n = read(tcp_socket, buffer, size); // read in chunks
        if (n <= 0) {
            perror("TCP read error");
            return 0;
        }
        bytes_read += n;

        // write to file
        FILE *file = fopen(path, "w");
        if (file == NULL) {
            perror("fopen error");
            return 0;
        }
        fwrite(buffer, 1, n, file);
        fclose(file);
        

    }
    return bytes_read;
}


