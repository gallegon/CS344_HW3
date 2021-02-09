/* Name: Nicholai Gallegos
 * File: Builtin.h
 * Date: 2/8/2021
 */ 
#include "Builtin.h"

/* Name: init_builtin
 * Desc: initializes a BUILTIN type
 */
void init_builtin(BUILTIN* b) {
	b->builtin_type = NULL;
	b->cd_path = NULL;
}

/* Name: cd_smallsh
 * Desc: changes the current director to a string specified by BUILTIN
 * member cd_path
 */
void cd_smallsh(BUILTIN* b) {
	/* if there is no path specified go to home */
	if (b->cd_path == NULL) {
			
		chdir(getenv("HOME"));
	}
	/* otherwise if a path is specified, go to that path */
	else {
		int dir = chdir(b->cd_path);
		if (dir == -1) {
			perror("cd error:");				
		}
	}
	/* print the current directory to the user */
	char curr_dir[256];
	printf("current directory: %s\n", getcwd(curr_dir, 256));

}

/* Name: status_smallsh
 * Desc: prints the int pinted to by the status parameter to the screen.
 */
void status_smallsh(int* status) {
	printf("exit value: %d\n", *status);
}

/* Name: exit_smallsh
 * Desc: changes a boolean variable pointed to by running to false.
 */
void exit_smallsh(bool* running) {
	*running = false;	
}

/* Name: handle_builtin
 * Desc: Takes a builtin struct and decides what to do with it.
 */
void handle_builtin(BUILTIN* b, int* status, bool* running) {
	if (strcmp(b->builtin_type, "cd") == 0) {
		cd_smallsh(b);
	}
	else if (strcmp(b->builtin_type, "status") == 0) {
		status_smallsh(status);
	}
	else {
		exit_smallsh(running);	
	}
}

/* Name: parse_builtin
 * Desc: takes a string input and parses it into a builtin struct.
 */
void parse_builtin(BUILTIN* b, char* input) {
	//temp holds a copy of input
	char* temp = (char*) malloc(sizeof(char) * (strlen(input) + 1));
	strcpy(temp, input);

	//for strstr
	char* location;
	
	/* check if cd builtin */
	if ((location = strstr(temp, "cd")) != NULL) {
		b->builtin_type = (char*) malloc(sizeof(char) * 3);
		strcpy(b->builtin_type, "cd");
			
			/* check if cd has a path */
			if (strlen(input) > 3) {
			
				location += 3;
				b->cd_path = (char*) malloc(sizeof(char) * (strlen(location) + 1));
				strcpy(b->cd_path, location);
			}
			
			/* if no path found, the cd by itself */
			else {
				b->cd_path = NULL;
			}
			
		}
	/*check for status */
	else if ((location = strstr(temp, "status")) != NULL) {
		b->builtin_type = (char*) malloc(sizeof(char) * 7);		
		strcpy(b->builtin_type, "status");
		b->cd_path = NULL;
	}
	/* check for temp */
	else if ((location = strstr(temp, "exit")) != NULL) {
		b->builtin_type = (char*) malloc(sizeof(char) * 5);
		strcpy(b->builtin_type, "exit");
		b->cd_path = NULL;
	}
	/*free string*/
	free(temp);
}

/* Name: free_builtin
 * Desc: frees strings malloc'd in the builtin struct
 */
void free_builtin(BUILTIN* b) {
	if (b->builtin_type != NULL) {
		free(b->builtin_type);
	}

	if (b->cd_path != NULL) {
		free(b->cd_path);
	}
}
