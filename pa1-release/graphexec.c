#include <stdio.h>
#define INELIGIBLE 0
#define READY 1
#define RUNNING 2
#define FINISHED 3
#include "makeargv.h"
#include "share.h"
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

char** get_str() {
    char** string = (char**)malloc(10000);
    char line[100];
 
    FILE *file; 
    file = fopen("testgraph5.txt", "r"); 
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

int main() {

    struct node *n_array = get_node_array();
    int id;
    int i;
    int j;
    for(i=0; i<linenum; i++) {
        printf("%d", n_array[i].num_parent);
        printf("\n");
    }
    return(0);

}


