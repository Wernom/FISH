#ifndef CMDEXEC_H
#define CMDXEC_H
#define _POSIX_C_SOURCE 200809L
#define _BSD_SOURCE

#include "cmdline.h"
#include "list.h"

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



int status;
struct sigaction old_SIGCHLD;
struct list list_status_fils;

struct free_on_exit{
    struct list *list_status_fils;
    struct line *li;
};

/**
 * TODO
 * 
 * @param   struct line     li    
 */
void cmd(struct line li, struct sigaction old);

/**
 * TODO
 * 
 * @param   struct line     li    
 */
void cmd_execute(struct line li, size_t nb_cmd);

/**
 * TODO
 * 
 * @param   struct line     li    
 */
void cmd_exit_fish(struct line li);

/**
 * TODO
 * 
 * @param   struct line     li    
 */
bool cmd_cd(struct line li);

/**
 * TODO
 * 
 * @param   struct line     li    
 */
void cmd_redirection(struct line li, int redirect);

/**
 * TODO
 * 
 * @param   struct line     li    
 */
void cmd_handler_SIGINT();

/**
 * TODO
 * 
 * @param   struct line     li    
 */
struct sigaction cmd_SIGINT_nothing();

/**
 * TODO
 * 
 * @param   struct line     li    
 */
void cmd_SIGINT(struct sigaction old, struct line li);

/**
 * TODO
 * 
 * @param   struct line     li    
 */
void cmd_handler_SIGCHLD ();

/**
 * TODO
 * 
 * @param   struct line     li    
 */
void cmd_state_child();

/**
 * TODO
 * 
 * @param   struct line     li    
 */
struct sigaction cmd_SIGCHLD();

/**
 * TODO
 * 
 * @param   struct line     li    
 */
void cmd_SIGCHLD_restor(struct sigaction old);

/**
 * TODO
 * 
 * @param   struct line     li    
 */
void cmd_redirection_black_hole(struct line li);

/**
 * TODO
 * 
 * @param   struct line     li    
 */
int* cmd_multi(struct line li, struct sigaction old, size_t cmd, pid_t pid[], int tubePrev[]);

void cmd_state_child_background();
void free_li (int ev, void *arg);

#endif