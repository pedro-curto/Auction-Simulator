#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    while (1) {
        int i = 0;
        char c, input[100], command[12], username[100], password[100];
        //printf("size of input: %lu\n", sizeof(input));
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        if (input[strlen(input)-1] == '\n') input[strlen(input)-1] = '\0';
        if (!strcmp(input, "exit")) break;
        sscanf(input, "%s", command);

        sscanf(input, "%*s %s %s", username, password);

        printf("input: %s\n", input);   
        printf("command: %s\n", command);
        printf("username: %s\n", username);
        printf("password: %s\n", password);


    }

    return 0;
}