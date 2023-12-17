#include "server.h"

int create_user(char* uid, char* password) {
    char path[100]; //server/users/uid/hosted
    
    sprintf(path, "server/users/%s", uid);
    mkdir(path, 0777); // creates server/users/uid
    sprintf(path, "server/users/%s/hosted", uid);
    mkdir(path, 0777); // creates server/users/uid/hosted
    sprintf(path, "server/users/%s/bidded", uid);
    mkdir(path, 0777); // creates server/users/uid/bidded
    // creates files
    sprintf(path, "server/users/%s/pass.txt", uid);
    FILE *pass_file = fopen(path, "w");
    if (pass_file == NULL) {
        return 0;
    }
    fprintf(pass_file, "%s", password);
    fclose(pass_file);
    sprintf(path, "server/users/%s/login.txt", uid);
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
    char path[100] = "server/users/";
    struct stat st;
    strcat(path, uid);
    DIR *dir = opendir(path);
    if (dir == NULL) {
        return 0;
    }
    closedir(dir);
    strcat(path, "/pass.txt");
    if (!stat(path, &st)) { // 0 = file exists
        return 1;
    }
    return 0;
}



int is_user_login(char* uid) {
    char path[50] = "server/users/";
    struct stat st;
    strcat(path, uid);
    strcat(path, "/login.txt");
    if (!stat(path, &st)) { // 0 = file exists
        return 1;
    }
    return 0;
}


void change_user_login(char* uid) {
    char path[50] = "server/users/";
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
    // checks if the given uid and password match the ones in the file
    char path[50] = "server/users/";
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
    char path[50] = "server/users/";
    char path2[50];
    strcat(path, uid);
    strcpy(path2, path);
    strcat(path, "/pass.txt");
    strcat(path2, "/login.txt");

    remove(path);
    remove(path2);
}

int user_auc_status(char* uid, char* status) {
    char user_auctions[9999];
    memset(user_auctions, 0, sizeof(user_auctions));
    char path[50];
    sprintf(path, "server/users/%s/hosted/", uid);
    if (is_directory_empty(path)) {
        return 0;
    }
    fetch_auctions(path, user_auctions);

    char* auc_uid = strtok(user_auctions, " ");
    while (auc_uid != NULL) {
        strcat(status, " ");
        strcat(status, auc_uid);
        if (ongoing_auction(atoi(auc_uid))){
            strcat(status, " 1");
        } else {
            strcat(status, " 0");
        }
        auc_uid = strtok(NULL, " ");
    }
    return 1;
}

int user_bids_status(char* uid, char* status) {
    char user_auctions[9999];
    memset(user_auctions, 0, sizeof(user_auctions));
    char path[50];
    sprintf(path, "server/users/%s/bidded/", uid);
    if (is_directory_empty(path)) {
        return 0;
    }
    fetch_auctions(path, user_auctions);

    char* auc_uid = strtok(user_auctions, " ");
    while (auc_uid != NULL) {
        strcat(status, " ");
        strcat(status, auc_uid);
        if (ongoing_auction(atoi(auc_uid))){
            strcat(status, " 1");
        } else {
            strcat(status, " 0");
        }
        auc_uid = strtok(NULL, " ");
    }
    return 1;
}

void fetch_auctions(char *path, char *auctions) {
    struct dirent **auctions_list;

    int num_entries = scandir(path, &auctions_list, NULL, alphasort);

    if (num_entries < 0) {
        perror("scandir error");
        return;
    }

    for (int i = 0; i < num_entries; ++i) {
        if (strcmp(auctions_list[i]->d_name, ".") && strcmp(auctions_list[i]->d_name, "..")) {
            char auc_id[4];
            memset(auc_id, 0, sizeof(auc_id));

            // server/auctions/auction_id.txt, so we must get the auction_id section
            strncpy(auc_id, auctions_list[i]->d_name, strlen(auctions_list[i]->d_name) - 4);
            auc_id[AID_SIZE] = '\0';

            strcat(auctions, auc_id);
            strcat(auctions, " ");

            free(auctions_list[i]);
        }
    }

    free(auctions_list);
}



int read_field(int tcp_socket, char *buffer, size_t size) {
    size_t bytes_read = 0;
    ssize_t n;
    int first_read = 1;
    // check if the first character read is a space
    while (bytes_read <= size) {
        n = read(tcp_socket, buffer + bytes_read, 1); // read one byte at a time
        if (n <= 0) {
            perror("TCP read error");
            return 0;
        }
        if (first_read) {
            if (buffer[0] == ' ') {
                return 0;
            }
            first_read = 0;
        }
        bytes_read += n;
        // at any time, if we read a space or \n we stop
        if (buffer[bytes_read-1] == ' ') {
            buffer[bytes_read-1] = '\0';
            return bytes_read;
        }
        if (buffer[bytes_read-1] == '\n') {
            buffer[bytes_read-1] = '\0';
            return -1;
        }
    }
    return 0;
}


int store_file(int tcp_socket, int size, char* path) {
    char buffer[1024];
    int bytes_read = 0;
    ssize_t n;
    FILE *file = fopen(path, "w");
    if (file == NULL) {
        perror("fopen error");
        return 0;
    }
    while (bytes_read < size) {
        if (size - bytes_read < 1024) {
            n = read(tcp_socket, buffer, size - bytes_read); // read the remaining bytes
        } else {
            n = read(tcp_socket, buffer, 1024); // read in chunks
        }
        if (n <= 0) {
            perror("TCP read error");
            return 0;
        }
        bytes_read += n;
        // write to file
        fwrite(buffer, 1, n, file);
    }
    fclose(file);

    read(tcp_socket, buffer, 1); // read the last \n
    if (buffer[0] != '\n') {
        perror("TCP read error");
        return 0;
    }
    return bytes_read;
}

int create_auction(int tcp_socket, char* uid, char* name, char* asset_fname, int start_value, int timeactive, int fsize) {
    int auction_id = get_next_auction_id();
    struct tm *local_time;
    char path[100], start_datetime[20];// YYYY-MM-DD HH-MM-SS (19 bytes)
    
    sprintf(path, "server/users/%s/hosted/%03d.txt", uid, auction_id); // server/users/uid/hosted/auction_id.txt
    FILE *hosted_file = fopen(path, "w"); // creates auction_id.txt in server/users/uid/hosted (1/5)
    if (hosted_file == NULL) {
        perror("Could not create auction file in user directory.\n");
        return 0;
    }
    // creates directory in auctions folder with asset and bids and start file
    sprintf(path, "server/auctions/%03d", auction_id);
    if (mkdir(path, 0777)) { // server/auctions/auction_id (2/5)
        perror("Could not create auction directory.\n");
        return 0;
    }
    strcat(path, "/asset");
    if (mkdir(path, 0777)) { // server/auctions/auction_id/asset (3/5)
        perror("Could not create asset directory.\n");
        return 0;
    }
    sprintf(path, "server/auctions/%03d/bids", auction_id);
    if (mkdir(path, 0777)) { // server/auctions/auction_id/bids (4/5)
        perror("Could not create bids directory.\n");
        return 0;
    }
    sprintf(path, "server/auctions/%03d/START_%03d.txt", auction_id, auction_id);
    FILE *auction_file = fopen(path, "w"); // server/auctions/auction_id/START_auction_id.txt (5/5)
    if (auction_file == NULL) {
        perror("Could not create auction file in auction directory.\n");
        return 0;
    }

    time_t t = time(NULL);
    local_time = localtime(&t); 
    strftime(start_datetime, sizeof(start_datetime), "%Y-%m-%d %H:%M:%S", local_time);
    fprintf(auction_file, "%s %s %s %d %d %s %ld", uid, name, asset_fname, 
    start_value, timeactive, start_datetime, t);
    fclose(auction_file);
    // creates and stores asset file
    sprintf(path, "server/auctions/%03d/asset/%s", auction_id, asset_fname);
    if (!store_file(tcp_socket, fsize, path)) {
        perror("Could not create and store asset file.\n");
        return 0;
    }
    return auction_id;
}


int exists_auctions() {
    DIR* dir = opendir("server/auctions");
    if (readdir(dir) == NULL) {
        return 0;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Exclude "." and ".." entries
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            closedir(dir);
            return 1;  // Directory is not empty
        }
    }

    closedir(dir);
    return 0;
}


int is_directory_empty(char *path) {
    DIR* dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        return -1;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Exclude "." and ".." entries
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            closedir(dir);
            return 0;  // directory is not empty
        }
    }
    closedir(dir);
    return 1;  // directory is empty
}


void append_auctions(char *status) {
    struct dirent **auctions_list;
    int num_entries = scandir("server/auctions", &auctions_list, NULL, alphasort);

    if (num_entries < 0) {
        perror("scandir error");
        return;
    }

    for (int i = 0; i < num_entries; ++i) {
        if (strcmp(auctions_list[i]->d_name, ".") && strcmp(auctions_list[i]->d_name, "..")) {
            strcat(status, " ");
            strcat(status, auctions_list[i]->d_name);

            if (ongoing_auction(atoi(auctions_list[i]->d_name))) {
                strcat(status, " 1");
            } else {
                strcat(status, " 0");
            }

            free(auctions_list[i]);
        }
    }

    free(auctions_list);
    strcat(status, "\n");
}


int exists_auction(char* auc_id) {
    char path[50] = "server/auctions/";
    strcat(path, auc_id);
    DIR* dir = opendir(path);
    if (dir == NULL) {
        return 0;
    }
    closedir(dir);
    return 1;
}


int get_auc_file_info(char* auc_id, char* status) {
    char path[50] = "server/auctions/";
    char size[10];
    strcat(path, auc_id);
    strcat(path, "/asset/");
    DIR *dir = opendir(path);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Exclude "." and ".." entries
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            strcat(status, " ");
            strcat(status, entry->d_name);
            strcat(status, " ");
            strcat(path, entry->d_name);
            struct stat st;
            stat(path, &st);
            sprintf(size, "%d", (int) st.st_size);
            strcat(status, size);
            closedir(dir);
            return 0;
        }
    }
    closedir(dir);
    return -1;
}

void write_tcp(int tcp_socket, char* status) {
    size_t n = 0;
    while (n < strlen(status)) {
        n += write(tcp_socket, status + n, strlen(status) - n);
    }
}


int send_auc_file(int tcp_socket, char* auc_id) {
    char path[100] = "server/auctions/";
    struct stat st;
    strcat(path, auc_id);
    strcat(path, "/asset/");
    DIR *dir = opendir(path);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Exclude "." and ".." entries
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            strcat(path, entry->d_name);
            int file = open(path, O_RDONLY);
            if (file == -1) {
                perror("fopen error");
                closedir(dir);
                return 0;
            }
            stat(path, &st);
            ssize_t fsize = st.st_size;
            off_t offset = 0;
            ssize_t sent_bytes = sendfile(tcp_socket, file, &offset, fsize);
            if (sent_bytes == -1) perror("Error sending file.\n");
            write(tcp_socket, "\n", 1);
            close(file);
            closedir(dir);
            return 1;
        }
    }
    closedir(dir);
    return -1;
}


int get_next_auction_id() {
    DIR* dir = opendir("server/auctions");
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

// needs to check if there is an END.txt file; if not, checks if we need to create one
int ongoing_auction(int auction_id) {
    // START.txt is composed like: UID name asset_fname start value timeactive start_datetime start_fulltime
    // we need to add timeactive to start_fulltime and compare it to a call to time() (current time)
    char path[100];
    time_t current_time, start_fulltime, timeactive, closure_fulltime;
    struct tm *closure_datetime;
    char time_str[20];
    // checks if there exists an END.txt file
    sprintf(path, "server/auctions/%03d/END_%03d.txt", auction_id, auction_id);
    if (access(path, F_OK) != -1) {
        return 0;
    }
    // if there is no END.txt file, we check if we need to create one
    sprintf(path, "server/auctions/%03d/START_%03d.txt", auction_id, auction_id);
    FILE *start_file = fopen(path, "r");
    if (start_file == NULL) {
        perror("fopen error");
        return 0;
    }
    // START.txt: UID name asset_fname start_value timeactive start_datetime (2 fields) start_fulltime
    fscanf(start_file, "%*s %*s %*s %*s %ld %*s %*s %ld", &timeactive, &start_fulltime);
    fclose(start_file);
    closure_fulltime = start_fulltime + timeactive;
    time(&current_time);
    if (current_time >= closure_fulltime) {

        // auction is over -> create END.txt file: end_datetime end_sec_time
        // end_datetime -> date of auction closure (YYYY-MM-DD HH:MM:SS)
        // end_sec_time -> time in seconds during which the auction was active
        // here, the auction wasn't closed prematurely so end_sec_time is the same as timeactive

        sprintf(path, "server/auctions/%03d/END_%03d.txt", auction_id, auction_id);
        FILE *end_file = fopen(path, "w");
        if (end_file == NULL) {
            perror("fopen error");
            return 0;
        }
        closure_datetime = localtime(&closure_fulltime);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", closure_datetime);
        fprintf(end_file, "%s %ld", time_str, timeactive);
        fclose(end_file);
        return 0;
    }
    return 1;
}



int hosted_by_self(int auction_id, char* uid) {
    char path[100];
    sprintf(path, "server/users/%s/hosted/", uid);
    DIR *dir = opendir(path);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // exclude "." and ".." entries
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            if (atoi(entry->d_name) == auction_id) {
                closedir(dir);
                return 1;
            }
        }
    }
    closedir(dir);
    return 0;
}


int bid_accepted(int auction_id, int value, char* uid) {
    // first we see if there is any placed bid
    char path[100];
    int max_bid = 0, start_value;
    time_t start_fulltime;
    sprintf(path, "server/auctions/%03d/bids/", auction_id);
    DIR *dir = opendir(path);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // exclude "." and ".." entries
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            // entry->d_name is the bid value
            int bid_value = atoi(entry->d_name);
            if (bid_value > max_bid) {
                max_bid = bid_value;
            }
        }
    }
    closedir(dir);
    sprintf(path, "server/auctions/%03d/START_%03d.txt", auction_id, auction_id);
    FILE *start_file = fopen(path, "r");
    if (start_file == NULL) {
        perror("fopen error");
        return 0;
    }
    // we need to get the start_fulltime anyways; we also get the start_value in case there are no bids
    fscanf(start_file, "%*s %*s %*s %d %*s %*s %*s %ld", &start_value, &start_fulltime);
    fclose(start_file);
    // if max_bid is 0, it takes the value of start_value, else it stays the same
    max_bid = max_bid > start_value ? max_bid : start_value;
    if (value > max_bid) {
        // create file with bid value
        if (create_bid_files(auction_id, value, uid, start_fulltime)) {
            return 1;
        }
    }
    return 0;
}


// creates a new bid file with name bid_value.txt
// and the contents: UID bid_value bid_datetime bid_sec_time
int create_bid_files(int auction_id, int value, char* uid, time_t start_fulltime) {
    char path[100];
    time_t current_fulltime;
    struct tm *current_datetime;
    char time_str[20]; // YYYY-MM-DD HH:MM:SS (19 bytes) + \0
    // creates file on auctions directory (server/auctions/aid/bids)
    sprintf(path, "server/auctions/%03d/bids/%d.txt", auction_id, value);
    FILE *bid_file = fopen(path, "w");
    if (bid_file == NULL) {
        perror("Could not create a bid file in the auctions directory.");
        return 0;
    }
    time(&current_fulltime);
    current_datetime = localtime(&current_fulltime); // convert time to YYYY-MM-DD HH:MM:SS. current_time points to a struct of type tm
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", current_datetime);
    fprintf(bid_file, "%s %d %s %ld", uid, value, time_str, current_fulltime-start_fulltime);
    fclose(bid_file);
    // creates file on users directory (server/users/uid/bidded)
    sprintf(path, "server/users/%s/bidded/%03d.txt", uid, auction_id);
    FILE *bidded_file = fopen(path, "w");
    if (bidded_file == NULL) {
        perror("Could not create a bid file in the user directory.");
        return 0;
    }
    fclose(bidded_file);
    return 1;
}


// creates an END file in the directory server/auctions/auction_id
int close_auction(int auction_id) {
    char path[100];
    time_t current_fulltime, start_fulltime;
    struct tm *current_datetime;
    char time_str[20];
    sprintf(path, "server/auctions/%03d/START_%03d.txt", auction_id, auction_id);
    FILE *start_file = fopen(path, "r");
    if (start_file == NULL) {
        perror("fopen error");
        return 0;
    }
    // START.txt contents: UID name asset_fname start_value timeactive start datetime (two fields) start_fulltime
    fscanf(start_file, "%*s %*s %*s %*s %*s %*s %*s %ld", &start_fulltime);
    fclose(start_file);
    time(&current_fulltime);
    sprintf(path, "server/auctions/%03d/END_%03d.txt", auction_id, auction_id);
    // creates an END.txt with contents: end_datetime end_sec_time
    FILE *end_file = fopen(path, "w");
    if (end_file == NULL) {
        perror("fopen error");
        return 0;
    }
    current_datetime = localtime(&current_fulltime);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", current_datetime);
    fprintf(end_file, "%s %ld", time_str, current_fulltime-start_fulltime);
    fclose(end_file);
    return 1;
}


void get_auc_info(int auc_id, char* status) {
    char path[100];
    char aux_buffer[100];
    char uid[7], name[11], asset_fname[25];
    char datetime1[11], datetime2[9], datetime[20];
    int start_value, bid_value;
    time_t fulltime, timeactive, bidtime, end_sec_time;
    sprintf(path, "server/auctions/%03d/START_%03d.txt", auc_id, auc_id);
    FILE *start_file = fopen(path, "r");
    if (start_file == NULL) {
        perror("fopen error");
        return;
    }
    
    // gets fields from the START.txt file
    fscanf(start_file, "%s %s %s %d %ld %s %s %ld", uid, name, asset_fname, &start_value,
    &timeactive, datetime1, datetime2, &fulltime);
    fclose(start_file);
    sprintf(datetime, "%s %s", datetime1, datetime2);
    sprintf(aux_buffer, " %s %s %s %d %s %ld", uid, name, asset_fname, start_value, datetime, timeactive);
    strcat(status, aux_buffer);
    // gets bids
    sprintf(path, "server/auctions/%03d/bids/", auc_id);
    if (access(path, F_OK) != -1) {
        DIR *dir = opendir(path);
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
                sprintf(path, "server/auctions/%03d/bids/", auc_id);
                strcat(path, entry->d_name);
                FILE *bid_file = fopen(path, "r");
                if (bid_file == NULL) {
                    perror("fopen error");
                    return;
                }
                // BID.txt content: UID bid_value bid_datetime bid_sec_time
                fscanf(bid_file, "%s %d %s %s %ld", uid, &bid_value, 
                datetime1, datetime2, &bidtime);
                fclose(bid_file);
                snprintf(datetime, sizeof(datetime), "%s %s", datetime1, datetime2);
                sprintf(aux_buffer, " B %s %d %s %ld", uid, bid_value, datetime, bidtime);
                strcat(status, aux_buffer);
            }
        }
        closedir(dir);
    }
    
    // appends info from END.txt if it exists
    // END.txt content: end_datetime end_sec_time
    // need to verify if auction is over
    if (!ongoing_auction(auc_id)) {
        sprintf(path, "server/auctions/%03d/END_%03d.txt", auc_id, auc_id);
        FILE *end_file = fopen(path, "r");
        if (end_file == NULL) {
            perror("fopen error");
            return;
        }
        fscanf(end_file, "%s %s %ld", datetime1, datetime2, &end_sec_time);
        fclose(end_file);
        sprintf(datetime, "%s %s", datetime1, datetime2);
        sprintf(aux_buffer, " E %s %ld", datetime, end_sec_time);
        strcat(status, aux_buffer);
    }
}


int lock_dir(char* path) {
    int dir_fd = open(path, O_RDONLY);
    if (dir_fd == -1) {
        perror("open error");
        return 0;
    }
    if (flock(dir_fd, LOCK_EX) == -1) {
        perror("flock error");
        return 0;
    }
    return dir_fd;
}


int unlock_dir(int dir_fd) {
    if (flock(dir_fd, LOCK_UN) == -1) {
        perror("flock error");
        return 0;
    }
    return 1;
}
