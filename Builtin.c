#include "Builtin.h"

void init_builtin(BUILTIN* b) {
	b->builtin_type = NULL;
	b->cd_path = NULL;
}

void cd_smallsh(BUILTIN* b) {
	if (b->cd_path == NULL) {
			
		chdir(getenv("HOME"));
	}
	else {
		int dir = chdir(b->cd_path);
		if (dir == -1) {
			perror("cd error:");				
		}
	}
	char curr_dir[256];
	printf("current directory: %s\n", getcwd(curr_dir, 256));

}

void status_smallsh(int* status) {
	printf("status: %d\n", *status);
}

void handle_builtin(BUILTIN* b, int* status) {
	if (strcmp(b->builtin_type, "cd") == 0) {
		cd_smallsh(b);
	}
	else if (strcmp(b->builtin_type, "status") == 0) {
		status_smallsh(status);
	}
	else {
		//do exit	
	}
}

void parse_builtin(BUILTIN* b, char* input) {
	//temp holds a copy of input
	char* temp = (char*) malloc(sizeof(char) * (strlen(input) + 1));
	strcpy(temp, input);

	//for strstr
	char* location;
	
	if ((location = strstr(temp, "cd")) != NULL) {
		b->builtin_type = (char*) malloc(sizeof(char) * 3);
		strcpy(b->builtin_type, "cd");
			
			if (strlen(input) > 3) {
			
			//if (location + 2 != '\0') {
				location += 3;
				b->cd_path = (char*) malloc(sizeof(char) * (strlen(location) + 1));
				strcpy(b->cd_path, location);
			}
			
			else {
				b->cd_path = NULL;
			}
			
		}
	else if ((location = strstr(temp, "status")) != NULL) {
		b->builtin_type = (char*) malloc(sizeof(char) * 7);		
		strcpy(b->builtin_type, "status");
		b->cd_path = NULL;
	}
	else if ((location = strstr(temp, "exit")) != NULL) {
		b->builtin_type = (char*) malloc(sizeof(char) * 5);
		strcpy(b->builtin_type, "exit");
		b->cd_path = NULL;
	}
	free(temp);
}

void free_builtin(BUILTIN* b) {
	if (b->builtin_type != NULL) {
		free(b->builtin_type);
	}

	if (b->cd_path != NULL) {
		free(b->cd_path);
	}
}
