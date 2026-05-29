#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

void execute(char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        // child process
        if (execvp(args[0], args) == -1) {
            perror("error");
        }
        exit(1);
    } else {
        // parent waits
        wait(NULL);
    }
}

int main() {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];

    while (1) {
        printf("myshell> ");
        fflush(stdout);

        if (!fgets(input, MAX_INPUT, stdin)) break;

        input[strcspn(input, "\n")] = 0; // remove newline

        if (strcmp(input, "exit") == 0) break;

        // split input into args
        int i = 0;
        args[i] = strtok(input, " ");
        while (args[i] != NULL) {
            args[++i] = strtok(NULL, " ");
        }

        if (args[0] != NULL) {
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            chdir(getenv("HOME"));
        } else {
            if (chdir(args[1]) != 0) {
                perror("cd");
            }
        }
    } else {
        execute(args);
    }
};
    }

    return 0;
}
