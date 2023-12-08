#include "server.h"

int create_user(char* uid, char* password) {
    char path[50] = "users/"; //users/uid/hosted
    char path2[50];
    char path3[50];
    char path4[50];
    strcat(path, uid);
    strcpy(path2, path);

    printf("path1: %s\n", path);
    strcat(path3, "/bidded");
    strcat(path4, "/hosted");
    mkdir(path, 0777);
    mkdir(path3, 0777);
    mkdir(path4, 0777);

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
        // at any time, if we read a space or \n we stop
        if (buffer[bytes_read-1] == ' ') {
            break;
        }
    }
    // if (buffer[bytes_read-1] != '\n' && buffer[bytes_read-1] != ' '){
    //     return -1;
    // }
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

// FIXME enviar antes uma estrutura com informações da auction?
int create_auction(char* uid, char* name, char* asset_fname, int start_value, int timeactive, int* auction_id) {
    // UID name asset_fname start value timeactive start_datetime start_fulltime
    char path[50] = "users/";
    char path2[50] = "auctions/";
    // creates txt in users directory first
    sprintf(path, "%03d", *auction_id);
    sprintf(path2, "%03d", *auction_id); // alterei cenas nesta func pra dar make, dps continua TODO

    FILE *auction_file = fopen(path, "w");
    if (auction_file == NULL) {
        perror("Could not create auction file in user directory.\n");
        return 0;
    }
    // handles time and date and then writes to file
    time_t t = time(NULL);
    struct tm *local_time = localtime(&t);
    char start_datetime[20]; // YYYY-MM-DD HH:MM:SS (19 bytes)
    strftime(start_datetime, sizeof(start_datetime), "%Y-%m-%d %H:%M:%S", local_time);
    fprintf(auction_file, "%s %s %s %d %d %ld %s", uid, name, asset_fname, 
    start_value, timeactive, t, start_datetime);
    fclose(auction_file);
    // TODO creation on auctions directory
    return 1;
}



int exists_auctions(){
    DIR* dir = opendir("auctions");
    
    if (readdir(dir) == NULL){
        return 0;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("entry: %s\n", entry->d_name);
        // Exclude "." and ".." entries
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            closedir(dir);
            return 1;  // Directory is not empty
        }
    }

    closedir(dir);
    return 0;
}

void append_auctions(char* status){
    DIR* dir = opendir("auctions");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Exclude "." and ".." entries
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            strcat(status, " ");
            strcat(status, entry->d_name);
            if (is_auc_active(entry->d_name)){
                strcat(status, " 1");
            }
            else{
                strcat(status, " 0");
            }
        }
    }
    closedir(dir);
    strcat(status, "\n");
}

int exists_auction(char* auc_id){
    char path[50] = "auctions/";
    strcat(path, auc_id);
    DIR* dir = opendir(path);
    if (dir == NULL){
        return 0;
    }
    closedir(dir);
    return 1;
}

int get_auc_file_info(char* auc_id, char* status){
    char path[50] = "auctions/";
    strcat(path, auc_id);
    strcat(path, "/asset/");
    DIR *dir = opendir(path);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Exclude "." and ".." entries
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            strcat(status, entry->d_name);
            strcat(status, " ");
            strcat(path, entry->d_name);
            struct stat st;
            stat(path, &st);
            sprintf(status, "%ld", st.st_size);
            closedir(dir);
            return 0;
        }
    }
    closedir(dir);
    return -1;
}

int send_auc_file(int tcp_socket, char* auc_id){
    char path[50] = "auctions/";
    strcat(path, auc_id);
    strcat(path, "/asset/");
    DIR *dir = opendir(path);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Exclude "." and ".." entries
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            strcat(path, entry->d_name);
            FILE *file = fopen(path, "r");
            if (file == NULL) {
                perror("fopen error");
                return 0;
            }
            char buffer[1024];
            size_t bytes_read;
            while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
                write(tcp_socket, buffer, bytes_read);
            }
            write(tcp_socket, "\n", 1);
            fclose(file);
            closedir(dir);
            return 1;
        }
    }
    closedir(dir);
    return -1;
}