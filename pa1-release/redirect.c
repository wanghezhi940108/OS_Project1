/*
* This won't compile, of course, but you can use this code
* in your program to perform the file redirection.
* ....
* if you are the process responsible for running the 
* command line argument, e.g., 
* echo hi there:1:blank-file.txt:1echo1.out
* then you'll need to redirect the input and output files
* to STDIN and STDOUT with the following code...
* In this example, 'n' is a node with a filename specified in
* n->input and another filename specified in n->output.
*/

int input_file = open(n -> input, O_RDONLY);
if (input_file < 0){
	perror("Error opening input file in child after fork! Exiting.");
	exit(0);
} else {
	dup2(input_file, STDIN_FILENO);
	close(input_file);
}

int output_file = open(n->output, O_WRONLY|O_CREAT|O_TRUNC, 0644);

if (output_file < 0){
	perror("Error opening output file in child after fork! Exiting.");
	exit(0);
} else{
	dup2(output_file, STDOUT_FILENO);
	close(output_file);
}

/* .....
* using makeargv, parse the program arguments and give to 
* execvp to run....
*/
