#define _GNU_SOURCE //gcc was complaining about getline
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

#include "Command.h"
#include "DynamicArray.h"
#include "PIDList.h"

//#include "smallsh.h"


void reap_exited_children(DYNARR*);
void reap_children(DYNARR*);
void run_foreground(COMMAND*, DYNARR*);
void run_background(COMMAND*, DYNARR*);
void ignore_sig_int(int);


void ignore_sig_int(int sig_number) {
    write(1, "\n: ", 3);
    signal(SIGINT, ignore_sig_int);
}

/*
 * Name 
 * 	exec_command(struct Command*, Struct PIDList*, int*)
 *		
 *
 * Description
 * 	Used to execute a command parsed from the smallsh command line.  When 
 * 	exec_command is called, it forks a child and uses the exec() family of
 * 	functions to start a program described by the string array in the passed
 * 	Command struct.  If either exec() or fork() fails, then status is set to
 * 	the appropriate exit status so smallsh can access it.  Otherwise exec()
 * 	runs the program, and uses waitpid() appropriately depending on if the
 * 	Command is a background process or not. Adds the child PID to a list that
 * 	the parent can later use to clean up zombie processes.
 *
 * Return 
 * 	None (void)
 *
 * Notes
 * 	Uses some code from example on wait(2) manpage 
 *
 */

void exec_command(COMMAND* c, PIDLIST* pids, int* status) {
	pid_t child_pid, w;
	int wstatus, exec_status;

	child_pid = fork();

	if (child_pid == -1) {
		perror("fork");	
		exit(EXIT_FAILURE);
	}
	
	// child process
	if (child_pid == 0) {
		if (c->input_redirection == true) {
			//set input redirection to input_path
		}
		if (c->output_redirection) {
			//set output redirection to output_path
		}
		if (c->is_background_process == true) {
			if (c->input_redirection == false) {
				//set redirection to /dev/null
			}
			if (c->output_redirection == false) {
				// set redirection to /dev/null
			}
		}
		//exec()
		exec_status = execvp(c->arguments->strings[0], c->arguments->strings);
		
		//check if exec() succeeded or failed
		if (exec_status == -1) {
			perror("exec");
			exit(EXIT_FAILURE);
		}
	}
	
	// parent
	else {
		if (c->is_background_process == true) {
			//dont wait for the child to finish termination
			//add the PID to the background PID array
			//not sure whether to use waitpid() here or not
		}
		else {
			//wait for the child to finish termination
			w = waitpid(child_pid, &wstatus, 0); //check if it doesnt mess
															 // anything up with 0 param
		}
	}
}

void run_sh() {
	struct sigaction SIGINT_action = {0};
	bool running = true;	
	//list of parent's child processes
	PIDLIST child_procs;
   
	SIGINT_action.sa_handler = ignore_sig_int;
   sigfillset(&SIGINT_action.sa_mask);
   SIGINT_action.sa_flags = 0;
		

	while(running) {
   	signal(SIGINT, ignore_sig_int);
      COMMAND command_line;

		//holds status for last command executed
		int status = 0;

      init_command(&command_line);
       
		char* buffer = NULL;
      size_t n = 2048;
      
		write(1, ": ", 2);
		fflush(stdout);
        
		getline(&buffer, &n, stdin);
     	remove_newline(buffer);

		parse_command(&command_line, buffer);
		
		//print_command(&command_line);
		
		if(command_line.is_builtin == true) {
			handle_builtin(command_line.builtin, &status, &running);
		}
    	else {
			exec_command(&command_line, &child_procs, &status);
		}
	 }
	 //clean up children after exit or ??sigstop??
}

int main() {
    //signal(SIGINT, ignore_sig_int);
    //test_loop();
    run_sh();
    return 0;
}
