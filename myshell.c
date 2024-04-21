#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h> 

int main() {
    while(1){
        char input[500];
        fgets(input, sizeof(input), stdin);
    
        // remove the newline character from the input
        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '\n') {
            input[len-1] = '\0';
        }
    
        char delim[] = "|";
        char *token;
        char **commands[10];
        int count = 0;
        char *p1;
        
        // Tokenizing logic //
        
        //parse input initially with "|"
        token = strtok_r(input, delim, &p1);
        printf("starting token: %s\n", token);
        while (token != NULL) {
            //create args array variable and a counter that keeps track of them
            char **args = malloc(10 * sizeof(char *));  // max 10 args per comment
            int arg_count = 0;
    
            // Tokenize each command based on spaces and redirection characters
            char *start = token;
            printf("starting start: %s\n", start);
            for (char *ptr = token; *ptr; ptr++) {
                printf("for loop for ptr: %s\n", ptr);
                if (*ptr == ' ') {
                    printf("case 1\n");
                    if (ptr != start && *(ptr-1) != '\0') {
                        printf("1.1 ptr: %s\n", ptr);
                        *ptr = '\0';
                        printf("1.2 ptr: %s\n", ptr);
                        printf("1.3 old arg_count: %d, start before adding to args: %s\n", arg_count, start);
                        args[arg_count++] = start;
                        printf("1.4 new arg_count: %d args: %s\n", arg_count -1, args[arg_count -1]);
                    }
                    printf("1.5 case 1 outside of if || ptr: %s\n", ptr);
                    start = ptr + 1;
                    printf("1.6 end of case 1 || start: %s\n", start);
                } else if (*ptr == '<' || *ptr == '>') {
                    printf("case 2\n");
                    char fix[2];
                    fix[1] = '\0';  // Null-terminate the string
                    if (*ptr == '<') {
                        fix[0] = '<';
                    } else if (*ptr == '>') {
                        fix[0] = '>';
                    }
                    if (ptr != start && *(ptr-1) != '\0') {
                        printf("2.1 ptr = %s\n", ptr);
                        *ptr = '\0';
                        printf("2.2 ptr = %s\n", ptr);
                        printf("2.3 start = %s\n", start);
                        args[arg_count++] = start;
                        printf("2.3 arg_counts: %s\n", args[arg_count - 1]);
                    }
                    args[arg_count++] = fix;  // Add the redirection character to args
                    printf("2.4 arg_count: %d args: %s\n", arg_count, args[arg_count]);
                    *ptr = '\0';
                    printf("2.5 ptr: %s\n", ptr);
                    while (*(ptr+1) == ' ') ptr++;
                    start = ptr + 1;
                    printf("2.7 start %s\n", start);
                }
            }
            if (*start) {
                printf("case 3\nstart: %s, arg_count: %d, args: %s\n", start, arg_count, args[arg_count]);
                args[arg_count++] = start;
            }
            printf("4, arg_count: %d, args: %s\n", arg_count, args[arg_count]);
            args[arg_count] = NULL;  // append null to arguments list, needed for execvp
    
            //printf("5 count: %d, args: %s", count, args);
            commands[count] = args;
            //printf("6 commands[count]: %s\n", commands[count]);
            count++;
            
            token = strtok_r(NULL, delim, &p1);
        }

        // Print the commands and their arguments
        for (int i = 0; i < count; i++) {
            printf("Command %d:\n", i+1);
            int j = 0;
            while (commands[i][j] != NULL) {
                printf("  Arg %d: '%s'\n", j+1, commands[i][j]);
                j++;
            }
        }
        
        int fd[2], prevFD = -2;
    
        for (int i = 0; i < count; ++i) {
            pipe(fd); 
            pid_t pid = fork(); 
            fflush(stdout);
            
            if (pid == 0) {
                // Child process//
                
                //first command
                if (i == 0) {
                    for (int j = 0; commands[i][j] != NULL; j++) {
                        if (strcmp(commands[i][j], "<") == 0) {
                            int fdFileIn = open(commands[i][j+1], O_RDONLY);
                            if (fdFileIn == -1) {
                                perror("open");
                                exit(EXIT_FAILURE);
                            }
                            dup2(fdFileIn, STDIN_FILENO);
                            close(fdFileIn);
                            commands[i][j] = NULL;  // end command early by replacing "<" with NULL -- rest of command isnt read
                            break;
                        }
                    }
                }
                
                //last command
                if (i == count - 1) {
                    for (int j = 0; commands[i][j] != NULL; j++) {
                        if (strcmp(commands[i][j], ">") == 0) {
                            int fdFileOut = open(commands[i][j+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                            if (fdFileOut == -1) {
                                perror("open");
                                exit(EXIT_FAILURE);
                            }
                            dup2(fdFileOut, STDOUT_FILENO);
                            close(fdFileOut);
                            commands[i][j] = NULL; 
                            break;
                        }
                    }
                }
                
                //not first
                if (i > 0) { 
                    dup2(prevFD, STDIN_FILENO);  // set stdin to the read end of the previous pipe
                    close(prevFD); //close 
                }
                
                //not last
                if (i < count - 1) {  
                    //dup2 fd[1] (write) to stdout, close fd[1]
                    dup2(fd[1], STDOUT_FILENO);  
                    close(fd[1]);  
                }
                
                close(fd[0]); //will have closed all fd's now
                execvp(commands[i][0], commands[i]);
                perror("execvp");
                exit(EXIT_FAILURE);

            } else if (pid > 0) {  
                // Parent process // 
                
                //first command
                if (i > 0) {
                    close(prevFD);  // close the read end of the previous pipe
                }
                
                prevFD = fd[0];  // save the read end of the current pipe for the next i
                close(fd[1]);  // close the write end of the current pipe
            } else {
                perror("fork");
                exit(EXIT_FAILURE);
            }
        }
    
        // wait for all child processes to complete
        for (int i = 0; i < count; ++i) {
            wait(NULL);
        }
    
        // free the allocated memory
        for (int i = 0; i < count; i++) {
            free(commands[i]);
        }
    }
    return 0;
}