/* File: smallsh.c
 * Author: Nicholai Gallegos
 *	Date: 2/8/2021
 *	Desc: A simple shell allowing users to execute commands 
 */
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

/* global variables for signal handlers */
int sigint_flag = 0;
int sigstp_flag = 0; /* acts as a flag to toggle foreground-only mode */

void ignore_sig_int(int);

/* Name: sigstp_handler
 * Desc: catchs the SIGTSTP signal and sets the foreground-only mode flag.
 * notifies the user when the signal has been recieved and what mode the
 * shell is currently in.
 */
void sigstp_handler(int sig_number) {
	write(1, "signal SIGSTP recieved\n", 23);
	if (sigstp_flag == 0) {
		sigstp_flag = 1;
		write(1, "foreground-only mode on\n", 24);
	}
	else {
		write(1, "foreground-only mode off\n", 25);
		sigstp_flag = 0;
	}
	write(1, ": ", 2);
	fflush(stdout);
	//sleep(1);
}


void ignore_sig_int(int sig_number) {
	/* this handler contains nothing, it has the effect of ignoring SIGINT in
	 * the parent */
}

/* Name: force_kill_zombies
 * Desc: Kills all background processes with SIGKILL signal. This is
 * to be used to kill bg procs before the shell exits
 */
void force_kill_zombies(PIDLIST* pids) {
	/* check if the list is empty */
	if (pids->head != NULL) {
		PID* curr = pids->head;
		/* send a signal to all current processes */
		while (curr != NULL) {
			kill((pid_t) curr->processID, SIGKILL);
			curr = curr->next;
		}
	}	
}

/* Name: zombie_cleanup
 * Desc: Checks for zombie processes, waits on background processes.  Prints
 * exit status of exited processes.  If a process terminates abnormally, then
 * it prints the signal the child was killed with 
 */
void zombie_cleanup(PIDLIST* pids) {
	int zstatus;
	pid_t proc;

	PID* curr_pid = pids->head;
	
	/* check that there are background processes */
	if (pids->head != NULL) {
		while (curr_pid != NULL) {
			int curr_proc_id = curr_pid->processID;
			proc = waitpid((pid_t) curr_proc_id, &zstatus, WNOHANG);
			/*check for normal termination */
			if ((int) proc == curr_proc_id) {	
				if(WIFEXITED(zstatus)) {
					/* print relevant information */
					printf("\nprocess ID: %d : exit status: %d\n", curr_proc_id, 
					WEXITSTATUS(zstatus));
					fflush(stdout);
					curr_pid = curr_pid->next;
					remove_PIDList(pids, curr_proc_id); 
				}
				/* check for abnormal termination */
				else if(WIFSIGNALED(zstatus)) {
					/* print relevant information */
					printf("killed by signal: %d\n", WTERMSIG(zstatus));
					fflush(stdout);
					printf("\nprocess ID: %d : exit status: %d\n", curr_proc_id, 
					WEXITSTATUS(zstatus));
					fflush(stdout);
					curr_pid = curr_pid->next;
					remove_PIDList(pids, curr_proc_id); 
				}
			}
			else {
				/* keep walking the list if no exited or terminated child processes found */
				curr_pid = curr_pid->next;
			}
		}
	}
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

void exec_command(COMMAND* c, PIDLIST* pids, int* status, 
	struct sigaction* sigact) {

	pid_t child_pid, w;
	int wstatus, exec_status, inp, out;

	child_pid = fork();

	/* check if there was a fork error */
	if (child_pid == -1) {
		perror("fork");
		/* update status */
		*status = 1;
		exit(EXIT_FAILURE);
	}
	
	// child process, reset siginthandler somewhere in here
	if (child_pid == 0) {
		/*
		 * check if there is input_redirection in the Command struct, if there 
		 * is then redirect stdin from the specified file path
		 */
		if (c->input_redirection == true) {
			//set input redirection to input_path
			inp = open(c->input_path, O_RDONLY, 0644);
			if (inp == -1) {
				perror("input redirection error");
				*status = 1;
				exit(EXIT_FAILURE);
			}
			dup2(inp, 0);
		}
		/*
		 * same as above, except this checks for output redirection and 
		 * redirects stdout
		 */
		if (c->output_redirection == true) {
			//set output redirection to output_path
			out = open(c->output_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);

			if (out == -1) {
				perror("output redirection error");
				*status = 1;
				exit(EXIT_FAILURE);
			}
			dup2(out, 1);
		}
		/* 
		 * This if statement checks the global variable 'sigstp_flg' to see if
		 * the shell is in forground only mode or not.  Depending on how the 
		 * flag is set, this statement decides how to redirect stdin and out
		 */
		if (c->is_background_process == true && sigstp_flag == 0) {
			if (c->input_redirection == false) {
				//set redirection to /dev/null
				inp = open("/dev/null", O_RDONLY);
				if (inp == -1) {
					perror("dev null");
					exit(EXIT_FAILURE);
				}
				dup2(inp, 0);
			}
			if (c->output_redirection == false) {
				// set redirection to /dev/null
				out = open("/dev/null", O_WRONLY);
				if (out == -1) {
					perror("dev null");
					exit(EXIT_FAILURE);
				}
				dup2(out, 1);
			}
		}	
		//exec()
		exec_status = execvp(c->arguments->strings[0], c->arguments->strings);
		//check if exec() succeeded or failed
		if (exec_status == -1) {
			perror("exec");
			*status = 1;
			//free memory before returning to parent
			free_command(c);
			free_PIDList(pids);
			exit(EXIT_FAILURE);
		}
	}
	
	// parent
	else {
		if (c->is_background_process == true && sigstp_flag == 0) {
			//immediately print the background process ID
			printf("process ID: %d\n", (int) child_pid);
			fflush(stdout);

			//dont wait for the child to finish termination
			w = waitpid(child_pid, &wstatus, WNOHANG);
			//add the PID to the background PID array
			add_PIDList(pids, (int) child_pid);
			
		}
		else {
			//wait for the child to finish termination
			w = waitpid(child_pid, &wstatus, 0);
			//check if the child_proc matches what waitpid returned
			if (w == child_pid) {
				//see if it exited normally, then update status
				if (WIFEXITED(wstatus)){
					*status = WEXITSTATUS(wstatus);
				}
				//see if it exited abnormally by signal, then update status
				else if (WIFSIGNALED(wstatus)) {
					*status = WTERMSIG(wstatus);
					printf("\nchild terminated by signal: %d\n", *status);
					fflush(stdout);
				}
			
			}
		}
	}
}

/* Name: run_sh()
 * Desc: handles shell loop.  Puts everything together, runs until user 
 * specified "exit" command
 */
void run_sh() {
	struct sigaction SIGINT_action = {0};
	struct sigaction SIGSTOP_action = {0};

	bool running = true;	
	//list of parent's child processes
	int status = 0;
	PIDLIST child_procs;
  
  	//for signal handling (from module 5 example)
	SIGSTOP_action.sa_handler = sigstp_handler;
	sigfillset(&SIGSTOP_action.sa_mask);
	SIGSTOP_action.sa_flags = SA_RESTART;

	SIGINT_action.sa_handler = ignore_sig_int;
   sigfillset(&SIGINT_action.sa_mask);
   SIGINT_action.sa_flags = SA_RESTART;
	
	sigaction(SIGINT, &SIGINT_action, NULL);
	sigaction(SIGTSTP, &SIGSTOP_action, NULL);
	
	//initialize the list of child processes
	init_PIDList(&child_procs);

	//This loop runs the shell until the user enters the exit command
	while(running) {
		//initialize an empty Command struct	
		COMMAND command_line;
      init_command(&command_line);
      
		//accept a command line up to 2048 characters
		char* buffer = NULL;
      size_t n = 2048;
      
		//print prompts
		printf(": ");
		fflush(stdout);
      
		//check for valid input and getline errors (in case of signal interrupts)
		n = getline(&buffer, &n, stdin);
		while (n < 2) {
			printf(": ");
			fflush(stdout);
			n = getline(&buffer, &n, stdin);
		}
		if (n == -1) {
			clearerr(stdin);
		}
		//if all is good, execute the command
		else {
     		//remove newline character from getline() buffer, then parse
			remove_newline(buffer);
			parse_command(&command_line, buffer);
		
			if(command_line.is_builtin == true) {
				handle_builtin(command_line.builtin, &status, &running);
			}
			else if (command_line.is_comment == true) {
				printf("%s\n", command_line.CL_string);
				fflush(stdout);
			}
    		else {
				exec_command(&command_line, &child_procs, &status, &SIGSTOP_action);
			}
		}
		//cleanup alloc'd memory from the last command
		zombie_cleanup(&child_procs);
	 	free(buffer);	
		free_command(&command_line);
	}
	//clean up children after exit command
	force_kill_zombies(&child_procs);
	zombie_cleanup(&child_procs);

	//free the list of child processes once they are killed off	
	free_PIDList(&child_procs);
}

int main() {
    run_sh();
    return 0;
}
