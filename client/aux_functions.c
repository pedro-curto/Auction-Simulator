#include "client.h"

// ---------------------------- Auxiliary functions ----------------------------

void connect_UDP(char* IP, char* port, char* request, char* buffer) { // FIXME incluir um campo buffer_size? 
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
    // usar sempre este buffer size? talvez só no list
    n = recvfrom(fd, buffer, 4095, 0, (struct sockaddr*) &addr, &addrlen);
    if (n == -1) perror("Error receiving response.");
    buffer[n] = '\0';
    freeaddrinfo(res);
    close(fd);
}

/*void connect_TCP(char* IP, char* port, char* request, char* buffer, size_t buffer_size) {
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
    n = read(fd, buffer, buffer_size - 1);
    if (!strncmp(request, "OPA", 3)) {
        char chunk
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
}*/

void connect_TCP(char* IP, char* port, char* request, char* buffer, size_t buffer_size) {
    int fd, asset_fd, errcode, openAuc = 0, fsize;
    char asset_fname[ASSET_FNAME_SIZE + 1];
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
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    if (getaddrinfo(IP, port, &hints, &res) != 0) printf("Error getting address info.\n");
    if (connect(fd, res->ai_addr, res->ai_addrlen) == -1) printf("Error connecting.\n");
    // writes the header in open
    if (write(fd, request, strlen(request)) == -1) perror("Error writing.\n");
    if (!strncmp(request, "OPA", 3)) {
        sscanf(request, "OPA %*s %*s %*s %*d %*d %s %d", asset_fname, &fsize); 
        // uses sendfile() to send the image
        asset_fd = open(asset_fname, O_RDONLY);
        if (asset_fd == -1) perror("Error opening file.\n");
        off_t offset = 0;
        ssize_t sent_bytes = sendfile(fd, asset_fd, &offset, fsize);
        if (sent_bytes == -1) perror("Error sending file.\n");
        close(asset_fd);
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


/*else if (!strncmp(request, "SAS", 3)) { // SAS AID\n
        // RSA OK Fname------------------- Fsize--- 0\n
        //printf("ENTERED\n");
        int bytes_read = 0;
        //printf("BUFFER SIZE: %ld\n", buffer_size);
        n = read(fd, buffer, buffer_size - 1);
        if (n == -1) perror("Error reading.\n");
        bytes_read += n;
        printf("bytes read: %ld\n", n);
        buffer[n] = '\0';
        printf("buffer: %s\n", buffer);
        //bytes_read += n;
        if (!strncmp(buffer, "RSA OK", 6)) {
            // RSA OK Fname------------------- Fsize--- 0
            while (bytes_read < SA_RESPONSE_HEADER) { // SA_RESPONSE_HEADER <- 41
                n = read(fd, buffer, buffer_size - 1);
                if (n == -1) perror("Error reading.\n");
                bytes_read += n;
                printf("buffer: %10s\n", buffer);
                printf("bytes_read: %d\n", bytes_read);
            }
            char chunk[512];
            ssize_t received_bytes;
            //printf("buffer: %s", buffer);
            sscanf(buffer, "RSA OK %24s %8d", asset_fname, &fsize);
            printf("asset_fname: %s\n", asset_fname);
            asset_fd = open(asset_fname, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            printf("asset_fd: %d\n", asset_fd);
            while (fsize > 0) {
                received_bytes = read(fd, chunk, sizeof(chunk));
                if (received_bytes <= 0) {
                    perror("Error while reading from socket.\n");
                    break;
                }
                
                printf("chunk size: %ld\n", sizeof(chunk));
                if (write(asset_fd, chunk, received_bytes) == -1) {
                    perror("Error while writing to file.\n");
                    break;
                }
                fsize -= received_bytes;
            }
        }

    }
*/