// myshell - a simple Unix shell written in C
// supports command execution, cd builtin, pipes, and command history
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

void execute(char **args)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        // child process
        if (execvp(args[0], args) == -1)
        {
            perror("error");
        }
        exit(1);
    }
    else
    {
        // parent waits
        wait(NULL);
    }
}

int main()
{
    char input[MAX_INPUT];
    char *args[MAX_ARGS];
    char history[100][MAX_INPUT];
    int history_count = 0;

    while (1)
    {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        char prompt[2048];

        // prints the current path 
        snprintf(prompt, sizeof(prompt), "%s> ", cwd);
        char *line = readline(prompt);
        if (!line)
            break;
        strncpy(input, line, MAX_INPUT);
        add_history(line);
        free(line);

        // Pipes
        if (strchr(input, '|') != NULL)
        {
            input[strcspn(input, "\n")] = 0; // remove newline
            strcpy(history[history_count], input);
            history_count++;

            char *pipe_pos = strchr(input, '|');
            *pipe_pos = '\0';
            char *left = input;
            char *right = pipe_pos + 1;

            int pipefd[2];
            pipe(pipefd);

            // first child - runs left command 
            pid_t pid1 = fork();
            if (pid1 == 0)
            {
                dup2(pipefd[1], STDOUT_FILENO); // redirect stdout to pipe write end
                close(pipefd[0]);
                close(pipefd[1]);

                char *args1[MAX_ARGS];
                int i = 0;
                args1[i] = strtok(left, " ");
                while (args1[i] != NULL)
                    args1[++i] = strtok(NULL, " ");

                execvp(args1[0], args1);
                exit(1);
            }

            // second child - runs right command 
            pid_t pid2 = fork();
            if (pid2 == 0)
            {
                dup2(pipefd[0], STDIN_FILENO); // redirect stdin to pipe read end
                close(pipefd[0]);
                close(pipefd[1]);

                char *args2[MAX_ARGS];
                int i = 0;
                args2[i] = strtok(right, " ");
                while (args2[i] != NULL)
                    args2[++i] = strtok(NULL, " ");

                execvp(args2[0], args2);
                exit(1);
            }

            // parent closes pipe and waits
            close(pipefd[0]);
            close(pipefd[1]);
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        }
        else
        {
            input[strcspn(input, "\n")] = 0; // remove newline

            if (strcmp(input, "exit") == 0)
                break;

            // split input into args
            int i = 0;
            args[i] = strtok(input, " ");
            while (args[i] != NULL)
            {
                args[++i] = strtok(NULL, " ");
            }

            if (args[0] != NULL)
            {
                if (strcmp(args[0], "cd") == 0)
                {
                    if (args[1] == NULL)
                    {
                        chdir(getenv("HOME"));
                    }
                    else
                    {
                        if (chdir(args[1]) != 0)
                        {
                            perror("cd");
                        }
                    }
                }
                else
                {
                    execute(args);
                }
            };
        }
    }

    return 0;
}
