#include "cmdexec.h"

#define MAX_PID 16

int status;
struct sigaction old_SIGCHLD;


void cmd(struct line li, size_t nb_cmd, struct sigaction old){
    printf("\nCommande result: \n");
    pid_t pid[MAX_PID];
    int tube[2];
    if(pipe(tube) == -1){
        perror("cmdexec.c -> pipe");
        exit(EXIT_FAILURE);
    }
    pid[0] = fork();
    if(!pid[0]){
        

        if(dup2(tube[1], 1) == -1){
            perror("1");
            exit(EXIT_FAILURE);
        }
        close(tube[1]);
        close(tube[0]);
        if((li.redirect_output || li.redirect_input)){
            cmd_redirection(li, li.redirect_input ? 0 : 1);
        }else{
            cmd_execute(li, 0);
        }
        
    }

    close(tube[1]);
    pid[1] = fork();

    if(!pid[1]){
        if(dup2(tube[0], 0) == -1){
            perror("2");
            exit(EXIT_FAILURE);
        }
        close(tube[0]);
        cmd_execute(li, 1);
    }

    if(close(tube[0]) == -1){
        perror("cmdexec.c -> close 3");
        exit(EXIT_FAILURE);
    }
    if(li.background){
        old_SIGCHLD = cmd_SIGCHLD();
    }else{
        /*for(size_t i=0; i<nb_cmd; ++i){
            waitpid(pid[i], &status, 0);//eliminer les zombies dans le bon ordre
            cmd_state_child();
        }*/
        
        waitpid(pid[0], &status, 0);
        waitpid(pid[1], &status, 0);
    }
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

/*void cmd_multi(struct line li, struct sigaction old, size_t cmd, size_t nb_cmd, pid_t *pid){
    size_t index = nb_cmd - cmd;
    pid[index] = fork();
    if(pid[index] == -1){
        perror("cmdexec.c -> fork");
        line_reset(&li);
        exit(EXIT_FAILURE);
    }
    if(!pid[index]){
        cmd_SIGINT(old);
        if(cmd != 0){
            cmd_multi(li, old, cmd-1, nb_cmd, pid);
        }

        int tube[2];
        if(pipe(tube) == -1){
            perror("cmdexec.c -> pipe");
            line_reset(&li);
            exit(EXIT_FAILURE);
        }

        if(cmd != nb_cmd){
            if(dup2(0, tube[0]) == -1){
                perror("cmdexec.c -> dup2");
                line_reset(&li);
                exit(EXIT_FAILURE);
            }
            if(close(tube[0]) == -1){
                perror("cmdexec.c -> close");
                line_reset(&li);
                exit(EXIT_FAILURE);
            }
        }
        

        if(cmd != 0){
            if(dup2(tube[1], 1) == -1){
                perror("cmdexec.c -> dup2");
                line_reset(&li);
                exit(EXIT_FAILURE);
            }
            if(close(tube[1]) == -1){
                perror("cmdexec.c -> close");
                line_reset(&li);
                exit(EXIT_FAILURE);
            }
        }

        if((li.redirect_output || li.redirect_input) && cmd == nb_cmd){
            cmd_redirection(li, li.redirect_input ? 0 : 1);
        }else{
            cmd_redirection_black_hole(li);
            cmd_execute(li, 0);
        }
    }
    
}*/