exec_tree(det_num)
    While det_num != NULL 
        for i in det_run
            run(i)
        update()



run(node)
    int pid = fork();
    if(pid > 0) 
        wait(NULL)
        node.state = "FINISHED"
        for i in node.children
            i.num_parent --
    

    if(pid == 0) 
        node.state = "RUNNING"
        exec(node.prog)
    

update()
    det_num = NULL
    for i in node_array
    if(i.num_parent == 0)
        det_num.add(i)
