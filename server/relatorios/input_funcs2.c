#include "server.h"
#define SEPARATION_CHAR ' ';
#define COMMAND_SIZE 3;
#define UID_SIZE 6;
#define PASSWORD_SIZE 8;

char* read_command_udp(char* input){
    // Read command from buffer input until SEPARATION_CHAR appears
    int i = 0;
    char command[COMMAND_SIZE+1];
    for (i = 0; input[i] != SEPARATION_CHAR; i++){
        if (i == COMMAND_SIZE + 1){
            if (input[i] != SEPARATION_CHAR){
                return NULL;
            }
            break;
        }
        command[i] = input[i];
    }

    if (strlen(command) != COMMAND_SIZE){
        return NULL;
    }

    return command;
}

char* read_uid_udp(char* input){
    int i = 0;
    char uid[UID_SIZE+1];
    int start_pos = COMMAND_SIZE + 1;
    for (i = start_pos; input[i] != SEPARATION_CHAR; i++){
        lenght = i - start_pos;
        if (lenght == UID_SIZE + 1){
            if (input[i] != SEPARATION_CHAR){
                return NULL;
            }
            break;
        }
        if (!isdigit(input[i])){
            return NULL;
        }
        uid[lenght] = input[i];
    }

    if (strlen(uid) != UID_SIZE){
        return NULL;
    }

    return uid;
}

char* read_password_udp(char* input){
    int i = 0;
    char password[PASSWORD_SIZE+1];
    int start_pos = COMMAND_SIZE + UID_SIZE + 2;
    for (i = start_pos input[i] != SEPARATION_CHAR; i++){
        lenght = i - start_pos;
        if (lenght == PASSWORD_SIZE + 1){
            if (input[i] != SEPARATION_CHAR){
                return NULL;
            }
            break;
        }
        if (!isalnum(input[i])){
            return NULL;
        }
        password[lenght] = input[i];
    }

    if (strlen(password) != PASSWORD_SIZE){
        return NULL;
    }

    return password;
}
