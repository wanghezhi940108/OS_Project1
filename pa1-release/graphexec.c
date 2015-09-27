#include <stdio.h>
#define INELIGIBLE 0
#define READY 1
#define RUNNING 2
#define FINISHED 3
#include "makeargv.h"

typedef struct node {
    int id; // corresponds to line number in graph text file
    char prog[1024]; // prog + arguments
    char input[1024]; // filename
    char output[1024]; // filename
    int children[10]; // children IDs
    int num_children; // how many children this node has
    int status;
    pid_t pid; // track it when it's running
} node_t;

int main() {
    //
    FILE *fp;
    char str[60];
    
    /* opening file for reading */
    fp = fopen("testgraph0.txt" , "r");
    if(fp == NULL) 
    {
       perror("Error opening file");
       return(-1);
    }
    fgets (str, 60, fp);
    fclose(fp);
    printf("%s\n",str);
    // makeargv
    int i;
    char a[] = ":"; 
    char **argvp;
    int numtokens;

    numtokens = makeargv(str, a, &argvp);
    for(i=0; i<numtokens; i++) {
        printf("%d:%s\n", i, argvp[i]);
    }
    return(0);

    //

}
