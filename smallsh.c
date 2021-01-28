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

void reap_exited_children(DYNARR*);
void reap_children(DYNARR*);
void run_foreground(COMMAND*, DYNARR*);
void run_background(COMMAND*, DYNARR*);
void ignore_sig_int(int);

void ignore_sig_int(int sig_number) {
    write(1, "\n: ", 3);
    signal(SIGINT, ignore_sig_int);
}

void reap_exited_children(DYNARR* d) {
    int w_status;
    for (int i = 0; i < d->count; ++i) {
        waitpid(d->array[1], &w_status, WNOHANG);
        if ((WIFEXITED(w_status) == 1)) {
            waitpid(d->array[i], &w_status, 0);
            remove_array(d, d->array[i]);
            if (WIFSIGNALED(w_status) == 1) {
                printf("child terminated by signal: %d\n", WTERMSIG(w_status));
                fflush(stdout);
            } 
        }
    }
}

void run_foreground(COMMAND* c, DYNARR* d) {
    int input_fd, output_fd, exit_status;
    
    pid_t child_pid = fork();
    
    if (child_pid == -1) {
        perror("fork failed");
        fflush(stderr);
    }

    if (child_pid == 0) {
        if(c->output_redirection == 1) {
            output_fd = open(c->output_path, O_CREAT | O_WRONLY | O_TRUNC);
            int stdout_cpy = dup(1);
            dup2(output_fd, 1);
            close(output_fd);
            dup2(stdout_cpy, 1);
            close(stdout_cpy);
        }

        if (c->input_redirection == 1) {
            if ((input_fd = open(c->input_path, O_RDONLY)) == -1) {
                perror("failed to redirect stdin");
                fflush(stderr);
                return;
            }
            else {            
                dup2(input_fd, 0);
                close(input_fd);
            }
        }

        if (execvp(c->arguments[0], c->arguments) == -1) {
            perror("exec() failure");
            fflush(stderr);
        }
    }
    else {
        wait(&exit_status);

        if (WIFSIGNALED(exit_status)) {
            printf("child terminated by signal: %d\n", WTERMSIG(exit_status));
            fflush(stdout);
        } 
    }
}

void run_background(COMMAND* c, DYNARR* d) {
    int input_fd, output_fd;

    int child_pid = fork();

    int w_status;
    //add_array(d, child_pid);
    
    if (child_pid == 1) {
        perror("fork failed");
        fflush(stderr);
    }

    if (child_pid == 0) {
        if(c->output_redirection == 1) {
            output_fd = open(c->output_path, O_CREAT | O_WRONLY | O_TRUNC);
            int stdout_cpy = dup(1);
            dup2(output_fd, 1);
            close(output_fd);
            dup2(1, stdout_cpy);
            close(stdout_cpy);
        }
        else {
            output_fd = open("/dev/null", O_WRONLY);
            dup2(output_fd, 1);
            close(output_fd);
        }

        if (c->input_redirection == 1) {
            if ((input_fd = open(c->input_path, O_RDONLY)) == -1) {
                perror("failed to redirect stdin");
                fflush(stderr);
                return;
            }
            else {            
                dup2(input_fd, 0);
                close(input_fd);
            }
        }
        if(execvp(c->arguments[0], c->arguments) == -1) {
            printf("'%s'\n", c->input_string);
            fflush(stdout);
            perror("exec() failure");
            fflush(stderr);
        }
    }
    else {
        waitpid(child_pid, &w_status, WNOHANG);
        add_array(d, child_pid);
    }
}

void run_sh() {
    DYNARR background_pids;
    
    init_dynamic_array(&background_pids);
    struct sigaction SIGINT_action = {0};

    SIGINT_action.sa_handler = ignore_sig_int;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = 0;


    while(1) {
        signal(SIGINT, ignore_sig_int);
        COMMAND command_line;

        //pid_t child_pid;
        int reap = 0;

        Command(&command_line);

        //reap_exited_children(&background_pids);

        char* buffer = NULL;
        size_t n = 2048;
        
        write(1, ": ", 2);
        fflush(stdout);
        
        getline(&buffer, &n, stdin);
        load_command(&command_line, buffer);

        if (strcmp(command_line.input_string, "exit") == 0) {
            exit(0);
        }
        else if (command_line.is_background_process == 0 && (strcmp(command_line.input_string, "") != 0)) {
            run_foreground(&command_line, &background_pids);
        }
        else if (command_line.is_background_process == 1 && (strcmp(command_line.input_string, "") != 0)) {
            run_background(&command_line, &background_pids);
            reap = 1;
        }
        free_command(&command_line);
        if (reap) {
            reap_exited_children(&background_pids);
        }
        //free_command(&command_line);
    }
}

void test_loop() {
    COMMAND c;
    Command(&c);
    char* buffer = NULL;
    size_t n = 2048;
    printf("enter a string: ");
    getline(&buffer, &n, stdin);

    //expand_pid(buffer);
    load_command(&c, buffer);

    for (int i = 0; i < c.command_count; ++i) {
        printf("%s\n", c.arguments[i]);
    }

    printf("Is comment?: %d\n", c.is_comment);
    printf("Is background proc?: %d\n", c.is_background_process);
    printf("Input redirection?: %d\n", c.input_redirection);
    printf("output redirection?: %d\n", c.output_redirection);
    printf("Input path: %s\n", c.input_path);
    printf("output path: %s\n", c.output_path);

    printf("#####################################################\n");
    
    /*
    DYNARR d;
    init_dynamic_array(&d);

    add_array(&d, 10);
    add_array(&d, 9);


    for (int i = 0; i < d.count; ++i) {
        printf("arr[%d]: %d\n", i, d.array[i]);
    }

    printf("\nRemoving 9!\n");
    remove_array(&d, 9);

    for (int i = 0; i < d.count; ++i) {
        printf("arr[%d]: %d\n", i, d.array[i]);
    }

    printf("\nRemoving 10!\n");

    remove_array(&d, 10);
    for (int i = 0; i < d.count; ++i) {
        printf("arr[%d]: %d\n", i, d.array[i]);
    }
    */
    free(buffer);
    free_command(&c);
}

int main() {
    //signal(SIGINT, ignore_sig_int);
    //test_loop();
    run_sh();
    return 0;
}
