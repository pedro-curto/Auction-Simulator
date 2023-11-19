#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char input[100], command[12];

    fgets(input, 100, stdin);

    sscanf(input, "%11s", command);

    printf("%s\n", str);

    return 0;
}