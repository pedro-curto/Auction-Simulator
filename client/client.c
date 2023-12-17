#include "client.h"


int main(int argc, char *argv[]) {
    char IP[50] = SERVER_IP;
    char port[10] = TEJO_PORT;
    char input[100], command[12];
    char uid[UID_SIZE+1], password[PASSWORD_SIZE+1];
    int user_loggedin = 0;
    struct sigaction act;
    memset(uid, 0, sizeof(uid));
    memset(password, 0, sizeof(password));
    //strcpy(uid, "000000");
    //strcpy(password, "00000000");

    // handles sigpipes
    act.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &act, NULL);


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
        printf(">> ");
        scanf("%s", command); // FIXME scanf not safe
        fgets(input, sizeof(input), stdin);

        // if (!verify_input_buffer(input, (int)sizeof(input))) {
        //     printf("Invalid input.\n");
        //     continue;
        // }

        if (input[strlen(input)-1] == '\n') input[strlen(input)-1] = '\0';
        
        if (!strcmp(command, "login")) {

            if (user_loggedin) {
                printf("You are already logged in.\n");
            } else if (login(IP, port, uid, password, input)) {
                user_loggedin = 1;
            }

        } else if (!strcmp(command, "logout")) {
            if (!user_loggedin) {
                printf("You are not logged in.\n");
            } else {
                if(logout(IP, port, uid, password)){
                    user_loggedin = 0;
                    // logged out: clears uid and password of user
                    //strcpy(uid, "000000");
                    //strcpy(password, "00000000");
                    memset(uid, 0, sizeof(uid));
                    memset(password, 0, sizeof(password));
                }
            }
        } else if (!strcmp(command, "unregister")) {

            if (!user_loggedin) {
                printf("You are not logged in.\n");
            } else {
                if (unregister(IP, port, uid, password)) { 
                user_loggedin = 0;
                // unregistered: clears uid and password of user
                memset(uid, 0, sizeof(uid));
                memset(password, 0, sizeof(password));
                } else printf("Error unregistering.\n");
            }
            
        } else if (!strcmp(command, "exit")) {

            if (user_loggedin) {
                printf("You are still logged in. Please logout first.\n");
            } else break;

        } else if (!strcmp(command, "open")) {

            if (!user_loggedin) {
                printf("You are not logged in.\n");
            } else openAuction(IP, port, uid, password, input);

        } else if (!strcmp(command, "list") || command[0] == 'l') { 

            listAllAuctions(IP, port);

        } else if (!strcmp(command, "close")) {

            if (!user_loggedin) {
                printf("You are not logged in.\n");
            } else closeAuction(IP, port, uid, password, input);

        } else if (!strcmp(command, "myauctions") || !strcmp(command, "ma")) {

            if (!user_loggedin) {
                printf("You are not logged in.\n");
            } else myAuctions(IP, port, uid);

        } else if (!strcmp(command, "mybids") || !strcmp(command, "mb")) {

            if (!user_loggedin) {
                printf("You are not logged in.\n");
            } else myBids(IP, port, uid); 

        } else if (!strcmp(command, "show_asset") || !strcmp(command, "sa")) {
            int aid;
            sscanf(input, "%d", &aid);
            showAsset(IP, port, aid);
        } else if (!strcmp(command, "bid") || command[0] == 'b') {
            int aid, value;
            sscanf(input, "%d %d", &aid, &value);

            if (!user_loggedin) {
                printf("You are not logged in.\n");
            } else bid(IP, port, uid, password, aid, value);

        } else if (!strcmp(command, "show_record") || !strcmp(command, "sr")) {
            int aid;
            sscanf(input, "%d", &aid);
            showRecord(IP, port, aid);
        
        } else {
            printf("invalid command!\n");
        }
    }
    
    return 0;
}
