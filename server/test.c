// Include the header file that contains the declaration of read_field
#include "server.h"


int read_field(int tcp_socket, char *buffer, size_t size) {
    size_t bytes_read = 0;
    while (bytes_read < size) {
        ssize_t n = read(tcp_socket, buffer + bytes_read, 1); // read one byte at a time
        if (n <= 0) {
            perror("TCP read error");
            return 0;
        }
        // at any time, if we read a space we stop
        if (buffer[bytes_read] == ' ') { 
            break;
        }
        bytes_read += n;
    }
    buffer[bytes_read] = '\0'; // null-terminate the string
    return bytes_read;
}

void test_read_field() {
    int sockets[2], bytes_read = 0;
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0) {
        perror("socketpair");
        return;
    }

    char *command = "OPA 103091 11111111 A.txt 100 10 A.jpg 1000";
    //size_t num_commands = sizeof(commands) / sizeof(commands[0]);

    char buffer[20];

    while (bytes_read = read_field(sockets[0], buffer, sizeof(buffer)) > 0) {
        
        printf("Read %d bytes: %s\n", bytes_read, buffer);
    }

    /*for (size_t i = 0; i < num_commands; i++) {
        printf("Writing %s\n", commands[i]);
        if (write(sockets[0], commands[i], strlen(commands[i])) != strlen(commands[i])) {
            perror("write");
            return;
        }
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = read_field(sockets[1], buffer, sizeof(buffer));

        printf("Read %d bytes: %s\n", bytes_read, buffer);
    }*/

    close(sockets[0]);
    close(sockets[1]);
}

int main() {
    test_read_field();
    return 0;
}
