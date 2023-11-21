#include "user.h"

// ---------------------------- Auxiliary functions ----------------------------

char* connect_UDP(char* IP, char* port, char* request, char* buffer) {
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
    n = recvfrom(fd, buffer, 1023, 0, (struct sockaddr*) &addr, &addrlen);
    if (n == -1) perror("Error receiving response.");

    freeaddrinfo(res);
    close(fd);

    return buffer;
}

char* connect_TCP(char* IP, char* port, char* request, char* buffer, size_t buffer_size) {
    int fd, errcode;
    ssize_t n;
    //socklen_t addrlen;
    struct addrinfo hints, *res;
    //struct sockaddr_in addr;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) printf("Error creating socket.\n");
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    errcode = getaddrinfo(IP, port, &hints, &res);
    if (errcode != 0) printf("Error getting address info.\n");

    n = connect(fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) perror("Error connecting.\n");
    
    n = write(fd, request, strlen(request));
    if (n == -1) perror("Error writing.\n");
    
    n = read(fd, buffer, buffer_size - 1);
    if (n == -1) perror("Error reading.\n");

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

off_t get_file_size(char *filename) {
    struct stat st;
    // stat returns 0 on success
    if (!stat(filename, &st)) {
        printf("File size: %ld\n", st.st_size);
        return st.st_size;
    }
    printf("File size not obtained.\n");
    return -1;
}


int read_file(char *filename, char *buffer, off_t size) {
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
}