#ifndef CMDEXEC_H
#define CMDXEC_H

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

void cmd(struct line li, size_t nb_cmd);
void cmd_execute(struct line li, size_t nb_cmd);
void cmd_exit_fish(struct line li);
bool cmd_cd(struct line li);
void cmd_redirection(struct line li, int redirect);

#endif