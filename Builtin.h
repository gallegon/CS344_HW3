/* Name: Nicholai Gallegos
 * File: Builtin.h
 * Date: 2/8/2021
 */
#ifndef __BUILTIN_H
#define __BUILTIN_H

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/* struct definitions, typedef, function prototypes for struct Builtin */

typedef struct Builtin BUILTIN;

struct Builtin {
	char* builtin_type;
	char* cd_path;
};

void init_builtin(BUILTIN*);

void cd_smallsh(BUILTIN*);
void status_smallsh(int*);
void exit_smallsh(bool* running);

void handle_builtin(BUILTIN*, int*, bool*);
void parse_builtin(BUILTIN*, char*);
void free_builtin(BUILTIN*);

#endif
