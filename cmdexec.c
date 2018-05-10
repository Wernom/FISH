#include "cmdexec.h"

#define MAX_PID 16

void cmd(struct line li, struct sigaction old){
    printf("\nCommande result: \n");
    int *pid = calloc(MAX_PID, sizeof(pid_t));
    int *tube = NULL;

    for(int i = 0; i < li.ncmds; ++i){
        tube = cmd_multi(li, old, i, pid, tube);
    }
    
    if(li.background){
        old_SIGCHLD = cmd_SIGCHLD(li);
    }else{
        for(size_t i=0; i<li.ncmds; ++i){
            if(waitpid(pid[i], &status, 0) == -1){
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
            cmd_state_child();
        }
    }
    cmd_state_child_background();
    free(tube);

    free(pid);
}

void cmd_execute(struct line li, size_t nb_cmd){
    if(li.background){
        cmd_redirection_black_hole(li);
    }
    execvp(li.cmds[nb_cmd].args[0], li.cmds[nb_cmd].args);
    printf("Invalid command");
    exit(EXIT_FAILURE);
}

void cmd_exit_fish(struct line li){
    if(!strcmp(li.cmds[0].args[0], "exit")){
        exit(EXIT_SUCCESS);
    }
}

bool cmd_cd(struct line li){
    if(!strcmp(li.cmds[0].args[0], "cd")){
        if(li.cmds[0].args[1] == NULL ){
            li.cmds[0].args[1] = getenv("HOME");
        }else if(li.cmds[0].args[1][0] == '~'){
            li.cmds[0].args[1] = strcat(getenv("HOME"), li.cmds[0].args[1] + 1);
        }
        
        if(chdir(li.cmds[0].args[1])==-1) perror("cmdexec.c -> chdir");
        return true;
    }
    return false;
}

void cmd_redirection_black_hole(struct line li){
    if(li.background){
        if(dup2(open("/dev/null", O_RDONLY), 0) == -1){
            perror("cmdexec.c -> dup2");
            exit(EXIT_FAILURE);
        }
    }
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

    if(close(fic) == -1){
        perror("cmdexex.c -> close");
        exit(EXIT_FAILURE);
    }
}

void cmd_handler_SIGINT(){
    printf("\n");
}

struct sigaction cmd_SIGINT_nothing(struct line li){
    struct sigaction new, old;
    new.sa_handler = SIG_IGN;
    new.sa_flags = 0;
    sigemptyset(&new.sa_mask);
    if (sigaction(SIGINT, &new, &old) == -1) {
        perror("cmdexec.c -> sigaction");
        exit(EXIT_FAILURE);
    }
    return old;
}

void cmd_SIGINT(struct sigaction old, struct line li){
    if (sigaction(SIGINT, &old, NULL) == -1) {
        perror("cmdexec.c -> sigaction");
        exit(EXIT_FAILURE);
    }
}

void cmd_handler_SIGCHLD(){
    int ret;
    while ((ret = waitpid((pid_t)(-1), &status, WNOHANG)) > 0) {
        if(ret == -1){
            perror("waitpid");
            exit(EXIT_FAILURE);
            //TODO: check on_exit pour netoyer la memoire.
        }
        struct status data;
        data.pid = ret;
        data.status = status;
        list_add(&list_status_fils, data);
    }
    cmd_SIGCHLD_restor(old_SIGCHLD);
}

void cmd_state_child_background(){
    while(!list_is_empty(&list_status_fils)){
        struct status *data = list_pull(&list_status_fils);

        if(WIFEXITED(data->status)){
            printf("\nChild %d terminated normaly.\nReturn code : %d.\n", data->pid, WEXITSTATUS(data->status));
        }
        if (WIFSIGNALED(data->status)) {
            printf("\nChild %d terminated by a signal.\nKiled by : %d.\n", data->pid,  WTERMSIG(data->status));
        }
        free(data);
    }
}



void cmd_state_child(){
    if(WIFEXITED(status)){
        printf("\nChild terminated normaly.\nReturn code : %d.\n", WEXITSTATUS(status));
    }
    if (WIFSIGNALED(status)) {
        printf("\nChild terminated by a signal.\nKiled by : %d.\n", WTERMSIG(status));
    }
}

struct sigaction cmd_SIGCHLD(struct line li){
    struct sigaction new, old;
    new.sa_handler = &cmd_handler_SIGCHLD;
    sigemptyset(&new.sa_mask);
    new.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &new, &old) == -1) {
        perror("cmdexec.c -> sigaction");
        exit(EXIT_FAILURE);
    }
    return old;
}

void cmd_SIGCHLD_restor(struct sigaction old){
    if (sigaction(SIGCHLD, &old, NULL) == -1) {
        perror("cmdexec.c -> sigaction");
        exit(EXIT_FAILURE);
    }
}

int* cmd_multi(struct line li, struct sigaction old, size_t cmd, pid_t *pid, int *tubePrev){

    int *tubeNext = NULL; 
    if(cmd != li.ncmds-1){
        tubeNext = calloc(2, sizeof(int));
        if(!tubeNext){
            fprintf(stderr, "Erreur calloc");
            exit(EXIT_FAILURE);
        }
        if(pipe(tubeNext) == -1){
            perror("cmdexec.c -> pipe");
            exit(EXIT_FAILURE);
        }
    }
    
    pid[cmd] = fork();
    if(!pid[cmd]){
        if(!li.background){
            cmd_SIGINT(old, li);
        }
        if(cmd != 0){
            if(dup2(tubePrev[0], 0) == -1){
                perror("cmdexec.c -> dup2 1");
                exit(EXIT_FAILURE);
            }
            if(close(tubePrev[0]) == -1){
                perror("cmdexec.c -> close 1");
                exit(EXIT_FAILURE);
            }
        }else if(li.redirect_input){
            cmd_redirection(li, 0);
        }
        if(cmd != li.ncmds-1){
            if(dup2(tubeNext[1], 1) == -1){
                perror("cmdexec.c -> dup2 1");
                exit(EXIT_FAILURE);
            }
            if(close(tubeNext[1]) == -1){
                perror("cmdexec.c -> close 2");
                exit(EXIT_FAILURE);
            }
            if(close(tubeNext[0]) == -1){
                perror("cmdexec.c -> close 3");
                exit(EXIT_FAILURE);
            }
        }else if(li.redirect_output){
            cmd_redirection(li, 1);
        }


        cmd_execute(li, cmd);
    }

    if(cmd != 0){
        if(close(tubePrev[0]) == -1){
            perror("cmdexec.c -> close 4");
            exit(EXIT_FAILURE);
        }
    }

    if(cmd != li.ncmds-1){
        if(close(tubeNext[1]) == -1){
            perror("cmdexec.c -> close 2");
            exit(EXIT_FAILURE);
        }
    }

    return tubeNext;
}

void free_li (int ev, void *arg){
    struct free_on_exit *fr = (struct free_on_exit *)arg;
    line_reset(fr->li);
    while(!list_is_empty(fr->list_status_fils)){
        list_pull(fr->list_status_fils);
    }
}