#include <stdio.h>
#define INELIGIBLE 0
#define READY 1
#define RUNNING 2
#define FINISHED 3
#include "makeargv.h"
#include "share.h"
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>

int linenum;

typedef struct node {
    int id; // corresponds to line number in graph text file
    char prog[1024]; // prog + arguments
    char input[1024]; // filename
    char output[1024]; // filename
    int children[10]; // children IDs
    int num_children; // how many children this node has
    int num_parent; // number of ready parent nodes
    int status;
    pid_t pid; // track it when it's running
} node_t;

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

void redirect(char *input, char *output) {
    strip(input);
    int input_file = open(input, O_RDONLY);
    if (input_file < 0){
    	perror("Error opening input file in child after fork! Exiting.");
    	exit(0);
    } else {
    	dup2(input_file, STDIN_FILENO);
    	close(input_file);
    }
    
    strip(output);
    int output_file = open(output, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    
    if (output_file < 0){
    	perror("Error opening output file in child after fork! Exiting.");
    	exit(0);
    } else{
    	dup2(output_file, STDOUT_FILENO);
    	close(output_file);
    }
}

char** get_str() {
    char** string = (char**)malloc(10000);
    char line[300];
 
    FILE *file; 
    file = fopen("testgraph1.txt", "r"); 
    int i = 0;
    while(fgets(line, sizeof line, file)!=NULL) {
        //printf("%s", line);
        string[i] = malloc(10000);
        strcpy(string[i], line);
        i++;
        //printf("%s\n",string[i - 1] );
        //printf("%d\n", i);
    }
    linenum = i;
    fclose(file);
    return string;
}

struct node* get_node_array() {
    int i;
    const char *a = ":";
    int numtokens;
    char **str = get_str();
    struct node* n_array = malloc(linenum * 10000);
    static const struct node node_empty;
    struct node Node;
    char **argvp;
    for(i=0; i<linenum; i++) {
        Node = node_empty;
        numtokens = makeargv(str[i], a, &argvp);
        
        Node.id = i;
        Node.num_parent = 0;
        strcpy(Node.prog, argvp[0]);
        // parse children from .txt file to node
        const char *none = "none";
        if(!strcmp(argvp[1],none)) {
            Node.children[0] = -1;
        }
        else {
             int k = 0;
             char *n = strtok(argvp[1], " ");
             do { 
                 Node.children[k++] = atoi(n);
              } while (n = strtok(NULL, " "));
             Node.num_children = k;
        }
        
        strcpy(Node.input, argvp[2]);
        
        strcpy(Node.output, argvp[3]);

        n_array[i] = Node;
        //printf("%d\n", Node.input);
    }
    free(str);

    // assign num_parent
    for(i=0; i<linenum; i++) {
        int j;
        for(j=0; j<n_array[i].num_children; j++) {
            int target = n_array[i].children[j];
            if(target != -1) {
                n_array[target].num_parent ++;
            }
        }
    }

    return n_array;
}

int *determine_eligible(struct node* n_array, int *size) {
    int i;
    int j = *size;
    int *d_eligible = (int*)malloc(sizeof(int)*1000);
    for(i=0; i<linenum; i++) {
        if((n_array[i].num_parent == 0) && (n_array[i].status != 3)) {
            d_eligible[j] = i;
            j++;
        }
    }
    *size = j;
    return d_eligible;
}

void run(struct node* n_array, int id) {
    
    char *prog = n_array[id].prog;
    char *input = n_array[id].input;
    char *output = n_array[id].output;
    redirect(input, output);
    const char *a = " ";
    char **argv;
    int numtokens = makeargv(prog, a, &argv);

    
    int i;
    pid_t pid = fork();
    if(pid < 0) {
        fprintf(stderr, "fork error\n");
        exit(-1);
    }
   
    if(pid > 0) {
        wait(NULL);
        n_array[id].status = FINISHED;
        for(i=0; i<n_array[id].num_children; i++) {
            n_array[i].num_parent --;
        }
    }

    if(pid == 0) {
        n_array[id].status = FINISHED;
        
        for(i=0; i<n_array[id].num_children; i++) {
            n_array[i].num_parent --;
        }
        execvp(argv[0], &argv[0]);
        perror("child fail to exec \n");
        exit(0);
    }
}
void exec_tree(struct node* n_array) {
    int i;
    int size = 0;
    int *d_eligible = determine_eligible(n_array, &size);
    while(size != 0) {
        for(i=0; i<size; i++) {
            if(n_array[i].status != FINISHED) {
                run(n_array, i);
            }
        }
        size = 0;
        d_eligible = determine_eligible(n_array, &size);
    }
}



int main() {

    struct node *n_array = get_node_array();
    int id;
    int i;
    int j;
    int has_cycle = 1;
    for(i=0; i<linenum; i++) {
        if(n_array[i].num_parent == 0) {
            has_cycle = 0;
            break;
        }
    }
    if(has_cycle) {
        fprintf(stderr, "Cycles exist in the graph!\n");
        exit(-1);
    }
    exec_tree(n_array);
    return(0);

}


