#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    char previous[100];
    while (1) {
        printf("Enter shell command :");
        char input[100];
        fgets(input, 100, stdin);
        if (strstr(input, "exit") != NULL) break;
        if (input[0] == '!') strcpy(input, previous);
        else strcpy(previous, input);
        int piped = 0;
        int concurrent = 0;

        for (int i = 0; input[i] != '\0'; i++)
            if (input[i] == '|')
                piped = 1;
            else if (input[i] == '&')
                concurrent = 1;

        char formatted_input[100];
        int increment = 0;
        int count = 1;
        int firstcount = 1;
        int secondcount = 0;

        for (int i = 0; input[i] != '\0'; i++)
            if (input[i] == ' ') {
                if (count == 1 && input[i+1] != '|')
                    firstcount++;
                else if (count == 2) secondcount++;

                if (input[i-1] != '|')
                    formatted_input[increment++] = '\0';
            } else if (input[i] != '|' && input[i] != '&' && input[i] != '\n')
                formatted_input[increment++] = input[i];
            else if (input[i] == '|')
                count = 2;
        
        formatted_input[increment] = '\0';

        if (piped == 0) {
            char *arguments[firstcount];
            int increment2 = 0;
            for (int i = 0; i < increment; i++)
                if (i == 0)
                    arguments[increment2++] = &formatted_input[i];
                else if (formatted_input[i-1] == '\0')
                    arguments[increment2++] = &formatted_input[i];
            arguments[increment2] = NULL;

            pid_t pid = fork();
            if (pid == 0) {
                execvp(arguments[0], arguments);
            }
            
            if (concurrent == 0)
                waitpid(pid, NULL, 0);
        } else {
            char *arguments1[firstcount+1];
            char *arguments2[secondcount+1];
            int increment2 = 0;
            for (int i = 0; i < increment; i++)
                if (firstcount > 0) {
                    if (i == 0 || formatted_input[i-1] == '\0') {
                        arguments1[increment2++] = &formatted_input[i];
                        firstcount--;
                    }
                    
                    if (firstcount == 0) {
                        arguments1[increment2] = NULL;
                        increment2 = 0;
                    }
                } else if (formatted_input[i-1] == '\0') {
                    arguments2[increment2++] = &formatted_input[i];
                }

            arguments2[increment2] = NULL;

            int ends[2];
            pipe(ends);

            pid_t pid1 = fork();
            if (pid1 == 0) {
                dup2(ends[1], STDOUT_FILENO);
                execvp(arguments1[0], arguments1);
            }

            close(ends[1]);

            pid_t pid2 = fork();
            if (pid2 == 0) {
                dup2(ends[0], STDIN_FILENO);
                execvp(arguments2[0], arguments2);
            }

            if (concurrent == 0) {
                waitpid(pid1, NULL, 0);
                waitpid(pid2, NULL, 0);
            }
        }
    }
}
