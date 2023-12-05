#include "client.h"

int getFileSize1(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file.\n");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fclose(file);

    return size;
}

int getFileSize2(char *filename) {
    struct stat st;
    int ret_stat;
    ret_stat = stat(filename, &st);
    if (ret_stat == -1 || st.st_size == 0) return -1;
    return st.st_size;
}


int main() {
    int size1 = getFileSize1("thescream.jpg");
    int size2 = getFileSize2("thescream.jpg");

    printf("size1: %d\nsize2: %d\n", size1, size2);


    return 0;
}


void connect_TCP(char* IP, char* port, char* request, char* buffer, size_t buffer_size) {
    int fd, asset_fd, errcode, fsize, n;
    char asset_fname[ASSET_FNAME_SIZE + 1];//, reqCode[4];
    struct addrinfo hints, *res;
    //socklen_t addrlen;
    //struct sockaddr_in addr;
    if (!strncmp(request, "OPA", 3)) { 
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
    printf("request: %s\n", request);
    //n = read(fd, buffer, buffer_size - 1);
    if (!strncmp(request, "OPA", 3)) {
        // uses sendfile() to send the image
        if (n == -1) perror("Error reading.\n");
        asset_fd = open(asset_fname, O_RDONLY);
        if (asset_fd == -1) perror("Error opening file.\n");
        off_t offset = 0;
        printf("check1\n");

        ssize_t sent_bytes = sendfile(fd, asset_fd, 0, fsize);
        printf("check2\n");
        printf("sent_bytes: %ld\n", sent_bytes);
        close(asset_fd);
        if (sent_bytes == -1) perror("Error sending file.\n");
        if (write(fd, "\n", 1) == -1) perror("Error writing \\n character.\n");
        
    }
    // reads all but Fdata
    printf("size n: %ld\n", n);
    printf("buffer: %sEND\n", buffer);
    buffer[n] = '\0';
    freeaddrinfo(res);
    close(fd);
    printf("LEAVING TCP\n");
}