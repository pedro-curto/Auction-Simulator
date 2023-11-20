#include "server.h"

/*int GetBidList(int AID, BIDLIST *list) {
    struct dirent **filelist;
    int n_entries, n_bids, len;
    char dirname[20];
    char pathname[32];

    sprintf(dirname, "AUCTIONS/%03d/BIDS/", AID);
    n_entries = scandir(dirname, &filelist, 0, alphasort);
    if (n_entries <= 0) // Could test for -1 since n_entries count always with . and ..
        return 0;

    n_bids = 0;
    list->no_bids = 0;
    while (n_entries--) {
        len = strlen(filelist[n_entries]->d_name);
        if (len == 10) // Discard '.', '..' and invalid filenames by size
            {
                sprintf(pathname, "AUCTIONS/%03d/BIDS/%s", AID, filelist[n_entries]->d_name);
            }
    }
}*/

void UDPServer() {
    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) perror("Error creating socket.");

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    hints.ai_flags = AI_PASSIVE; // Use my IP
    errcode = getaddrinfo(NULL, PORT, &hints, &res);
    if (errcode != 0) perror("Error getting address info.");
    n = bind(fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) perror("Error binding.");

    while (1) {
        addrlen = sizeof(addr);
        n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*) &addr, &addrlen);
        if (n == -1) perror("Error receiving request.");
        write(1, "Received: ", 10);
        write(1, buffer, n);
        n = sendto(fd, buffer, n, 0, (struct sockaddr*) &addr, addrlen);
        if (n == -1) perror("Error sending response.");

        freeaddrinfo(res);
        close(fd);
    }
}

void TCPServer() {
    int fd, newfd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];

    if (socket(AF_INET, SOCK_STREAM, 0) == -1) perror("Error creating socket.");

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    hints.ai_flags = AI_PASSIVE; // Use my IP

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) perror("Error getting address info.");
    if (bind(fd, res->ai_addr, res->ai_addrlen) == -1) perror("Error binding.");
    if (listen(fd, 5) == -1) perror("Error listening.");

    while (1) {
        addrlen = sizeof(addr);
        if ((newfd = accept(fd, (struct sockaddr*) &addr, &addrlen)) == -1) perror("Error accepting connection.");
        if (read(newfd, buffer, 128) == -1) perror("Error reading.");
        write(1, "Received: ", 10);
        write(1, buffer, n);
        if (write(newfd, buffer, n) == -1) perror("Error writing.");
        close(newfd);

    }
    freeaddrinfo(res);
    close(fd);
}

void *GetInAddr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

