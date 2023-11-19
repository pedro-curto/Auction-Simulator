#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char input[100], command[12];
    int a;

    fgets(input, 100, stdin);

    sscanf(input, "%d", &a);

    printf("%d\n", a);
    if (a >= 0 && a <= 999) {*/65
        printf("a is between 0 and 999\n");
    }
    else printf("a is not between 0 and 999\n");

    return 0;
}

/* OLD TCP:
char* connect_TCP(char* IP, char* port, char* request, char* buffer) {
    int fd, errcode;
    ssize_t n;
    //socklen_t addrlen;
    struct addrinfo hints, *res;
    //struct sockaddr_in addr;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) exit(1);
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    errcode = getaddrinfo(IP, port, &hints, &res);
    printf("IP in TCP: %s\nport in TCP: %s", IP, port);
    if (errcode != 0) perror("Error getting address info.");

    n = connect(fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) perror("Error connecting.");

    n = write(fd, request, strlen(request));
    if (n == -1) perror("Error writing.");

    n = read(fd, buffer, sizeof(buffer));
    if (n == -1) perror("Error reading.");

    freeaddrinfo(res);
    close(fd);

    return buffer;
}
*/