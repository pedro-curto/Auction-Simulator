#include "server.h"

int create_user(char* uid, char* password) {
    char path[50]; //users/uid/hosted

    sprintf(path, "users/%s", uid);
    mkdir(path, 0777); // creates users/uid
    sprintf(path, "users/%s/hosted", uid);
    mkdir(path, 0777); // creates users/uid/hosted
    sprintf(path, "users/%s/bidded", uid);
    mkdir(path, 0777); // creates users/uid/bidded
    // creates files
    sprintf(path, "users/%s/pass.txt", uid);
    FILE *pass_file = fopen(path, "w");
    if (pass_file == NULL) {
        return 0;
    }
    fprintf(pass_file, "%s", password);
    fclose(pass_file);
    sprintf(path, "users/%s/login.txt", uid);
    FILE *login_file = fopen(path, "w");
    fclose(login_file);
    return 1;
}

void reply_msg(int udp_socket, struct sockaddr_in client_addr,socklen_t client_addr_len, char* status) {
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
        //printf("User %s does not exist\n", uid);
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


void delete_user(char* uid) {
    char path[50] = "users/";
    char path2[50];
    strcat(path, uid);
    strcpy(path2, path);
    strcat(path, "/pass.txt");
    strcat(path2, "/login.txt");

    remove(path);
    remove(path2);
}

void user_auc_status(char* uid, char* status) {
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

// FIXME read_file? isto não é bem o melhor nome de sempre né
int read_file(int tcp_socket, int size, char* path) {
    char buffer[1024];
    int bytes_read = 0;
    ssize_t n;
    FILE *file = fopen(path, "w");
    if (file == NULL) {
        perror("fopen error");
        return 0;
    }
    while (bytes_read < size) {
        n = read(tcp_socket, buffer, 1024); // read in chunks
        if (n <= 0) {
            perror("TCP read error");
            return 0;
        }
        bytes_read += n;
        // write to file
        fwrite(buffer, 1, n, file);
    }
    fclose(file);
    return bytes_read;
}


// FIXME enviar antes uma estrutura com informações da auction? devíamos apagar o diretório em qualquer return 0?
int create_auction(int tcp_socket, char* uid, char* name, char* asset_fname, int start_value, int timeactive, int fsize) {
    int auction_id = get_next_auction_id();
    // need to create: users/uid/hosted/auction_id.txt; 
    // auctions/auction_id (directory); auctions/auction_id/START_auction_id.txt; auctions/auction_id/asset (directory);
    // auctions/auction_id/bids (directory); auctions/auction_id/asset/asset_fname (5 total)
    // UID name asset_fname start value timeactive start_datetime start_fulltime
    char path[500];
    // creates hosted file txt
    sprintf(path, "users/%s/hosted/%03d.txt", uid, auction_id); // users/uid/hosted/auction_id.txt
    FILE *hosted_file = fopen(path, "w"); // creates auction_id.txt in users/uid/hosted (1/5)
    if (hosted_file == NULL) {
        perror("Could not create auction file in user directory.\n");
        return 0;
    }
    // creates directory in auctions folder with asset and bids and start file
    sprintf(path, "auctions/%03d", auction_id);
    if (mkdir(path, 0777)) { // auctions/auction_id (2/5)
        perror("Could not create auction directory.\n");
        return 0;
    }
    strcat(path, "/asset");
    if (mkdir(path, 0777)) { // auctions/auction_id/asset (3/5)
        perror("Could not create asset directory.\n");
        return 0;
    }
    sprintf(path, "auctions/%03d/bids", auction_id);
    if (mkdir(path, 0777)) { // auctions/auction_id/bids (4/5)
        perror("Could not create bids directory.\n");
        return 0;
    }
    sprintf(path, "auctions/%03d/START_%03d.txt", auction_id, auction_id);
    FILE *auction_file = fopen(path, "w"); // auctions/auction_id/START_auction_id.txt (5/5)
    if (auction_file == NULL) {
        perror("Could not create auction file in auction directory.\n");
        return 0;
    }
    // handles time and date and then writes to file
    time_t t = time(NULL);
    struct tm *local_time = localtime(&t);
    char start_datetime[20]; // YYYY-MM-DD HH:MM:SS (19 bytes)
    strftime(start_datetime, sizeof(start_datetime), "%Y-%m-%d %H:%M:%S", local_time);
    fprintf(auction_file, "%s %s %s %d %d %s %ld", uid, name, asset_fname, 
    start_value, timeactive, start_datetime, t);

    fclose(auction_file);
    // creates and stores asset file
    sprintf(path, "auctions/%03d/asset/%s", auction_id, asset_fname);
    if (!read_file(tcp_socket, fsize, path)) {
        perror("Could not create and store asset file.\n");
        return 0;
    }
    return 1;
}



int exists_auctions() {
    DIR* dir = opendir("auctions");
    
    if (readdir(dir) == NULL) {
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


void append_auctions(char* status) {
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

int exists_auction(char* auc_id) {
    char path[50] = "auctions/";
    strcat(path, auc_id);
    DIR* dir = opendir(path);
    if (dir == NULL){
        return 0;
    }
    closedir(dir);
    return 1;
}

int get_auc_file_info(char* auc_id, char* status) {
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


int send_auc_file(int tcp_socket, char* auc_id) {
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


int get_next_auction_id() {
    DIR* dir = opendir("auctions");
    struct dirent *entry;
    int max = 0;
    while ((entry = readdir(dir)) != NULL) {
        // Exclude "." and ".." entries
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            int auc_id = atoi(entry->d_name);
            if (auc_id > max) {
                max = auc_id;
            }
        }
    }
    closedir(dir);
    return max + 1;
}


