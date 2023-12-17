#include "server.h"


//verbose_mode global variable
int verbose_mode = 0;

//mutex
//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


int main(int argc, char *argv[]) {
    struct sigaction act;
    int udp_socket, tcp_socket, max_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[MAX_BUFFER_SIZE];
    char *as_port;

    // sets necessary signal handlers
    act.sa_handler = SIG_IGN;
    if (sigaction(SIGCHLD, &act, NULL) == -1) {
        perror("sigaction error");
        exit(EXIT_FAILURE);
    }

    // FIXME sigpipe faz sentido no server?
    if (sigaction(SIGPIPE, &act, NULL) == -1) {
        perror("sigaction error");
        exit(EXIT_FAILURE);
    }

    // initializes the mutex
    //pthread_mutex_init(&mutex, NULL);
    

    fd_set read_fds;
    FD_ZERO(&read_fds);

    as_port = PORT;
    mkdir("server/users", 0777);
    mkdir("server/auctions", 0777);
    
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

    // sets socket option to reuse address
    int reuse = 1;
    if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("setsockopt error");
        close(udp_socket);
        close(tcp_socket);
        exit(EXIT_FAILURE);
    }

    struct timeval timeout;
    timeout.tv_sec = 5;  // 5 seconds
    timeout.tv_usec = 0;

    if (setsockopt(tcp_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        perror("Error setting receive timeout");
        close(udp_socket);
        close(tcp_socket);
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
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);

            if (recvfrom(udp_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &client_addr_len) == -1) {
                perror("UDP receive error");
            } else {
                if (verbose_mode) {
                    print_verbose_info(client_addr, "UDP");
                }

                // // Create a new thread for processing UDP request
                // pthread_t udp_thread;
                // struct {
                //     int udp_socket;
                //     struct sockaddr_in client_addr;
                //     char buffer[MAX_BUFFER_SIZE];
                //     socklen_t client_addr_len;
                // }* thread_arg = malloc(sizeof(*thread_arg));

                // thread_arg->udp_socket = udp_socket;
                // thread_arg->client_addr = client_addr;
                // thread_arg->client_addr_len = client_addr_len;
                // memcpy(thread_arg->buffer, buffer, sizeof(buffer));

                // pthread_create(&udp_thread, NULL, process_udp_thread, thread_arg);

                process_udp_request(udp_socket, client_addr, buffer, client_addr_len);
                memset(buffer, 0, sizeof(buffer));
            }
        }

        if (FD_ISSET(tcp_socket, &read_fds)) {
            int client_socket = accept(tcp_socket, (struct sockaddr*)&client_addr, &client_addr_len);
            if (client_socket == -1) {
                perror("TCP accept error");
            } else {
                if (verbose_mode) {
                    print_verbose_info(client_addr, "TCP");
                }
                pid_t pid;
                if ((pid = fork()) == -1) {
                    perror("fork error");
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {
                    close(tcp_socket);
                    int auc_fd = lock_dir("server/auctions");
                    int users_fd = lock_dir("server/users");
                    //pthread_mutex_lock(&mutex);
                    process_tcp_request(client_socket);
                    //pthread_mutex_unlock(&mutex);
                    unlock_dir(auc_fd);
                    unlock_dir(users_fd);
                    // FIXME temos de fechar o client_socket aqui? 
                    exit(EXIT_SUCCESS);
                }

                // int ret;
                // do ret = close(client_socket); while (ret==-1 && errno == EINTR);
                // if (ret == -1) {
                //     perror("close error");
                //     exit(EXIT_FAILURE);
                // }

                close(client_socket);
                // // Create a new thread for processing TCP request
                // pthread_t tcp_thread;
                // int* thread_arg = malloc(sizeof(int));
                // *thread_arg = client_socket;

                // pthread_create(&tcp_thread, NULL, process_tcp_thread, thread_arg);
            }
        }
    }
    //pthread_mutex_destroy(&mutex);
    // Cleanup (not reached in this example)
    close(udp_socket);
    close(tcp_socket);

    return 0;
}

void process_udp_request(int udp_socket, struct sockaddr_in client_addr, char *buffer, socklen_t client_addr_len) {
    char command[COMMAND_SIZE+1];
    memset(command, 0, sizeof(command));
    // FIXME problem here?
    int command_info = read_command_udp(buffer, command);

    if (command_info == 0){
        reply_msg(udp_socket, client_addr, client_addr_len, "ERR\n");
        printf("Invalid command.\n");
        return;
    } else if (command_info == 2 && strcmp(command, "LST") != 0){
        printf("%s\n", command);
        reply_msg(udp_socket, client_addr, client_addr_len, "ERR\n");
        printf("Invalid command.\n");
        return;
    }

    if (!strcmp(command,"LIN")) {
        handle_login(udp_socket, client_addr, buffer, client_addr_len);
    } else if (!strcmp(command,"LOU")) {
        handle_logout(udp_socket, client_addr, buffer, client_addr_len);
    } else if (!strcmp(command,"UNR")) {
        handle_unregister(udp_socket, client_addr, buffer, client_addr_len);
    } else if (!strcmp(command, "LMA")) {
        handle_myauctions(udp_socket, client_addr, buffer, client_addr_len);
    } else if (!strcmp(command,"LMB")) {
        handle_mybids(udp_socket, client_addr, buffer, client_addr_len);
    } else if (!strcmp(command,"LST")) {
        handle_list(udp_socket, client_addr, buffer, client_addr_len);
    } else if (!strcmp(command,"SRC")) {
        handle_show_record(udp_socket, client_addr, buffer, client_addr_len);
    } else {
        reply_msg(udp_socket, client_addr, client_addr_len, "ERR\n");
        printf("Invalid command.\n");
    }
}


void process_tcp_request(int tcp_socket) {
    char command[5];
    memset(command, 0, sizeof(command));
    read_field(tcp_socket, command, 4); 

    printf("command: %s\n", command);
    if (!strncmp(command, "OPA", 3)) {
        handle_open(tcp_socket);
    } else if (!strcmp(command, "CLS")) {
        handle_close(tcp_socket);
    } else if (!strcmp(command, "SAS")){
        handle_show_asset(tcp_socket);
    } else if (!strcmp(command, "BID")) {
        handle_bid(tcp_socket);
    } else {
        write_tcp(tcp_socket, "ERR\n");
        printf("Invalid command.\n");
    }

    
}

void print_verbose_info(struct sockaddr_in client_addr, const char *protocol) {
    printf("Received %s request from %s:%d\n", protocol, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
}