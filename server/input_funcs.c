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