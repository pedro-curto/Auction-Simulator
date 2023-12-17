#include "server.h"


int read_command_udp(char* input, char* command){
    // Read command from buffer input until SEPARATION_CHAR appears
    int i = 0;
    char buffer[COMMAND_SIZE+1];
    int default_size = COMMAND_SIZE + 1; //2 separation chars + \n

    if ((int)strlen(input) < default_size){
        return 0;
    }

    for (i = 0; i < default_size; i++){
        if (i == default_size - 1) {
            if (input[i] == SEPARATION_CHAR){
                buffer[COMMAND_SIZE] = '\0';
                strcpy(command, buffer);
                return 1;
            } else if (input[i] == '\n'){
                buffer[COMMAND_SIZE] = '\0';
                strcpy(command, buffer);
                return 2;
            }
            return 0;
        }
        buffer[i] = input[i];
    }
    return 0;
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
            if (input[i] == SEPARATION_CHAR){
                buffer[UID_SIZE] = '\0';
                strcpy(uid, buffer);
                return 1;
            }
            return 0;
        }
        if (!isdigit(input[i])){
            return 0;
        }
        buffer[lenght] = input[i];
    }
    return 0;
}

int read_uid_udp_final(char* input, char* uid) {
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
            if (input[i] == '\n'){
                buffer[UID_SIZE] = '\0';
                strcpy(uid, buffer);
                return 1;
            }
            return 0;
        }
        if (!isdigit(input[i])){
            return 0;
        }
        buffer[lenght] = input[i];
    }
    return 0;
}

int read_password_udp(char* input, char* password){
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
            if (input[i] == '\n'){
                buffer[PASSWORD_SIZE] = '\0';
                strcpy(password, buffer);
                return 1;
            }
            return 0;
        }
        if (!isalnum(input[i])){
            return 0;
        }
        buffer[lenght] = input[i];
    }
    return 0;
}


int read_aid_udp(char* input, char* aid){
    int i, id , l = 0;
    char buffer[AID_SIZE+1];
    int start_pos = COMMAND_SIZE + 1;
    int max_size = COMMAND_SIZE + AID_SIZE + 2; //1 separation chars + \n or ' '
    int min_size = COMMAND_SIZE + 1 + 2; //1 separation chars

    if ((int)strlen(input) > max_size || (int)strlen(input) < min_size){
        return 0;
    }

    for (i = start_pos; i < max_size; i++) {

        if (input[i] == '\n'){
            if (i > start_pos) {
                buffer[l] = '\0';
                id = atoi(buffer);
                sprintf(aid, "%03d", id);
                return 1;
            }
            return 0;
        }
        if (!isdigit(input[i])){
            return 0;
        }
        buffer[l] = input[i];
        l++;
    }
    return 0;
}

int verify_uid(char* uid){
    int i;
    if (strlen(uid) != UID_SIZE){
        return 0;
    }
    for (i = 0; i < (int)strlen(uid); i++){
        if (!isdigit(uid[i])){
            return 0;
        }
    }
    return 1;
}

int verify_password(char* password){
    int i;
    if (strlen(password) != PASSWORD_SIZE){
        return 0;
    }
    for (i = 0; i < (int)strlen(password); i++){
        if (!isalnum(password[i])){
            return 0;
        }
    }
    return 1;
}

int verify_aid(char* aid){
    int i;
    
    for (i = 0; i < (int)strlen(aid); i++){
        if (!isdigit(aid[i])){
            return 0;
        }
    }
    if (atoi(aid) < 0 || atoi(aid) > 999){
        return 0;
    }
    return 1;
}

int verify_name(char* name){
    int i;
    if (strlen(name) > NAME_SIZE || strlen(name) < 1){
        return 0;
    }
    for (i = 0; i < (int)strlen(name); i++){
        if (!isalnum(name[i])){
            return 0;
        }
    }
    return 1;
}

int verify_start_value(char* start_value){
    int i;
    if (strlen(start_value) > VALUE_SIZE || strlen(start_value) < 1){
        return 0;
    }
    for (i = 0; i < (int)strlen(start_value); i++){
        if (!isdigit(start_value[i])){
            return 0;
        }
    }
    return 1;
}

int verify_timeactive(char* timeactive){
    int i;
    if (strlen(timeactive) > TIMEACTIVE_SIZE || strlen(timeactive) < 1){
        return 0;
    }
    for (i = 0; i < (int)strlen(timeactive); i++){
        if (!isdigit(timeactive[i])){
            return 0;
        }
    }
    return 1;
}

int verify_asset_fname(char* asset_fname){
    int i;
    if (strlen(asset_fname) > ASSET_FNAME_SIZE || strlen(asset_fname) < 1){
        return 0;
    }
    for (i = 0; i < (int)strlen(asset_fname); i++){
        if (!isalnum(asset_fname[i]) && asset_fname[i] != '.' && asset_fname[i] != '_' && asset_fname[i] != '-'){
            return 0;
        }
    }
    return 1;
}

int verify_asset_fsize(char* asset_fsize){
    int i;
    if (strlen(asset_fsize) > ASSET_FSIZE_SIZE || strlen(asset_fsize) < 1){
        return 0;
    }
    for (i = 0; i < (int)strlen(asset_fsize); i++){
        if (!isdigit(asset_fsize[i])){
            return 0;
        }
    }
    if (atoi(asset_fsize) < 0 || atoi(asset_fsize) > MAX_ASSET_FSIZE){
        return 0;
    }
    return 1;
}


int verify_bid_value(char* bid_value){
    int i;
    if (strlen(bid_value) > VALUE_SIZE || strlen(bid_value) < 1){
        return 0;
    }
    for (i = 0; i < (int)strlen(bid_value); i++){
        if (!isdigit(bid_value[i])){
            return 0;
        }
    }
    return 1;
}

// int verify_bid_datetime(char* bid_datetime){
//     int i;
//     if (strlen(bid_datetime) != DATETIME_SIZE){
//         return 0;
//     }
//     for (i = 0; i < (int)strlen(bid_datetime); i++){
//         if (!isdigit(bid_datetime[i])){
//             return 0;
//         }
//     }
//     return 1;
// }

