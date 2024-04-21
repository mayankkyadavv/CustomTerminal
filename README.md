# CustomTerminal

## Introduction
This project is an innovative project that leverages the power of C programming to create a custom terminal interface. This terminal is capable of interpreting and executing various commands similar to standard Unix shells, with enhanced control over command parsing and execution flow.

## Features
- **Command Parsing:** Efficiently parses user input into executable commands using custom logic.
- **Redirection and Piping:** Supports input and output redirection (`<`, `>`) and command chaining with pipes (`|`).
- **Custom Execution:** Handles the execution of commands using `execvp` with an ability to modify stdin and stdout streams dynamically.

## How It Works
The terminal reads input from the user, tokenizes the input to separate commands and arguments, and then executes these commands. It supports complex command lines with multiple commands chained through pipes and redirections, handling their execution in the correct order.

## Building and Running
Compile the project using the provided Makefile:

```bash
make all
```
then: ```./myshell```
