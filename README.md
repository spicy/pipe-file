# Pipe File Transfer in C++

This project demonstrates a simple pipe-based file transfer between a parent and child process using Linux system calls such as `pipe()`, `fork()`, `read()`, and `sendfile()`. The parent process reads from a source file and sends its contents through a pipe to a child process, which writes the received data into a new file named `filerecv`.

## How It Works

1. **Parent Process**: 
   - The parent opens the source file specified as a command-line argument.
   - It creates a pipe and forks a child process.
   - Using `sendfile()`, the parent reads the content of the source file and sends it to the child process via the pipe.

2. **Child Process**:
   - The child process reads from the pipe and writes the content into a new file called `filerecv`.
   - The new file is created with read/write permissions for the owner and read permissions for the group and others.

## File Permissions

- The new file (`filerecv`) is created with the following permissions:
  - **User**: read, write
  - **Group**: read
  - **Others**: read

## Prerequisites

- A Linux-based operating system
- GCC compiler to compile the code
- Standard libraries: `unistd.h`, `fcntl.h`, `sys/wait.h`, `sys/sendfile.h`, `cerrno`

## Compilation

To compile the program, use the following command:

g++ -o pipefile pipefile.cpp

## Usage

To run the program, specify the source file you want to transfer:

./pipefile <source_file>

For example:

./pipefile test.txt

This will create a new file named `filerecv` that contains the content of `test.txt`.

## Explanation of Functions

- **handleParentProcess()**: Opens the source file and sends the file content to the child process using `sendfile()`.
- **handleChildProcess()**: Receives the file content from the pipe and writes it to a new file called `filerecv`.

## Error Handling

The program includes basic error handling for system calls like `pipe()`, `fork()`, `open()`, and `write()`. If an error occurs, it will print an error message and terminate.
