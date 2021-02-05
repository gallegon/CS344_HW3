#include "Command.h"
//#include "DynamicArray.h"
//#include "Builtin.h"

#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	<unistd.h>


void init_command(COMMAND* c) {
	c->CL_string = NULL;
	c->input_path = NULL;
	c->output_path = NULL;
	c->arguments = malloc(sizeof(DYNARR));
	c->builtin = malloc(sizeof(BUILTIN));

	// assume no special attributes 
	c->is_builtin = false;
	c->is_background_process = false;
	c->is_comment = false;
	c->input_redirection = false;
	c->output_redirection = false;
	
	c->command_count = 0;
	
	init_dynamic_array(c->arguments, INITIAL_CL_SIZE);
	init_builtin(c->builtin);
}


void parse_command(COMMAND* c, char* CLI_input) {
	char* temp = (char*) malloc(sizeof(char) * (strlen(CLI_input) + 1));
	
	memset(temp, '\0', (strlen(CLI_input) + 1) * sizeof(char));
	
	strcpy (temp, CLI_input);
	
	if (check_builtin(c, temp)) {
		parse_builtin(c->builtin, temp);

		//printf("builtin: %s\ncd_path: %s\n", c->builtin->builtin_type, c->builtin->cd_path);
		//fflush(stdout);
	}
	else {
		load_args(c, CLI_input);			
	}
}


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
		//printf("%s\n", input);	
		free(temp);
	}
	else {
		if ((temp[strlen(temp) - 1] == '&') && (temp[strlen(temp) - 2] == ' ')) {
			c->is_background_process = true;
			temp[strlen(temp) - 1] = '\0';
		}

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

		free(temp);
	}
}


bool check_builtin(COMMAND* c, char* input) {
	char* temp = (char*) malloc((strlen(input) + 1) * sizeof(char));
	strcpy(temp, input);

		
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


void expand_pid(char* input) {
	char* location; //points to substring "$$"
	char* end; //points to the rest of the string after the substring

	char* pid_string = (char*) calloc(6, sizeof(char)); //holds process ID
	char* temp1 = (char*) calloc(2048, sizeof(char));
	char* temp2 = (char*) calloc(2048, sizeof(char));


	int pID = (int) getpid();
	sprintf(pid_string, "%d", pID);

	
	do {
		location = strstr(input, "$$");
		
		if (location != NULL) {
			memset(location, '\0', 2 * sizeof(char));
			
			strcpy(temp1, input);
			
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

	
	free(pid_string);
	free(temp1);
	free(temp2);
}


void remove_newline(char* input) {
	char* newline_location;
	if ((newline_location = strstr(input, "\n")) != NULL) {
		*newline_location = '\0';
	}
}


void free_command(COMMAND* c) {
	if(c->CL_string != NULL)
		free(c->CL_string);
	if(c->input_path != NULL)
		free(c->input_path);
	if(c->output_path != NULL)
		free(c->output_path);
	free_dynamic_array(c->arguments);
	free_builtin(c->builtin);

	free(c->arguments);
	free(c->builtin);

}
//for testing

void print_command(COMMAND* c) {
	if (c->is_comment == true) {
		printf("%s\n", c->CL_string);	
		fflush(stdout);	
	}
	else {
		if (c->command_count > 0) {
			printf("number of arguments: %d\n", c->command_count); 
			fflush(stdout);	
			printf("----Arguments----\n");
			fflush(stdout);	
			
			for (int i = 0; i < c->command_count; ++i) {
				printf("%d. %s\n", i + 1, c->arguments->strings[i]);
				fflush(stdout);	
			}
			
			if(c->is_builtin == true) {
				printf("builtin\n");
				fflush(stdout);	
			}
			
			if(c->is_background_process == true) { 
				printf("background process: true\n");
				fflush(stdout);
			}
			else {
				printf("background process: false\n");
				fflush(stdout);	
			}
			if(c->input_redirection == true) {
				printf("input redirection: true\n");
				fflush(stdout);	
				printf("input file: %s\n", c->input_path);
				fflush(stdout);	
			}
			else {
				printf("input redirection: false\n");
				fflush(stdout);	
			}
			if(c->output_redirection == true) {
				printf("output redirection: true\n");
				fflush(stdout);	
				printf("output file: %s\n", c->output_path);
				fflush(stdout);	
			}
			else {
				printf("output redirection: false\n");
				fflush(stdout);
			}
		}
	}
}
