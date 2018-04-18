#include <stdio.h>
#include <string.h>

#include "cmdline.h"
#include "cmdexec.h"

#define BUFLEN 2048
#define SIZE_CUR_DIR 2048

#define YESNO(i) ((i) ? "Y" : "N")

int main() {
  struct line li;
  char buf[BUFLEN];
  char cur_dir[SIZE_CUR_DIR];

  line_init(&li);

  for (;;) {
    if(!getcwd(cur_dir, SIZE_CUR_DIR)){
      perror("fish.c -> getcwd");
      exit(EXIT_FAILURE);
    }  
    printf("\033[1;32mfish\033[1;00m:\033[1;36m%s\033[00m > ", cur_dir); //\033[XXm is used for display the a colored and bold text where XX is the code for the action to realise
    char *c = fgets(buf, BUFLEN, stdin);
    ++c;

    int err = line_parse(&li, buf);
    if (err) { 
      //the command line entered by the user isn't valid
      line_reset(&li);
      continue;
    }

    fprintf(stderr, "Command line:\n");
    fprintf(stderr, "\tNumber of commands: %zu\n", li.ncmds);

    for (size_t i = 0; i < li.ncmds; ++i) {
      fprintf(stderr, "\t\tCommand #%zu:\n", i);
      fprintf(stderr, "\t\t\tNumber of args: %zu\n", li.cmds[i].nargs);
      fprintf(stderr, "\t\t\tArgs:");
      for (size_t j = 0; j < li.cmds[i].nargs; ++j) {
        fprintf(stderr, " \"%s\"", li.cmds[i].args[j]);
      }
      fprintf(stderr, "\n");
    }

    fprintf(stderr, "\tRedirection of input: %s\n", YESNO(li.redirect_input));
    if (li.redirect_input) {
      fprintf(stderr, "\t\tFilename: '%s'\n", li.file_input);
    }

    fprintf(stderr, "\tRedirection of output: %s\n", YESNO(li.redirect_output));
    if (li.redirect_output) {
      fprintf(stderr, "\t\tFilename: '%s'\n", li.file_output);
    }

    fprintf(stderr, "\tBackground: %s\n", YESNO(li.background));

    /* do something with li */
    /*ok*/
    if(li.cmds[0].args[0] != NULL){
      cmd_exit_fish(li);
      if(cmd_cd(li)){}
      else{
        cmd(li, 0);
      }
    }

    line_reset(&li);
  }
  
  return 0;
}

