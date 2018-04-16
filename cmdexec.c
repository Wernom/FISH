#include "cmdexec.h"

#include <stdio.h>
#include <stdlib.h>

int status;

void cmd_execute(struct line li, size_t nb_cmd){
    if(fork()){
        execl("/bin/sh","sh","-c", li.cmds[nb_cmd].args);
        fprintf(stderr, "Command invalid ");
        exit(1);
    }
    wait(&status);
    if(WIFEXITED(status)){
        printf("\nchild terminated normaly.\nReturn code : %d.\n", WEXITSTATUS(status));
    }
    if (WIFSIGNALED(status)) {
        printf("\nChild terminated by a signal.\nKiled by : %d.\n", WTERMSIG(status));
    }
}