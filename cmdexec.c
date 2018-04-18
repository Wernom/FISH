#include "cmdexec.h"


int status;

void cmd(struct line li, size_t nb_cmd){
    printf("\nCommande result: \n");
    if(!fork()){
        if(li.redirect_output || li.redirect_input){
            cmd_redirection(li, li.redirect_input ? 0 : 1);
        }else{
            cmd_execute(li, 0);
        }
    }
    wait(&status);
    if(WIFEXITED(status)){
        printf("\nChild terminated normaly.\nReturn code : %d.\n", WEXITSTATUS(status));
    }
    if (WIFSIGNALED(status)) {
        printf("\nChild terminated by a signal.\nKiled by : %d.\n", WTERMSIG(status));
    }
}

void cmd_execute(struct line li, size_t nb_cmd){
    execvp(li.cmds[nb_cmd].args[0], li.cmds[nb_cmd].args);
    fprintf(stderr, "Invalid command");
    exit(EXIT_FAILURE);
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

void cmd_redirection(struct line li, int redirect){
    int fic;
    if(redirect == 0){
        fic = open(li.file_input, O_RDONLY);
    }else if (redirect == 1){
        fic = creat(li.file_output, S_IRWXU | S_IRWXG | S_IRWXO);
    }else{
        fprintf(stderr, "wrong redirect value");
        exit(EXIT_FAILURE);
    }
    if(fic == -1){
        perror("cmdexec.c -> open/creat");
        exit(EXIT_FAILURE);
    }
    if(dup2(fic, redirect) == -1){
        perror("cmdexec.c -> dup2");
        exit(EXIT_FAILURE);
    }

    cmd_execute(li, 0);

    if(close(fic) == -1){
        perror("cmdexex.c -> close");
        exit(EXIT_FAILURE);
    }
}