/* Name: Nicholai Gallegos
 * File: Command.c
 * Date: 2/8/2021
 */
#include "Command.h"

#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	<unistd.h>

/* Name: init_command
 * Desc: Initializes a command by setting strings to NULL, and allocation memory 
 * the additional structs inside
 */
void init_command(COMMAND* c) {
	c->CL_string = NULL;
	c->input_path = NULL;
	c->output_path = NULL;
	/* allocate memory for the string array and the builtin struct */
	c->arguments = malloc(sizeof(DYNARR));
	c->builtin = malloc(sizeof(BUILTIN));

	// assume no special attributes 
	c->is_builtin = false;
	c->is_background_process = false;
	c->is_comment = false;
	c->input_redirection = false;
	c->output_redirection = false;
	
	c->command_count = 0;
	
	/* initialize string array and builtin struct */
	init_dynamic_array(c->arguments, INITIAL_CL_SIZE);
	init_builtin(c->builtin);
}

/*
 * Name: parse_command
 * Desc: parses a string into a Command struct with associated infomation
 */
void parse_command(COMMAND* c, char* CLI_input) {
	/* create a copy of the "command line" input */
	char* temp = (char*) malloc(sizeof(char) * (strlen(CLI_input) + 1));
	/* initialize temp to be an empty string */
	memset(temp, '\0', (strlen(CLI_input) + 1) * sizeof(char));
	
	strcpy (temp, CLI_input);
	
	/* check if the input string for a builtin command, if it is don't parse
	 * the command into arguments the non-builtin way, just set the relevant
	 * builtin data */
	if (check_builtin(c, temp)) {
		parse_builtin(c->builtin, temp);
	}
	/* if not a builtin, then parse the string so it can be more easily be 
	 * interpreted as a command in smallsh.c */
	else {
		load_args(c, CLI_input);			
	}
	/* free alloc'd memory for temp */
	free(temp);
}

/* Name: load_args
 * Desc: Load args is called as a helper function to parse_command.  This
 * function parse the command into arguments, sets redirection if any, checks
 * if it is a comment.  This is only called after parse_command() has 
 * determined the command line passed is not for a builtin command 
 */
void load_args(COMMAND* c, char* input) {
	// temp holds a copy of the input parameter for use with strtok
	char* temp = (char*) malloc(sizeof(char) * (strlen(input) + 1));
	c->CL_string = (char*) malloc(sizeof(char) * (strlen(input) + 1));
	strcpy(temp, input);
	strcpy(c->CL_string, input);
	
	
	// assume that there is no redirection initially
	c->input_redirection = false;
	c->output_redirection = false;	
	
	//check if the command is a comment, if so exit the function and do nothing
	if (input[0] == '#') {
		c->is_comment = true;
		free(temp);
	}
	else {
	   /* this uses some pointer math to check for the '&' symbol at the 
		 * end of the line, this is to set the is_background_process 
		 * variable in the Command struct */
		if ((temp[strlen(temp) - 1] == '&') && (temp[strlen(temp) - 2] == ' ')) {
			c->is_background_process = true;
			temp[strlen(temp) - 1] = '\0';
		}

		/* tokenize the string input into arguments, look specifically for input
		 * and output redirection sybols, then set those paths accordingly */
		char* token = strtok(temp, " ");
	
		while(token != NULL) {
			if (strcmp(token, ">") == 0) {
				c->output_redirection = true; //set output redirection	
				token = strtok(NULL, " ");
				
				c->output_path = (char*) malloc((strlen(token) + 1) * sizeof(char));
				strcpy(c->output_path, token);
			}
			else if (strcmp(token, "<") == 0) {
				c->input_redirection= true; //set input redirection
				token = strtok(NULL, " ");
				
				c->input_path = (char*) malloc((strlen(token) + 1) * sizeof(char));
				strcpy(c->input_path, token);
			}
			else {
				add_array(c->arguments, token);	
				c->command_count++;
			}
			token = strtok(NULL, " ");
		}
		/* free temp string copy */
		free(temp);
	}
}

/* Name: check_builtin
 * Desc: This function takes a pointer to a command and determines if the
 * command is a builtin.  Returns true if it is a builting, returns false
 * otherwise.
 */
bool check_builtin(COMMAND* c, char* input) {
	char* temp = (char*) malloc((strlen(input) + 1) * sizeof(char));
	strcpy(temp, input);

	/* check for the substrings "cd", "status", or "exit" in the command this
	 * assumes valid syntax--meaning the builtin command must be the first
	 * argument */
	if (strstr(temp, "cd") != NULL) {
		c->is_builtin = true;
		free(temp);
		return true;
	}
	else if (strstr(temp, "status") != NULL) {
		c->is_builtin = true;
		free(temp);
		return true;
	}
	else if (strstr(temp, "exit") != NULL) {
		c->is_builtin = true;
		free(temp);
		return true;
	}
	else {
		c->is_builtin = false;
		free(temp);
		return false;
	}
}

/* Name: expand_pid
 * Desc: takes a string as input, searchs for a substring "$$", then replaces
 * it with the process ID of the calling process at each occurance of the 
 * substring
 */
void expand_pid(char* input) {
	char* location; //points to substring "$$"
	char* end; //points to the rest of the string after the substring

	char* pid_string = (char*) calloc(7, sizeof(char)); //holds process ID
	/* temp strings to use to concatenate strings later */
	char* temp1 = (char*) calloc(2048, sizeof(char));
	char* temp2 = (char*) calloc(2048, sizeof(char));

	/* print the integer to a string */	
	int pID = (int) getpid();
	sprintf(pid_string, "%d", pID);

	/* uses pointer arithmetic to replace the pid string at every occurance
	 * of "$$" */
	do {
		/* look for substring */
		location = strstr(input, "$$");
		
		/* if the substring is found, clear it with null character */
		if (location != NULL) {
			memset(location, '\0', 2 * sizeof(char));
			
			strcpy(temp1, input);
			
			/* concatenate strings for variable expansion */
			if ((location + 2) != NULL) {
				end = location + 2;
				strcpy(temp2, end);

				strcat(temp1, pid_string);
				strcat(temp1, temp2);
			
			}
			else {
				strcat(temp1, pid_string);
			}
			
		memset(input, '\0', 2048 * sizeof(char));
		strcpy(input, temp1);
		
		memset(temp1, '\0', 2048 * sizeof(char));
		memset(temp2, '\0', 2048 * sizeof(char));
		
		}
	} while (location != NULL);
	
	/* free the strings used */
	free(pid_string);
	free(temp1);
	free(temp2);
}

/* Name: remove_newline
 * Desc: removes a newline character from a string.  This is to be called
 * after getline() so there is no extraneous newline character when parsing
 * a command line */
void remove_newline(char* input) {
	char* newline_location;
	/* find newline character and replace it with a null terminator */
	if ((newline_location = strstr(input, "\n")) != NULL) {
		*newline_location = '\0';
	}
}

/* Name: free_command
 * Desc: frees all memory malloc'd for a Command struct. Checks if memory
 * needs to be freed */
void free_command(COMMAND* c) {
	/* check strings to avoid freeing unalloc'd memory */
	if(c->CL_string != NULL)
		free(c->CL_string);
	if(c->input_path != NULL)
		free(c->input_path);
	if(c->output_path != NULL)
		free(c->output_path);

	/* free the two structs inside of Command */
	free_dynamic_array(c->arguments);
	free_builtin(c->builtin);

	free(c->arguments);
	free(c->builtin);

}

