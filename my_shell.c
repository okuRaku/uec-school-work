/*
  Very basic linux Shell program, demonstrating child/parent processes
  execvp() is used to handled command execution, allowing paths to be
  manual or automatic.  Arguments are also handled as expected.

  Written by Jesse Jennings for Operating Systems Bonus #1
  University of Electro-Communications, Tokyo, Japan
*/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

extern int errno;

static char *my_argv[100];

void fill_argv(char *tmp_argv)
{
       char *foo = tmp_argv;
       int index = 0;
       char ret[100];
       bzero(ret, 100);
       while(*foo != '\0') {
               if(index == 10)
                       break;

               if(*foo == ' ') {
                       if(my_argv[index] == NULL)
                               my_argv[index] = (char *)malloc(sizeof(char) * strlen(ret) + 1);
                       else {
                               bzero(my_argv[index], strlen(my_argv[index]));
                       }
                       strncpy(my_argv[index], ret, strlen(ret));
                       strncat(my_argv[index], "\0", 1);
                       bzero(ret, 100);
                       index++;
               } else {
                       strncat(ret, foo, 1);
               }
               foo++;
       }
       my_argv[index] = (char *)malloc(sizeof(char) * strlen(ret) + 1);
       strncpy(my_argv[index], ret, strlen(ret));
       strncat(my_argv[index], "\0", 1);
}

void free_argv()
{
       int index;
       for(index=0;my_argv[index]!=NULL;index++) {
               bzero(my_argv[index], strlen(my_argv[index])+1);
               my_argv[index] = NULL;
               free(my_argv[index]);
       }
}

void call_exec(char *cmd)
{
       int i;
       if(fork() == 0) {
               i = execvp(cmd, my_argv);
               if(i < 0) {
                       printf("%s: %s\n", cmd, "command not found");
                       exit(1);
               }
       } else {
               wait(NULL);
       }
}

int main(int argc, char **argv, char **envp)
{
       char *cmd = (char *)malloc(sizeof(char) * 100);
       char c;
       char *tmp = (char *)malloc(sizeof(char) * 100);
       printf("\nWelcome to HappyShell.  Try cmds like 'ls / -al'  Ctrl-D to exit.");
       printf("\n\\('_')_ ");
       fflush(stdout);
       while(c != EOF){
               c = getchar();
               switch(c){
                       case '\n': if(tmp[0] == '\0') {
                                       printf("\\(X_X); ");
                               } else {
                                       fill_argv(tmp);
                                       strncpy(cmd, my_argv[0], strlen(my_argv[0]));
                                       strncat(cmd, "\0", 1);
                                       call_exec(cmd);
                                       free_argv();
                                       printf("\\(^_^)/ ");
                                       bzero(cmd, 100);
                               }
                               bzero(tmp, 100);
                               break;
                       default:
                               strncat(tmp, &c, 1);
                               break;
               }
       }
       free(tmp);
       printf("Exiting...\n");
       return 0;
}
