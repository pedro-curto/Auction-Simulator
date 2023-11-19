#include "user.h"


int main(int argc, char *argv[]) {
    char IP[50] = SERVER_IP;
    char port[10] = PORT;
    char input[100], command[12];
    char uid[UID_SIZE+1], password[PASSWORD_SIZE+1];
    int user_loggedin = 0;
    
    switch(argc) {
        case 1:
            break;
        case 3: 
            if(!strcmp(argv[1], "-n")) {
                strcpy(IP, argv[2]);
            } else if(!strcmp(argv[1], "-p")) {
                strcpy(port, argv[2]);
            } else {
                printf("Invalid arguments.\n");
                return -1;  
            }
            break;
        case 5:
            if(!strcmp(argv[1],"-n") && !strcmp(argv[3],"-p")) {
                strcpy(IP, argv[2]);
                strcpy(port, argv[4]);
            } else {
                printf("Invalid arguments.\n");
                return -1;
            }
            break;
        default: 
            printf("Invalid arguments.\n");
            return -1;
    }

    while (1) {
        // gets stdin into input buffer
        scanf("%s", command);
        // printf("command: %s\n", command);
        fgets(input, sizeof(input), stdin);
        if (input[strlen(input)-1] == '\n') input[strlen(input)-1] = '\0';
        // printf("input:%s\n", input);
        // if (fgets(input, sizeof(input), stdin) == NULL) {
        //     perror("Error reading input");
        //     return -1;
        // }
        // sscanf(input, "%11s", command);
        if (!user_loggedin){
            if (!strcmp(command, "login")) {
                if (login(IP, port, uid, password, input)){
                    user_loggedin = 1;
                }
            } else if(!strcmp(command, "exit")) {
                break;
            } else printf("You are not logged in.\n");

        } else if (!strcmp(command, "logout")) {
            if(logout(IP, port, uid, password)){
                user_loggedin = 0;
                // logged out: clears uid and password of user
                strcpy(uid, "");
                strcpy(password, "");
            }
        } else if (!strcmp(command, "unregister")) {
            if (unregister(IP, port, uid, password)) { 
                user_loggedin = 0;
                // unregistered: clears uid and password of user
                strcpy(uid, "");
                strcpy(password, "");
            } else printf("Error unregistering.\n");
            
        } else if (!strcmp(command, "exit")) {
            printf("You are still logged in. Please logout first.\n");

        } else if (!strcmp(command, "open")) {
            openAuction(IP, port, uid, password, input);
        
        } else if (!strcmp(command, "list") || command[0] == 'l') { 
            listAllAuctions(IP, port);
        } else if (!strcmp(command, "close")) {
            
            closeAuction(IP, port, uid, password, input);
        
        /*
        } else if (!strcmp(command, "myauctions") || !strcmp(command, "ma")) {
            myAuctions(IP, port, uid, password);*/

        /*
        
        } /else if (!strcmp(command, "mybids") || !strcmp(command, "mb")) {
            my_bids(IP, port); */

        /*
        } else if (!strcmp(command, "show_asset") || !strcmp(command, "sa")) {
            show_asset(IP, port);
        } else if (!strcmp(command, "bid") || command == 'b'){
            bid(IP, port);
        } else if (!strcmp(command, "show_record") || !strcmp(command, "sr")) {
            show_record(IP, port);
        */
        } else {
            printf("invalid command!\n");
        }
    }
    
    return 0;
}