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

void cmd_execute(struct line li, size_t nb_cmd);

#endif