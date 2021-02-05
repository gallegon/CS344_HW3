#ifndef __COMMAND_H
#define __COMMAND_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "DynamicArray.h"
#include "Builtin.h"

#define	STRING_SIZE			33
#define	MAX_ARGS				512
#define	MAX_CL_LEN			2048
#define	INITIAL_CL_SIZE 	32

typedef struct Command COMMAND;

struct Command {
	char* CL_string; // string literal from user's command input
	char* input_path;
	char* output_path;
	
	DYNARR* arguments;
 	BUILTIN* builtin;
 	
  	// boolean values for special attributes of a command
  	bool is_builtin;
	bool is_background_process;
	bool is_comment;
	
	bool input_redirection; 
	bool output_redirection;
	
	int command_count;
};

void init_command(COMMAND*);
void parse_command(COMMAND*, char*);
bool check_builtin(COMMAND* c, char*);
bool check_background_proc(COMMAND* c);
void execute_builtin(char*);

void load_args(COMMAND*, char*);

//Old code
void expand_pid(char*);
void remove_newline(char*);
void free_command(COMMAND*);

void print_command(COMMAND*);
#endif
