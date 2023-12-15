#include "client.h"

// ---------------------------- Auxiliary functions ----------------------------

void connect_UDP(char* IP, char* port, char* request, char* buffer) { // FIXME incluir um campo buffer_size? 
    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    //printf("connect_UDP request: %s\n", request);
    // TODO devemos permitir que a função continue se qualquer um dos perrors ocorrer?
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) perror("Error creating socket.");

    struct timeval timeout;
    timeout.tv_sec = 5;  // 5 seconds
    timeout.tv_usec = 0;

    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        perror("Error setting receive timeout");
        close(fd);
        return;
    }

    memset(&hints, 0, sizeof hints);
    memset(&addr, 0, sizeof addr);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    errcode = getaddrinfo(IP, port, &hints, &res);
    if (errcode != 0) perror("Error getting address info."); 
    n = sendto(fd, request, strlen(request), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) perror("Error sending request.");
    addrlen = sizeof(addr);
    // usar sempre este buffer size? talvez só no list
    n = recvfrom(fd, buffer, 8095, 0, (struct sockaddr*) &addr, &addrlen);
    if (n == -1) perror("Error receiving response.");
    buffer[n] = '\0';
    freeaddrinfo(res);
    close(fd);
}


void connect_TCP(char* IP, char* port, char* request, char* buffer, size_t buffer_size) {
    int fd, asset_fd, fsize, to_read;//,  errcode;
    char asset_fname[ASSET_FNAME_SIZE + 1], path[50];
    ssize_t n;
    struct addrinfo hints, *res;
    //socklen_t addrlen;
    //struct sockaddr_in addr;
    // OPA UID password name start_value timeactive Fname Fsize Fdata OPA %s %s %s %d %d %s %d\n
    /*if (!strncmp(request, "OPA", 3)) { 
        openAuc = 1;
        sscanf(request, "OPA %*s %*s %*s %*d %*d %s %d", asset_fname, &fsize);
        //printf("connect_TCP asset_fname: %s, fsize: %d\n", asset_fname, fsize);
    }*/
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) perror("Error creating socket.\n");

    struct timeval timeout;
    timeout.tv_sec = 5;  // 5 seconds
    timeout.tv_usec = 0;

    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        perror("Error setting receive timeout");
        close(fd);
        return;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    if (getaddrinfo(IP, port, &hints, &res) != 0) printf("Error getting address info.\n");
    if (connect(fd, res->ai_addr, res->ai_addrlen) == -1) printf("Error connecting.\n");
    // writes the header in open
    write_tcp(fd, request);

    if (!strncmp(request, "OPA", 3)) {
        sscanf(request, "OPA %*s %*s %*s %*d %*d %s %d", asset_fname, &fsize); 
        // uses sendfile() to send the image
        sprintf(path, "local_assets/%s", asset_fname);
        asset_fd = open(path, O_RDONLY);
        printf("path in tcp_connect: %s\n", path);
        if (asset_fd == -1) perror("Error opening file.\n");
        off_t offset = 0;
        ssize_t sent_bytes = sendfile(fd, asset_fd, &offset, fsize);
        if (sent_bytes == -1) perror("Error sending file.\n");
        if (write(fd, "\n", 1) == -1) perror("Error writing.\n");
        close(asset_fd);
    } else if (!strncmp(request, "SAS", 3)) {
        // reads the first four bytes, guaranteed to be SAS and a space
        to_read = 4;
        while (to_read > 0) {
            n = read(fd, buffer, 1);
            if (n == -1) perror("Error reading.\n");
            to_read -= n;
        }

    }
    n = read(fd, buffer, buffer_size - 1);
    if (n == -1) perror("Error reading.\n");
    buffer[n] = '\0';
    freeaddrinfo(res);
    close(fd);
    //return buffer;
}


int valid_filename(char *filename) {
    if (strlen(filename) > ASSET_FNAME_SIZE) return -1;
    if (strspn(filename, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.")) return -1;
    char *extension = strrchr(filename, '.');
    if (!extension || strlen(extension) != 4) return -1;
    if (!isalpha(extension[1]) || !isalpha(extension[2]) || !isalpha(extension[3])) return -1;
    return 1;
}

int getFileSize(char *filename) {
    struct stat st;
    int ret_stat;
    ret_stat = stat(filename, &st);
    if (ret_stat == -1 || st.st_size == 0) return -1;
    return st.st_size;
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
        // at any time, if we read a space we stop
        if (buffer[bytes_read-1] == ' ') { // 103091 11111111 abcdefgh
            break;
        } 
    }
    buffer[bytes_read-1] = '\0';
    return bytes_read;
}


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
        perror("TCP read1 error");
        return 0;
    }

    return bytes_read;
}


int connect_tcp(char* IP, char* port) {
    int fd;
    struct addrinfo hints, *res;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("Error creating socket.\n");
        exit(EXIT_FAILURE);
    }

    struct timeval timeout;
    timeout.tv_sec = 5;  // 5 seconds
    timeout.tv_usec = 0;

    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        perror("Error setting receive timeout");
        close(fd);
        return 0;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;          // IPv4
    hints.ai_socktype = SOCK_STREAM;    // TCP socket

    if (getaddrinfo(IP, port, &hints, &res) != 0) {
        perror("Error getting address info.\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (connect(fd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("Error connecting.\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(res);
    return fd;
}

int read_buffer_token(char* buffer, char* token, ssize_t token_size, int start_pos){
    int i,j;
    int size = (int)token_size -1;
    for (i = start_pos, j = 0; buffer[i] != ' ' && j < size && i > -1; i++,j++){
        if (buffer[i] == '\n'){
            token[j] = '\0';
            return -1;
        }
        token[j] = buffer[i];
    }
    token[j] = '\0';
    if (i == 1024) return -1;
    return i;
}

void write_tcp(int tcp_socket, char* status) {
    size_t n = 0;
    while (n < strlen(status)) {
        n += write(tcp_socket, status + n, strlen(status) - n);
    }
}