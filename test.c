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

int main(int argc, char **argv) {
    

    /*char *cmd[]= {"echo","azerty",NULL};
    execvp(cmd[0], cmd);*/

    int tube[2];
    pipe(tube);
    pid_t pid1 = fork();
    
    if(!pid1){

        if(dup2(tube[1], 1) == -1){
            perror("1");
            exit(EXIT_FAILURE);
        }
        close(tube[1]);
        close(tube[0]);
        
        execlp("ps", "ps", "-e", NULL);

        exit(0);
    }
    close(tube[1]);
    int pid2 = fork();

    if(!pid2){
        if(dup2(tube[0], 0) == -1){
            perror("2");
            exit(EXIT_FAILURE);
        }
        close(tube[0]);
        execlp("wc", "wc", "-l", NULL);
        exit(0);
    }
    close(tube[0]);

    wait(NULL);
    wait(NULL);

    
    return 0;
}

