#include "cmdexec.h"


int status;

void cmd_execute(struct line li, size_t nb_cmd){
    printf("\nCommande result: \n");
    if(!fork()){  
        execvp(li.cmds[nb_cmd].args[0], li.cmds[nb_cmd].args);
        fprintf(stderr, "Command invalid ");
        exit(EXIT_FAILURE);
    }
    wait(&status);
    if(WIFEXITED(status)){
        printf("\nChild terminated normaly.\nReturn code : %d.\n", WEXITSTATUS(status));
    }
    if (WIFSIGNALED(status)) {
        printf("\nChild terminated by a signal.\nKiled by : %d.\n", WTERMSIG(status));
    }
}

void cmd_exit_fish(struct line li){
    if(!strcmp(li.cmds[0].args[0], "exit")){
        exit(EXIT_SUCCESS);
    }
}

bool cmd_cd(struct line li){
    if(!strcmp(li.cmds[0].args[0], "cd")){
        if(chdir(li.cmds[0].args[1])==-1) perror("cmdexec.c -> chdir");
        return true;
    }
    return false;
}