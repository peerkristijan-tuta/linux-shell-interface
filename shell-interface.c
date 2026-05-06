#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>

int main(void) {
    char past_command[50] = {false};
    char* parameters[] = {NULL};

    while (1) {
        bool flags[5] = {false};
        printf("osh> ");
        char input[50];
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        if (strcspn(input, "!") != strlen(input)) {
            flags[0] = true;
            strcpy(input, past_command);
        } else {
            strcpy(past_command, input);
        }

        if (strcspn(input, "&") != strlen(input))
            flags[1] = true;
        if (strcspn(input, ">") != strlen(input))
            flags[2] = true;
        if (strcspn(input, "<") != strlen(input))
            flags[3] = true;
        if (strcspn(input, "|") != strlen(input))
            flags[4] = true;

        if (flags[2]) {
            if (flags[1])
                input[strcspn(input, "&") - 1] = 0;

            char file[15];
            int file_increment = 0;
            for (int x = (strcspn(input, ">") + 1); x < strlen(input); x++)
                if (input[x] == ' ')
                    continue;
                else
                    file[file_increment++] = input[x];
            file[file_increment] = 0;

            char command[15];
            int command_increment = 0;
            for (int x = 0; x < strcspn(input, ">"); x++)
                if (input[x] == ' ')
                    continue;
                else
                    command[command_increment++] = input[x];

            command[command_increment] = 0;

            pid_t process = fork();
            if (process == 0) {
                int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                dup2(fd, STDOUT_FILENO);
                close(fd);
                execvp(command, parameters);
            }
            if (!flags[1])
                waitpid(process, NULL, 0);
        } else if (flags[3]) {
            if (flags[1])
                input[strcspn(input, "&") - 1] = 0;

            char command[15];
            int command_increment = 0;
            for (int x = 0; x < strcspn(input, "<"); x++)
                if (input[x] == ' ')
                    continue;
                else
                    command[command_increment++] = input[x];
            command[command_increment] = 0;

            char file[15];
            int file_increment = 0;
            for (int x = (strcspn(input, "<") + 1); x < strlen(input); x++)
                if (input[x] == ' ')
                    continue;
                else
                    file[file_increment++] = input[x];
            file[file_increment] = 0;

            pid_t process = fork();
            if (process == 0) {
                int fd = open(file, O_RDONLY);
                dup2(fd, STDIN_FILENO);
                close(fd);
                execvp(command, parameters);
            }

            if (!flags[1])
             waitpid(process, NULL, 0);
        } else if (flags[4]) {
            if (flags[1])
                input[strcspn(input, "&") - 1] = 0;

            int piped[2];
            pipe(piped);

            char left_command[10];
            char right_command[10];
            int command_increment = 0;

            for (int x = 0; x < strcspn(input, "|"); x++)
                if (input[x] == ' ')
                    continue;
                else
                    left_command[command_increment++] = input[x];
            left_command[command_increment] = 0;

            command_increment = 0;

            for (int x = (strcspn(input, "|") + 1); x < strlen(input); x++)
                if (input[x] == ' ' && strlen(input) - x == 1)
                    continue;
                else if (input[x] == ' ' && x == (strcspn(input, "|") + 1))
                    continue;
                else    
                    right_command[command_increment++] = input[x];\
            right_command[command_increment] = 0;

            char *left_args[10];
            int left_argc = 0;
            left_args[left_argc++] = strtok(left_command, " ");
            while ((left_args[left_argc] = strtok(NULL, " ")) != NULL)
                left_argc++;
            
            pid_t left = fork();
            if (left == 0) {
                dup2(piped[1], STDOUT_FILENO);
                close(piped[0]);
                close(piped[1]);
                execvp(left_args[0], left_args);
            } 

            char *right_args[10];
            int right_argc = 0;
            right_args[right_argc++] = strtok(right_command, " ");
            while ((right_args[right_argc] = strtok(NULL, " ")) != NULL)
                right_argc++;

            pid_t right = fork();
            if (right == 0) {
                dup2(piped[0], STDIN_FILENO);
                close(piped[0]);
                close(piped[1]);
                execvp(right_args[0], right_args);
            }

            close(piped[0]);
            close(piped[1]);

            if (!flags[1])
                waitpid(left, NULL, 0);
            if (!flags[1])
                waitpid(right, NULL, 0);
        } else {
            if (!strcmp(input, "exit"))
                return 0;

            if (flags[1])
                input[strcspn(input, "&") - 1] = 0;

            char *args[10];
            int argc = 0;
            args[argc++] = strtok(input, " ");
            while ((args[argc] = strtok(NULL, " ")) != NULL)
                argc++;

            pid_t process = fork();

            if (process == 0)
                execvp(args[0], args);

            if (!flags[1])
                waitpid(process, NULL, 0);
        }
    }
}
