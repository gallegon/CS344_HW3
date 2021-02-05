#ifndef __BUILTIN_H
#define __BUILTIN_H

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Builtin BUILTIN;

struct Builtin {
	char* builtin_type;
	char* cd_path;
};

void init_builtin(BUILTIN*);

void cd_smallsh(BUILTIN*);
void status_smallsh(int*);
void exit_smallsh(/*type not yet defined*/);

void handle_builtin(BUILTIN*, int*);
void parse_builtin(BUILTIN*, char*);
void free_builtin(BUILTIN*);
#endif
