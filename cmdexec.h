#ifndef CMDEXEC_H
#define CMDXEC_H
#define _POSIX_C_SOURCE 200809L

#include "cmdline.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>



void cmd(struct line li, struct sigaction old);
void cmd_execute(struct line li, size_t nb_cmd);
void cmd_exit_fish(struct line li);
bool cmd_cd(struct line li);
void cmd_redirection(struct line li, int redirect);
void cmd_handler_SIGINT();
struct sigaction cmd_SIGINT_nothing();
void cmd_SIGINT(struct sigaction old);
void cmd_handler_SIGCHLD ();
void cmd_state_child();
struct sigaction cmd_SIGCHLD();
void cmd_SIGCHLD_restor(struct sigaction old);
void cmd_redirection_black_hole(struct line li);
int* cmd_multi(struct line li, struct sigaction old, size_t cmd, pid_t pid[], int tubePrev[]);

#endif