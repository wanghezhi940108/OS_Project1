/*login: chen3593
* date: 10/05/15
* name: Raoyin Chen, Hezhi Wang
* id: chen3593, wang5575*/

Raoyin Chen chen3593
HeZhi Wang wang5575

1. The purpose of our program:
Our main program graphexec will be responsible for analyzing a graph of user programs, determining which ones are eligible to run, and running programs that are eligible to run at the moment in time. 

2. How to compile the program:
There are two ways to compile the program:
(1) make
(2) gcc graphexec.c -o graphexec

3. How to use the program from the shell:
After compile, run ./graphexec <filename>

4. What exactly our program does:
Our main program will first parse the graph file given in the first argument, construct a tree that models the graph, and then start executing the nodes. Our program can determine which nodes are eligible to run, then execute those nodes, wait for any node to finish, then repeat this process until all nodes have finished executing.
