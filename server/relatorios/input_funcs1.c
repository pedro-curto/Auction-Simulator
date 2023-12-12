#include "server.h"


void read_command_udp(char* input, char* command){
    // Read command from buffer input until SEPARATION_CHAR appears
    int i = 0;
    char buffer[COMMAND_SIZE+1];
    for (i = 0; (input[i] != SEPARATION_CHAR); i++){
        if (i == COMMAND_SIZE + 1){
            if (input[i] != SEPARATION_CHAR){
                return;
            }
            break;
        }
        buffer[i] = input[i];
        printf("buffer[%d]:%d\n",i, buffer[i])
    }

    if (strlen(command) != COMMAND_SIZE){
        return;
    }

    buffer[COMMAND_SIZE] = '\0';
    strcpy(command, buffer);
}

void read_uid_udp(char* input, char* uid){
    int lenght, i = 0;
    int start_pos = COMMAND_SIZE + 1;
    char buffer[UID_SIZE+1];
    for (i = start_pos; (input[i] != SEPARATION_CHAR); i++) {
        lenght = i - start_pos;
        if (lenght == UID_SIZE + 1){
            if (input[i] != SEPARATION_CHAR){
                return;
            }
            break;
        }
        if (!isdigit(input[i])) {
            return;
        }
        buffer[lenght] = input[i];
    }

    if (strlen(uid) != UID_SIZE){
        return;
    }

    buffer[UID_SIZE] = '\0';
    strcpy(uid, buffer);
}

void read_password_udp(char* input, char* password){
    int lenght, i = 0;
    char buffer[PASSWORD_SIZE+1];
    int start_pos = COMMAND_SIZE + UID_SIZE + 2;
    for (i = start_pos; (input[i] != SEPARATION_CHAR) && (input[i] != '\n'); i++) {
        printf("i:%d\n", i);
        lenght = i - start_pos;
        if (lenght == PASSWORD_SIZE + 1) {
            if (input[i] != SEPARATION_CHAR) {
                return;
            }
            break;
        }
        if (!isalnum(input[i])){
            return;
        }
        buffer[lenght] = input[i];
    }

    if (strlen(password) != PASSWORD_SIZE) {
        return;
    }

    buffer[PASSWORD_SIZE] = '\0';
    strcpy(password, buffer);

    printf("buf:%s\n", buffer);
    printf("pass:%s\n", password);

}
