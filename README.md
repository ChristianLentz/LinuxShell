# Writing a Shell

## Deliverables 

### Authors: Christian Lentz, Jeff Wang, Theo Hoyer 

#### Task required to complete: 

- Read User Input (completed) 
- Run a simple process (completed) 
- Parse whitespace in commands (completed) 
- Run multi-token commands (completed) 
- Built-in commands (completed) 
     
#### Tasks not required: 

- Trap `SIGINT` (completed)
- Run background processes (not completed) 
- Output redirection (not completed) 
- Piping (not completed)

#### Known bugs/erros: 

- None!
     
#### Sources: 

- [Writing a Shell in C](https://brennan.io/2015/01/16/write-a-shell-in-c/): This 
was a valuable source which gve a detailed discussion of how to go about writing a 
shell and making sense of each task that we were walking through. We relied on this 
source to give us an overview of how to parse strings and fork to child processes. 
- [GeeksForGeeks](https://www.geeksforgeeks.org/?newui): provided details on how to
use specific C functions, mostly used for some more examples/context for new functions past just the abstract type information in the man pages
- [man7.org](https://www.man7.org/index.html) provided details on how to use specific 
C functions, including function parameters and return values
- [linuxhint](https://linuxhint.com/signal_handlers_c_programming_language/) specifically useful for examples of passing function references, for the SIGINT step, which was a new concept
# Project Outline 

In this assignment, you will create a program, `shell.c`, which implements a
command-line interface for running programs, similar to bash, which you
have been using for this course.

Your program will need to loop, reading commands from the user, and use C
methods `fork()` and `exec()` to carry out those instructions. You will need to
handle common errors without exiting the program, like a real shell.

You may work with a partner for this project, though it is not required.

This assignment is meant to stretch your coding ability--it is intentionally
challenging! You can use outside sources for help: google for example code, look
at man pages. But **you must credit your sources in the comments** and **you
must write up the code on your own**. You may **not** copy code from external
sources verbatim. A good general rule is that if you do seek external help, you
should cite your source and write up pseudocode as a comment, then adapt that
pseudocode to real code.

# Problem Description

This assignment has multiple parts that you will need to implement for full
credit, each described below. 

If you complete the first five parts successfully, you will receive a B
(85/100) on this assignment. To receive a 100/100, you will need to complete
everything described below. I want you to prioritize--complete as much as you
can, and be willing to accept the B if you need more time for other
end-of-semester considerations!

Each task includes a reference to the C function and/or Unix system call that
could help you implement that task. You can use these to find the man page. For
example, bring up the man page for `fork(2)` by typing the following in the
command line:

```
     man 2 fork
```

If you get stuck on a particular step, you can try faking it and demonstrating
that the rest is working. For example, if you aren't able to tokenize user input
in step 3, you might hard code a pre-tokenized array to demonstrate your
completion of step 4. Please clearly indicate any work that you have faked.

# Tasks

For many of the below tasks, you should plan to use the `PATH` variable that is
inherited from the environment.

Reference: `getenv(3)`

## 1. Read user input

Inside a loop in main(), present a prompt to the user and then read a full
line of input (up to and including the newline character). Continue looping
until `EOF` is reached. You can send the `EOF` character by entering Ctrl-D on
the command line.

Reference: `fgets(3)`

## 2. Run a simple process

Run a single word command line (e.g., "ls") by forking a subprocess and
executing it. You may assume the word is at the start of the line and that there
is no extraneous whitespace. Your main loop should wait until the process
completes before continuing. 

Reference: `fork(2)`, `execvp(2)`, `waitpid(2)`

## 3. Parse whitespace in commands

Parse a command line with whitespace into a vector of strings. 

For example, given this string:

    "foo |    bar > baz"
        
Your program will produce an array, `args`, containing the following:

    args[0] = "foo";
    args[1] = "|";
    args[2] = "bar"
    args[3] = ">";
    args[4] = "baz";
    args[5] = NULL;
        
This part of the assignment can be  quite difficult, so be sure to test your
work thoroughly.

For additional challenge, handle parsing a command line without spaces between
the symbols:  > < & | 

Reference: `strtok(3)`, `strchr(3)`, `strsep(3)`, `strpbrk(3)`

## 4. Run multi-token commands

Take a vectorized multi-token command line from task 3, above, and run the
program specified. Your shell should wait until the program completes before
displaying the next command prompt. I recommend creating a function that
executes the program and returns the child process ID (PID). 

Note that supporting piping and redirection are additional tasks for this
assignment--for this task, you only have to support commands with more than one
token, so being able to run a program that takes an argument is sufficient for
this. I suggest you try `sort` or `cat` for testing.

Reference: `fork(2)`, `execvp(2)`, `waitpid(2)`

## 5. Built-in commands

Implement the following built-in commands. This means that you should not run
them as external processes, but rather capture the command and use your own code
to execute the described functionality.

* `exit` - terminate your shell input loop
* `myinfo` - print out your PID and PPID (parent process ID--the ID of the
  process that started this process) in a readable format
* `cd` - change your working directory to `$HOME`
* `cd <dir>` - change your working directory to `<dir>`

Reference: `chdir(2)`, `getenv(3)`, `getpid(2)`, `getppid(2)`

## 6. Trap `SIGINT`

Trap `SIGINT` (triggered when the user enters Ctrl-C at the terminal) and have
it terminate the currently running child process of the shell, if any, and **not
the shell itself**.

Reference: `signal(3)`, `kill(3)`

## 7. Run background processes

If the last token on the command line is `&`, have that program run in the
background instead of the foreground. The prompt should be redisplayed without
waiting for the child to return. At the start of the loop in `main`, you will
need to do the following before displaying the user prompt:

* Check if any children have completed their execution and 'died'--these are
  known as zombie processes. You will need to manually clean them up, a process
  known as reaping
* Reap the zombie children
* Display a message to the user that the processes have been reaped

Reference: `waitpid(2)` with a PID of -1 and the `WNOHANG` option

As an additional challenge, add in support to trap `SIGTSTP` (Ctrl-Z) and have
it change the child program that is currently running into a background process.

## 8. Output redirection

Support `<` and `>` for input and output redirection to the program. Do this by
opening the requested file for read/write as needed and duping it to
`STDIN_FILENO` or `STDOUT_FILENO`. 

Reference: `open(2)` or `fopen(3)`/`fileno(3)`, `dup2(2)`

## 9. Piping

Support command line piping with a single pipe (the `|` character, on the same
key as the backslash, above the return/enter key on most keyboards). For
example, given the following command:

    foo | bar
        
Your shell should set `STDIN_FILENO` of bar to read from the `STDOUT_FILENO` of
foo. You can do this by using `pipe(2)` to create connected pairs of file
descriptors and then use `dup2(2)` to set them up appropriately in the children.
Close the unused ends (e.g., foo should close the descriptor that bar is using
to read). Your shell will need to execute both processes and wait for both of
them to return. Note that you can change the `|` in your argument vector to a
NULL and just pass in different starting locations of the same vector to your
execution function. You may want to have a separate array that keeps track of
the start index of various command segments. 

Reference: `dup2(2)`, `pipe(2)`

## Challenge

The following are challenge problems. Do not attempt them unless you have
completed all other parts of the assignment.

* Add support for setting and modifying environment variables
* Support a multi-pipe sequence

# Programming Notes

* Your program needs to handle input lines up to 1024 characters
* You may assume/limit the number of background processes
* You may assume there are no more than 100 tokens (words) on a line
* You may assume there are no more than 10 command segments on the command line

# Error handling

Your program will need to handle the following errors. Note that some of these
errors only become possible after you have implemented the corresponding task;
you only have to handle errors for tasks that you have completed. The first two
errors should be simple to handle, but the last two may result in some processes
running, depending on your implementation of piping.

* Command not found on the path
* Input redirected in from a non-existent file
* Input redirected in to a pipe command that is not the first command
* Output redirected from a pipe command that is not the last command

# Deliverables

You should deliver:
* `shell.c`, which contains the C source code for your shell
* `Makefile`, which will `make` and `make clean` your source code
* A README that contains:
    * Your name and your partner's name, if any
    * A list of all the portions of the shell you were asked to complete, and
      in which you've clearly indicated which parts you have completed and which
      parts you have not
    * Any known bugs/errors with your code

## Thanks

Thanks to Professor Ben Kuperman for developing the original version of this
assignment and Professor Roberto Hoyle for his additional modifications.
