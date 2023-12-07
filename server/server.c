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

int main(int argc, char *argv[]) {
    int udp_socket, tcp_socket, max_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[MAX_BUFFER_SIZE];
    char *as_port;

    fd_set read_fds;
    FD_ZERO(&read_fds);

    as_port = PORT;
    int verbose_mode = 0;
    
    switch(argc) {
        case 1:
            break;
        case 2:
            if(!strcmp(argv[1], "-v")) {
                verbose_mode = 1;
            } else {
                printf("Invalid arguments.\n");
                return -1;
            }
            break;
        case 3: 
            if(!strcmp(argv[1], "-p")) {
                strcpy(as_port, argv[2]);
            } else {
                printf("Invalid arguments.\n");
                return -1;
            }
            break;
        case 4:
            if(!strcmp(argv[1],"-p") && !strcmp(argv[3],"-v")) {
                strcpy(as_port, argv[2]);
                verbose_mode = 1;
            } else {
                printf("Invalid arguments.\n");
                return -1;
            }
            break;
        default: 
            printf("Invalid arguments.\n");
            return -1;
    }
    // Create UDP socket
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket == -1) {
        perror("UDP socket creation error");
        exit(EXIT_FAILURE);
    }

    // Create TCP socket
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket == -1) {
        perror("TCP socket creation error");
        close(udp_socket);
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(as_port));

    // Bind UDP socket
    if (bind(udp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("UDP socket bind error");
        close(udp_socket);
        close(tcp_socket);
        exit(EXIT_FAILURE);
    }

    // Bind TCP socket
    if (bind(tcp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("TCP socket bind error");
        close(udp_socket);
        close(tcp_socket);
        exit(EXIT_FAILURE);
    }

    // Listen on TCP socket
    if (listen(tcp_socket, 5) == -1) {
        perror("TCP socket listen error");
        close(udp_socket);
        close(tcp_socket);
        exit(EXIT_FAILURE);
    }

    printf("Auction Server is listening on port %s\n", as_port);

    max_socket = (udp_socket > tcp_socket) ? udp_socket : tcp_socket;

    while (1) {
        FD_SET(udp_socket, &read_fds);
        FD_SET(tcp_socket, &read_fds);

        if (select(max_socket + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("Select error");
            break;
        }

        if (FD_ISSET(udp_socket, &read_fds)) {
            if (recvfrom(udp_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_addr_len) == -1) {
                perror("UDP receive error");
            } else {
                if (verbose_mode) {
                    print_verbose_info(client_addr, "UDP");
                }
                process_udp_request(udp_socket, client_addr, buffer, client_addr_len);
            }
        }

        if (FD_ISSET(tcp_socket, &read_fds)) {
            int client_socket = accept(tcp_socket, (struct sockaddr *)&client_addr, &client_addr_len);
            if (client_socket == -1) {
                perror("TCP accept error");
            } else {
                if (verbose_mode) {
                    print_verbose_info(client_addr, "TCP");
                }
                process_tcp_request(client_socket, buffer);
                /*if (recv(client_socket, buffer, sizeof(buffer), 0) == -1) {
                    perror("TCP receive error");
                } else {
                    process_tcp_request(client_socket, buffer);
                }*/

                close(client_socket);
            }
        }
    }

    // Cleanup (not reached in this example)
    close(udp_socket);
    close(tcp_socket);

    return 0;
}


void process_udp_request(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len) {
    char command[10];
    sscanf(buffer, "%s",command);

    if (!strcmp(command,"LIN")) {
        handle_login(udp_socket, client_addr, buffer, client_addr_len);
    } else if (!strcmp(command,"LOU")){
        handle_logout(udp_socket, client_addr, buffer, client_addr_len);
    } else if (!strcmp(command,"UNR")){
        handle_unregister(udp_socket, client_addr, buffer, client_addr_len);
    } else if (!strcmp(command,"LMA")){
        handle_myauctions(udp_socket, client_addr, buffer, client_addr_len);
    // } else if (!strcmp(command,"LMB")){
    //     handle_mybids(udp_socket, client_addr, buffer, client_addr_len);
    // } else if (!strcmp(command,"LST")){11111111
    //     handle_list(udp_socket, client_addr, buffer, client_addr_len);
    // } else if (!strcmp(command,"SRC")){
    //     handle_show_record(udp_socket, client_addr, buffer, client_addr_len);
    } else {
        printf("Invalid command.\n");
    }


}

void process_tcp_request(int tcp_socket, char *buffer) {
    char command[4];
    int bytes_read = 0;
    while (bytes_read < 3) {
        bytes_read += read(tcp_socket, buffer + bytes_read, 3 - bytes_read);
    }
    command[strlen(command)] = '\0';
    printf("Command: %s\n", command);

    if (!strcmp(command,"OPA")) {
        handle_open(tcp_socket, buffer);
    } /*else if (!strcmp(command,"CLS")){
        handle_close(tcp_socket, buffer);
    } else if (!strcmp(command,"SAS")){
        handle_show_asset(tcp_socket, buffer);
    } else if (!strcmp(command,"BID")){
        handle_bid(tcp_socket, buffer);
    }*/ else {
        printf("Invalid command.\n");
    }

    // Implement TCP request processing here
}

void print_verbose_info(struct sockaddr_in client_addr, const char *protocol) {
    printf("Received %s request from %s:%d\n", protocol, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
}



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
    int fd, newfd;
    ssize_t n = 10;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) perror("Error creating socket.\n");

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


