#include "server.h"


int read_command_udp(char* input, char* command) {
    // Read command from buffer input until SEPARATION_CHAR appears
    int i = 0;
    char buffer[COMMAND_SIZE+1];
    int default_size = COMMAND_SIZE + 1; //2 separation chars + \n

    if ((int)strlen(input) < default_size){
        return 0;
    }

    for (i = 0; i < default_size; i++){
        if (i == default_size - 1) {
            if (input[i] != SEPARATION_CHAR && input[i] != '\n') {
                return 0;
            }
            break;
        }
        buffer[i] = input[i];
    }

    if (strlen(buffer) != COMMAND_SIZE){
        return 0;
    }

    buffer[COMMAND_SIZE] = '\0';
    strcpy(command, buffer);
    return 1;
}


int read_uid_udp(char* input, char* uid) {
    int lenght, i = 0;
    int start_pos = COMMAND_SIZE + 1;
    char buffer[UID_SIZE+1];
    int default_size = COMMAND_SIZE + UID_SIZE + 2; //1 separation chars + \n or ' '

    if ((int)strlen(input) < default_size){
        return 0;
    }

    for (i = start_pos; i < default_size; i++) {
        lenght = i - start_pos;
        if (i == default_size - 1) {
            if (input[i] != SEPARATION_CHAR && input[i] != '\n') {
                return 0;
            }
            break;
        }
        if (!isdigit(input[i])){
            return 0;
        }
        buffer[lenght] = input[i];
    }

    if (strlen(buffer) != UID_SIZE){
        return 0;
    }

    buffer[UID_SIZE] = '\0';
    strcpy(uid, buffer);
    return 1;
}


int read_password_udp(char* input, char* password) {
    int lenght, i = 0;
    char buffer[PASSWORD_SIZE+1];
    int start_pos = COMMAND_SIZE + UID_SIZE + 2; //2 separation chars
    int default_size = COMMAND_SIZE + UID_SIZE + PASSWORD_SIZE + 3; //2 separation chars + \n
    
    if ((int)strlen(input) < default_size){
        return 0;
    }

    for (i = start_pos; i < default_size; i++) {
        lenght = i - start_pos;
        if (i == default_size - 1) {
            if (input[i] != '\n'){
                return 0;
            }
            break;
        }
        if (!isalnum(input[i])){
            return 0;
        }
        buffer[lenght] = input[i];
    }

    if (strlen(buffer) != PASSWORD_SIZE) {
        return 0;
    }

    buffer[PASSWORD_SIZE] = '\0';
    strcpy(password, buffer);
    return 1;
}

int read_aid_udp(char* input, char* aid) {
    int i, l = 0;
    char buffer[AID_SIZE+1];
    int start_pos = COMMAND_SIZE + 1;
    int max_size = COMMAND_SIZE + AID_SIZE + 2; //1 separation chars + \n or ' '
    int min_size = COMMAND_SIZE + 1 + 2; //1 separation chars

    if ((int)strlen(input) > max_size || (int)strlen(input) < min_size){
        return 0;
    }

    for (i = start_pos; i < max_size; i++) {

        if (i > start_pos) {
            if (input[i] == '\n'){
                break;
            }
        }
        if (!isdigit(input[i])){
            return 0;
        }
        buffer[l] = input[i];
        l++;
    }
    buffer[l] = '\0';
    int id = atoi(buffer);
    sprintf(aid, "%03d", id);
    return 1;
}

// FIXME first character cannot be a space, and last read character must be a space: how to ensure?
int read_field(int tcp_socket, char *buffer, size_t size) {
    size_t bytes_read = 0;
    ssize_t n;
    // check if the first character read is a space
    printf("buffer at beginning of read_field: %s\n", buffer);
    while (bytes_read <= size) {
        n = read(tcp_socket, buffer + bytes_read, 1); // read one byte at a time
        if (n <= 0) {
            perror("TCP read error");
            return 0;
        }
        bytes_read += n;
        // at any time, if we read a space or \n we stop
        if (buffer[bytes_read-1] == ' ' || buffer[bytes_read-1] == '\n') {
            break;
        }
    }
    if (buffer[0] == ' ') {
        return -1;
    }
    printf("buffer at the end of read_field: %s\n", buffer);
    // if (buffer[bytes_read-1] != '\n' && buffer[bytes_read-1] != ' '){
    //     return -1;
    // }
    buffer[bytes_read-1] = '\0';
    printf("buffer: %s\n", buffer);
    return bytes_read;
}

// FIXME tentar usar antes esta? idk
/*int read_field(int tcp_socket, char *buffer, size_t size) {
    size_t bytes_read = 0;
    ssize_t n;

    while (bytes_read <= size) {
        n = read(tcp_socket, buffer + bytes_read, 1); // read one byte at a time
        if (n <= 0) {
            perror("TCP read error");
            return 0;
        }
        bytes_read += n;

        if (bytes_read == 1) {
            if (buffer[0] == ' ') {
                return -1; 
            }
            first_char_read = 1; 
        }

        // at any time, if we read a space or \n we stop
        if (buffer[bytes_read-1] == ' ' || buffer[bytes_read-1] == '\n') {
            break; // Last character is a space, exit the loop
        }
    }

    if (buffer[bytes_read-1] != ' ') {
        return -1; // last character is not a space, return error
    }

    buffer[bytes_read-1] = '\0'; 
    return 1;
}*/


int valid_uid(char* uid) {
    int i;
    if (strlen(uid) != UID_SIZE) {
        return 0;
    }
    for (i = 0; i < UID_SIZE; i++) {
        if (!isdigit(uid[i])) {
            return 0;
        }
    }
    return 1;
}


int valid_password(char* password) {
    int i;
    if (strlen(password) != PASSWORD_SIZE) {
        return 0;
    }
    for (i = 0; i < PASSWORD_SIZE; i++) {
        if (!isalnum(password[i])) {
            return 0;
        }
    }
    return 1;
}
