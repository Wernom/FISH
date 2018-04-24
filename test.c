#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


int status;

void handler(){

}



int main(int argc, char **argv) {
    
    if(!fork()){
        execl("/bin/sh", "sh", "-c", argv[1], NULL);
        perror("execl");
        exit(1);
    }

    struct sigaction new, old;
    new.sa_handler= SIG_IGN;
    sigaction(SIGINT, &new, &old);
    wait(&status);

    if(WIFEXITED(status)){
        printf("\nChild terminated normaly.\nReturn code : %d.\n", WEXITSTATUS(status));
    }
    if (WIFSIGNALED(status)) {
        printf("\nChild terminated by a signal.\nKiled by : %d.\n", WTERMSIG(status));
    }
    
    return 0;
}

