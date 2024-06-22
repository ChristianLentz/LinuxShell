#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h> 
#include <signal.h>

#define MAX_LEN 1024
#define MAX_TOKENS 100 
pid_t child_PID;

/**
 * Authors: Christian Lentz, Theo Hoyer, Jeff Wang 
 * Reference README for sources 
*/

/**
 * Fork from the parent process to the child process we want to execute and wait 
 * for its PID. (Step 2)
 * @param input the buffer which holds tokenized user input 
*/
pid_t forking(char* input[]) { 

    // fork the process 
    pid_t PID = fork();  
    // handle each case
    if (PID < 0) { 
        // unsuccessful child creation 
        perror("ERROR: unsuccessful fork to child \n"); 
        exit(EXIT_FAILURE); 
    } else if (PID == 0) { 
        // check the child process
        pid_t child_ret = execvp(input[0], input); 
        // error when executing children 
        if (child_ret == -1) { 
            perror("ERROR: error when executing children \n");
            exit(EXIT_FAILURE);  
        }
    } else {
        // we ARE the parent 
        signal(SIGINT, SIG_IGN); 
        pid_t wPID, fork_ret; 
        // parent waits for the PID of child while it has not exited or signaled 
        waitpid(PID, &fork_ret, 0);
    }
    return PID; 
}

/**
 * Parse the user input into tokens using strtok() 
 * @param buffer the buffer holding user input to tokenize 
 * @return an array of pointers to the tokens 
*/
char** parse_input(char buffer[]) { 

    // create arry of tokens and delim to split the buffer 
    char* delim = " \t\r\n\a"; 
    char** tokens = malloc(sizeof(char*) * MAX_LEN); 
    char* token;

    // add tokens to the array using strtok
    // make sure the tokens array is null terminated 
    // keep track of how many characters/tokens we have seen 
    token = strtok(buffer, delim);
    int i, total_chars = 0; 
    int curr_tok_size; 
    while (token != NULL) { 
        // get number of characters in current token and add to total 
        curr_tok_size = sizeof(token) / sizeof(char); 
        total_chars += curr_tok_size; 
        // handle user input when it goes beyond max characters 
        if (curr_tok_size > MAX_LEN) { 
            perror("ERROR: input has more than 1024 characters \n"); 
            exit(EXIT_FAILURE); 
        // handle user input when it goes beyond max tokens 
        } else if (i >= MAX_LEN) { 
            perror("ERROR: user input exceeds 100 tokens \n"); 
            exit(EXIT_FAILURE);
        // get tokens  
        } else {
            tokens[i] = token; 
            i++;
            token = strtok(NULL, delim);
        } 
    }
    tokens[i] = NULL; 
    return tokens; 
}

/**
 * Print ID for this process and its parent when 'myinfo' used 
*/
void print_info() { 
    pid_t PID, PPID; 
    PID = getpid(); 
    PPID = getppid(); 
    printf("The PID: %d \n", PID); 
    printf("The PPID: %d \n", PPID); 
}

/**
 * Handle built in commands and conditions when we need to fork
 * @param parsed the parsed user input 
*/
int fork_cons(char** parsed) { 
    int fork_con = 1; 
    // print PIDs when 'myinfo' used 
    if (!(fork_con = strcmp(parsed[0], "myinfo"))){
        print_info(); 
    }
    // handle cd with/without a directory provided 
    if (!(fork_con &= strcmp(parsed[0], "cd"))) { 
        if (parsed[1] != NULL) {  
            chdir(parsed[1]); 
        } else { 
            chdir(getenv("HOME"));
        }
    }
    return fork_con; 
}

/**
 * Terminate child (process) for interrupt signal handling
*/
void kill_child() { 
    kill(child_PID, SIGINT); 
}

/**
 * Main method for the shell program 
*/
int main(int argc, char *argv[]){
    // set conditions for when to fork/exit
    char exit_con = 1; 
    char fork_con; 
    // loop over: request and read user input with command prompt
    char buffer[MAX_LEN];
    while (exit_con) {
        // terminate child process on ctrl-c 
        signal(SIGINT, kill_child);
        // give user command prompt and read input
        printf("%s", "=> ");
        fgets(buffer, MAX_LEN, stdin);
        // parse/tokenize input (when given) and fork to child process
        if (buffer[0] != '\n') { 
            char** parsed = parse_input(buffer);
            // handle built in commands, set exit/fork conditions 
            exit_con = strcmp(parsed[0], "exit"); 
            fork_con = fork_cons(parsed);
            // fork when necesssary 
            if (fork_con) { 
                child_PID = forking(parsed); 
            }
            free(parsed); 
        }
    }
    return 0; 
}

