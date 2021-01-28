#ifndef __COMMAND_H
#define __COMMAND_H

#define		STRING_SIZE	33
#define		MAX_ARGS	512
#define 	MAX_CL_LEN	2048

typedef struct Command COMMAND;

struct Command {
	//char* name;
	char* input_string;
	char* input_path;
	char* output_path;
	char** arguments;
  
	int is_background_process;
	int is_comment;
	int command_count;
	int input_redirection;
	int output_redirection;
};

void Command(struct Command*);
void expand_pid(char*);
void load_command(struct Command*, char*);
void remove_newline(char*);
void clear_command(struct Command*);
void free_command(struct Command*);

#endif
