#include "cmdexec.h"

#define MAX_PID 16

int status;
struct sigaction old_SIGCHLD;


void cmd(struct line li, struct sigaction old){
    printf("\nCommande result: \n");
    int *pid = calloc(MAX_PID, sizeof(pid_t));
    int *tube = NULL;

    for(int i = 0; i < li.ncmds; ++i){
        tube = cmd_multi(li, old, i, pid, tube);//TODO: fuite memoire il y en a partout c'est dégueulasse  remettre en place les redirections l'arriere plan et la gestion des signaux.
    }
    
    if(li.background){
        old_SIGCHLD = cmd_SIGCHLD();
    }else{
        for(size_t i=0; i<li.ncmds; ++i){
            waitpid(pid[i], &status, 0);//eliminer les zombies dans le bon ordre
            cmd_state_child();
        }
    }
    free(tube);
    free(pid);
}

void cmd_execute(struct line li, size_t nb_cmd){
    if(li.background){
        cmd_redirection_black_hole(li);
    }
    execvp(li.cmds[nb_cmd].args[0], li.cmds[nb_cmd].args);
    printf("Invalid command");
    line_reset(&li);
    exit(EXIT_FAILURE);
}

void cmd_exit_fish(struct line li){
    if(!strcmp(li.cmds[0].args[0], "exit")){
        line_reset(&li);
        exit(EXIT_SUCCESS);
    }
}

bool cmd_cd(struct line li){
    if(!strcmp(li.cmds[0].args[0], "cd")){
        if(li.cmds[0].args[1] == NULL){
            li.cmds[0].args[1] = getenv("HOME");
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
            line_reset(&li);
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
        line_reset(&li);
        exit(EXIT_FAILURE);
    }
    if(fic == -1){
        perror("cmdexec.c -> open/creat");
        line_reset(&li);
        exit(EXIT_FAILURE);
    }
    if(dup2(fic, redirect) == -1){
        perror("cmdexec.c -> dup2");
        line_reset(&li);
        exit(EXIT_FAILURE);
    }

    cmd_execute(li, 0);

    if(close(fic) == -1){
        perror("cmdexex.c -> close");
        line_reset(&li);
        exit(EXIT_FAILURE);
    }
}

void cmd_handler_SIGINT(){
    printf("\n");
}

struct sigaction cmd_SIGINT_nothing(){
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

void cmd_SIGINT(struct sigaction old){
    if (sigaction(SIGINT, &old, NULL) == -1) {
        perror("cmdexec.c -> sigaction");
        exit(EXIT_FAILURE);
    }
}

void cmd_handler_SIGCHLD(){
    while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {}
    cmd_SIGCHLD_restor(old_SIGCHLD);
}

void cmd_state_child(){
    if(WIFEXITED(status)){
        printf("\nChild terminated normaly.\nReturn code : %d.\n", WEXITSTATUS(status));
    }
    if (WIFSIGNALED(status)) {
        printf("\nChild terminated by a signal.\nKiled by : %d.\n", WTERMSIG(status));
    }
}

struct sigaction cmd_SIGCHLD(){
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
        if(pipe(tubeNext) == -1){
            perror("cmdexec.c -> pipe");
            exit(EXIT_FAILURE);
        }
    }
    pid[cmd] = fork();
    if(!pid[cmd]){//FILS
        if(cmd != 0){
            if(dup2(tubePrev[0], 0) == -1){
                perror("cmdexec.c -> dup2 1");
                exit(EXIT_FAILURE);//TODO: ne pas oublier fuite memoire
            }
            if(close(tubePrev[0]) == -1){
                perror("cmdexec.c -> close 1");
                exit(EXIT_FAILURE);
            }
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