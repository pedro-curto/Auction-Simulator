#include "client.h"

// ---------------------------- Auxiliary functions ----------------------------

char* connect_UDP(char* IP, char* port, char* request, char* buffer) { // FIXME incluir um campo buffer_size?
    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    // TODO devemos permitir que a função continue se qualquer um dos perrors ocorrer?
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) perror("Error creating socket.");
    
    memset(&hints, 0, sizeof hints);
    memset(&addr, 0, sizeof addr);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

    errcode = getaddrinfo(IP, port, &hints, &res);
    if (errcode != 0) perror("Error getting address info."); 
    n = sendto(fd, request, strlen(request), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) perror("Error sending request.");

    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, MA_BUFFER_SIZE, 0, (struct sockaddr*) &addr, &addrlen); // usar sempre este buffer size? talvez só no list
    if (n == -1) perror("Error receiving response.");
    buffer[n] = '\0';
    freeaddrinfo(res);
    close(fd);

    return buffer;
}

char* connect_TCP(char* IP, char* port, char* request, char* buffer, size_t buffer_size) {
    int fd, asset_fd, errcode, openAuc = 0, fsize;
    char asset_fname[ASSET_FNAME_SIZE + 1];
    ssize_t n;
    struct addrinfo hints, *res;
    //socklen_t addrlen;
    //struct sockaddr_in addr;
    // OPA UID password name start_value timeactive Fname Fsize Fdata OPA %s %s %s %d %d %s %d\n
    if (!strncmp(request, "OPA", 3)) { 
        openAuc = 1;
        sscanf(request, "OPA %*s %*s %*s %*d %*d %s %d", asset_fname, &fsize);
        //printf("connect_TCP asset_fname: %s, fsize: %d\n", asset_fname, fsize);
    }
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) perror("Error creating socket.\n");
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    if (getaddrinfo(IP, port, &hints, &res) != 0) printf("Error getting address info.\n");
    if (connect(fd, res->ai_addr, res->ai_addrlen) == -1) printf("Error connecting.\n");
    // writes the header in open
    if (write(fd, request, strlen(request)) == -1) perror("Error writing.\n");
    if (openAuc) { 
        // uses sendfile() to send the image
        asset_fd = open(asset_fname, O_RDONLY);
        if (asset_fd == -1) perror("Error opening file.\n");
        off_t offset = 0;
        ssize_t sent_bytes = sendfile(fd, asset_fd, &offset, fsize);
        if (sent_bytes == -1) perror("Error sending file.\n");
    }
    if (read(fd, buffer, buffer_size - 1) == -1) perror("Error reading.\n");

    freeaddrinfo(res);
    close(fd);
    return buffer;
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


/*int readFile(char *filename, char *buffer, int size) {
    //size_t bufferSize = 100;
    int fd = open(filename, O_RDONLY);
    if (fd == -1) { 
        perror("Error opening file");
        return -1;
    }
    ssize_t n = read(fd, buffer, size);
    close(fd);
    if (n == -1) {
        perror("Error reading file");
        return -1;
    }
    return 0;
}*/
