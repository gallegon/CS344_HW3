#include "Command.h"

#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	<unistd.h>

void Command(struct Command* c) {
	//c->name = NULL;
	c->input_string = (char*) malloc(sizeof(char) * MAX_CL_LEN);
	c->input_path = (char*) malloc(sizeof(char) * STRING_SIZE);
	c->output_path = (char*) malloc(sizeof(char) * STRING_SIZE);
	c->arguments = (char**) malloc(sizeof(char*) * MAX_ARGS);

	for (int i = 0; i < MAX_ARGS; ++i) {
		c->arguments[i] = (char*) malloc(sizeof(char) * STRING_SIZE);
		memset(c->arguments[i], '\0', STRING_SIZE);
	}

	memset(c->input_string, '\0', MAX_CL_LEN);
	memset(c->input_path, '\0', STRING_SIZE);
	memset(c->output_path, '\0', STRING_SIZE);

	c->is_background_process = 0;
	c->is_comment = 0;
	c->command_count = 0;
	c->input_redirection = 0;
	c->output_redirection = 0;
}

/*#############################################################################
NAME	
	void load_command(struct Command* c, char* input)

DESCRIPTION
	load_command takes a string and stores 
#############################################################################*/
void load_command(struct Command* c, char* input) {
	int count = 0; // count of "commands" to be parsed by strtok

	remove_newline(input);

	strcpy(c->input_string, input);
	if (*input == '#') {
		c->is_comment = 1;
	}
	else {
		char** command_arguments = (char**) malloc(sizeof(char*) * MAX_ARGS);
		expand_pid(input);

		for (int i = 0; i < MAX_ARGS; ++i) {
			command_arguments[i] = (char*) malloc(sizeof(char) * STRING_SIZE);
		}

		char* location = strtok(input, " ");

		while (location != NULL) {
			strcpy(command_arguments[count], location);
			count++;
			location = strtok(NULL, " ");
		}

		c->command_count = count;
		
		if (count != 0) {
			if ((strcmp(command_arguments[count - 1], "&") == 0)) {
				c->is_background_process = 1;
			} 
			else { 
				c->is_background_process = 0; 
			}
		}
		for (int i = 0; i < count; ++i) {
			strcpy(c->arguments[i], command_arguments[i]);
			if (strcmp(command_arguments[i], ">") == 0) {
				if (i == c->command_count - 1) {
					strcpy(c->output_path, "/dev/null");
					c->output_redirection = 1;
					memset(c->arguments[i], '\0', STRING_SIZE);
					c->command_count--; 
				}
				else {
					if (strcmp(command_arguments[i + 1], "&") == 0) {
						strcpy(c->output_path, "/dev/null");
						c->output_redirection = 1;
						memset(c->arguments[i], '\0', STRING_SIZE);
						memset(c->arguments[i + i], '\0', STRING_SIZE);
						c->command_count -= 2; 
					}
					else {
						strcpy(c->output_path, command_arguments[i + 1]);
						c->output_redirection = 1;
						memset(c->arguments[i], '\0', STRING_SIZE);
						memset(c->arguments[i + i], '\0', STRING_SIZE);
						c->command_count -= 2; 
					}
				}
			}
			
			if (strcmp(command_arguments[i], "<") == 0) {
				if (i == c->command_count - 1) {
					strcpy(c->input_path, "/dev/null");
					c->input_redirection = 1;
					memset(c->arguments[i], '\0', STRING_SIZE);
					c->command_count--;	
				}
				else {
					if (strcmp(command_arguments[i + 1], "&") == 0) {
						strcpy(c->input_path, "/dev/null");
						c->input_redirection = 1;
						memset(c->arguments[i], '\0', STRING_SIZE);
						memset(c->arguments[i + i], '\0', STRING_SIZE);
						c->command_count -= 2; 
					}
					else {
						strcpy(c->input_path, command_arguments[i + 1]);
						c->input_redirection = 1;
						memset(c->arguments[i], '\0', STRING_SIZE);
						memset(c->arguments[i + i], '\0', STRING_SIZE);
						c->command_count -= 2; 
					}
				}
			}
		}
		c->arguments[c->command_count] = NULL; //to use with execvp
		for (int i = 0; i < MAX_ARGS; ++i) {
			free(command_arguments[i]);
		}
		free(command_arguments);
	}
}

void expand_pid(char* input) {
	char* location;
	
	char* input_copy = (char*) malloc(sizeof(char) * 2048);
	char* pid_string = (char*) malloc(sizeof(char) * 33);
	
	int pID = (int) getpid();
	sprintf(pid_string, "%d", pID);

	do {
		strcpy(input_copy, input);
		location = strstr(input, "$$");
		if (location != NULL) {
			*location = '\0';
			strcat(input, pid_string);
			location = strstr(input_copy, "$$");
			strcat(input, location + 2);
		}
	} while (location != NULL);

	free(input_copy);
	free(pid_string);

}

void remove_newline(char* input) {
	char* newline_location;
	if ((newline_location = strstr(input, "\n")) != NULL) {
		*newline_location = '\0';
	}
}

void clear_command(struct Command* c) {
	memset(c->input_string, '\0', MAX_CL_LEN);
	memset(c->input_path, '\0', STRING_SIZE);
	memset(c->output_path, '\0', STRING_SIZE);

	for (int i = 0; i < MAX_ARGS; ++i) {
		memset(c->arguments[i], '\0', STRING_SIZE);
	}

	c->is_background_process = 0;
	c->is_comment = 0;
	c->command_count = 0;
	c->input_path = 0;
	c->output_redirection = 0;
}

void free_command(struct Command* c) {
	free(c->input_string);
	free(c->input_path);
	free(c->output_path);

	for (int i = 0; i < MAX_ARGS; ++i) {
		free(c->arguments[i]);
	}

	free(c->arguments);
}
