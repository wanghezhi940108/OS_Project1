/*login: chen3593
* date: 10/05/15
* name: Raoyin Chen, Hezhi Wang
* id: chen3593, wang5575*/

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include "makeargv.h"
#include "share.h"

#define INELIGIBLE 0
#define READY 1
#define RUNNING 2
#define FINISHED 3

int linenum;
char *in_fp;

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

/*
 * Function:  strip 
 * --------------------
 * Removes illegal characters in a file name
 *
 *  s: the file name
 */
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

/*
 * Function:  strip 
 * --------------------
 * Removes illegal characters in a file name
 *
 *  s: the file name
 */
void redirect(char *input, char *output) {

    strip(input);

    int input_file = open(input, O_RDONLY);

    if (input_file < 0){
    	perror("Error opening input file in child after fork! Exiting.");
    	exit(1);
    } else {
    	dup2(input_file, STDIN_FILENO);
    	close(input_file);
    }   
    strip(output);

    int output_file = open(output, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    if (output_file < 0){
    	perror("Error opening output file in child after fork! Exiting.");
    	exit(1);
    } else{
    	dup2(output_file, STDOUT_FILENO);
    	close(output_file);
    }
}

/*
 * Function:  get_str 
 * --------------------
 * Parse lines of characters from the input text file to an 2d char array
 *
 * returns: a 2d char array of the txt file content
 */
char** get_str() {

    char** string = (char**)malloc(10000);

    if (string == NULL) {
        fprintf(stderr, "failed to allocate memory.\n");
        exit(1);
    }

    char line[300];
    FILE *file; 
    file = fopen(in_fp, "r"); 
    if ( file == NULL ) {
	    printf ("File \"%s\" not found.\n", in_fp);
	    exit(2);
    }

    int i = 0;

    while(fgets(line, sizeof line, file)!=NULL) {
        string[i] = malloc(10000);
        if (string[i] == NULL) {
            fprintf(stderr, "failed to allocate memory.\n");
            exit(1);
        }
        strcpy(string[i], line);
        i++;
    }

    linenum = i;
    fclose(file);
    return string;
}

/*
 * Function:  get_node_array 
 * --------------------
 * Construct the node array
 * returns: an array with all the nodes
 */

struct node* get_node_array() {

    int i, j;
    const char *a = ":";
    int numtokens;
    char **str = get_str();

    struct node* n_array = malloc(linenum * 10000);

    if (n_array == NULL) {
        fprintf(stderr, "failed to allocate memory.\n");
        exit(1);
    }

    static const struct node node_empty;
    struct node Node;
    char **argvp;

    for(i=0; i<linenum; i++) {
        Node = node_empty;
        numtokens = makeargv(str[i], a, &argvp);
        Node.id = i;
        Node.num_parent = 0;
        strcpy(Node.prog, argvp[0]);
        /*parse children from .txt file to node*/
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
        strcpy(Node.input, argvp[2]); /*parse the input filename*/
        strcpy(Node.output, argvp[3]); /*parse the output filename*/
        n_array[i] = Node;
    }

    for(j=0; j<linenum; j++) {
        free(str[j]);
    }

    free(str);
    /* assign num_parent*/
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

/*
 * Function:  determine_eligible
 * --------------------
 * Get the node ids that are ready to run
 * returns: an array with the id of eligible nodes
 */
int *determine_eligible(struct node* n_array, int *size) {

    int i;
    int j = *size;
    int *d_eligible = (int*)malloc(sizeof(int)*1000);

    if (d_eligible == NULL) {
        fprintf(stderr, "failed to allocate memory.\n");
        exit(1);
    }

    for(i=0; i<linenum; i++) {
        if((n_array[i].num_parent == 0) && (n_array[i].status != 3)) {
            d_eligible[j] = i;
            j++;
        }
    }

    *size = j;

    return d_eligible;
}

/*
 * Function:  run
 * --------------------
 * Run the node with given id
 * n_array: an array of all the nodes
 * id: the id of the node to run
 */
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
        exit(1);
    }
    if(pid > 0) {

        wait(NULL);

        n_array[id].status = FINISHED;
        /*update parent numbers in every children*/
        for(i=0; i<n_array[id].num_children; i++) {
            int child_index = n_array[id].children[i];
            n_array[child_index].num_parent --;
        }
    }
    if(pid == 0) {
        n_array[id].status = RUNNING;
        execvp(argv[0], &argv[0]);
        perror("child fail to exec \n");
        exit(1);
    }
}

/*
 * Function:  exec_tree
 * --------------------
 * Run the program tree
 * n_array: an array of all the nodes
 */
void exec_tree(struct node* n_array) {
    int i;
    int size = 0;
    int *d_eligible = determine_eligible(n_array, &size);
  
    while(size != 0) {
    	for(i=0; i<size; i++) {
            int eligible_index = d_eligible[i];
            if(n_array[eligible_index].status != FINISHED) {
                run(n_array, d_eligible[i]);
            }
        }
        size = 0;
        d_eligible = determine_eligible(n_array, &size);
    }
}

int main(int ac, char** av) {    
    if(ac <= 1) {
        printf ("Usage: ./graphexec <filename.txt>\n");
        return 1;
    }
    in_fp = av[1];
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
        return(1);
    }

    exec_tree(n_array);

    free(n_array);

    return(0);
}
