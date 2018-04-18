#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int status;



int main(int argc, char **argv) {
    char buf[1024];

    printf("%s\n", getcwd(buf, 1024));

    return 0;
}

