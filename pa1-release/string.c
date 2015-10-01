#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include "makeargv.h"

void strip(char *s) {
    char *p2 = s;
        while(*s != '\0') {
            if(*s != '\t' && *s != '\n') {
                *p2++ = *s++;
            } else {
                ++s;
            }
        }
    *p2 = '\0';
} 

int main(int ac, char *av[]) {
    char prog[1024] = "ls -l";
    char input[1024] = "blank-file.txt";
    char output[1024] = "file.txt\n";
    const char *a = " ";
    char **argv;
    int numtokens = makeargv(prog, a, &argv);
    strip(output);
    int input_file = open(input, O_RDONLY);
    if (input_file < 0){
    	perror("Error opening input file in child after fork! Exiting.");
    	exit(0);
    } else {
    	dup2(input_file, STDIN_FILENO);
    	close(input_file);
    }
    
    int output_file = open(output, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    
    if (output_file < 0){
    	perror("Error opening output file in child after fork! Exiting.");
    	exit(0);
    } else{
    	dup2(output_file, STDOUT_FILENO);
    	close(output_file);
    }

    pid_t pid = fork();
    if(pid > 0) {
        wait(NULL);
        printf("finish\n");
    }
    if(pid == 0) {
        execvp(argv[0], &argv[0]);
        perror("child fail to exec \n");
        return 1;
    }
}
