#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/sendfile.h>
#include <cerrno>

const int BUFFER_SIZE = 4096;
// user read, write
// group read
// others read
const mode_t FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

void handleChildProcess(int pipefd[2]);
void handleParentProcess(int pipefd[2], const char* filename);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./pipefile <file>\n";
        return 1;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        handleChildProcess(pipefd);
    }
    else {
        handleParentProcess(pipefd, argv[1]);
        wait(nullptr);
    }

    return 0;
}

void handleChildProcess(int pipefd[2]) {
    close(pipefd[1]);

    int file = open("filerecv", O_WRONLY | O_CREAT | O_TRUNC, FILE_MODE);
    if (file == -1) {
        perror("open");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    while ((bytesRead = read(pipefd[0], buffer, BUFFER_SIZE)) > 0) {
        if (write(file, buffer, bytesRead) == -1) {
            perror("write");
            exit(1);
        }
    }

    close(file);
    close(pipefd[0]);
    exit(0);
}

void handleParentProcess(int pipefd[2], const char* filename) {
    close(pipefd[0]);

    int file = open(filename, O_RDONLY);
    if (file == -1) {
        perror("open");
        exit(1);
    }

    off_t offset = 0;
    ssize_t bytesSent;
    while ((bytesSent = sendfile(pipefd[1], file, &offset, BUFFER_SIZE)) > 0) {
        if (bytesSent == -1) {
            perror("sendfile");
            break;
        }
    }

    close(file);
    close(pipefd[1]);
}
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/sendfile.h>
#include <cerrno>

const int BUFFER_SIZE = 4096;
// user read, write
// group read
// others read
const mode_t FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

void handleChildProcess(int pipefd[2]);
void handleParentProcess(int pipefd[2], const char* filename);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./pipefile <file>\n";
        return 1;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        handleChildProcess(pipefd);
    }
    else {
        handleParentProcess(pipefd, argv[1]);
        wait(nullptr);
    }

    return 0;
}

void handleChildProcess(int pipefd[2]) {
    close(pipefd[1]);

    int file = open("filerecv", O_WRONLY | O_CREAT | O_TRUNC, FILE_MODE);
    if (file == -1) {
        perror("open");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    while ((bytesRead = read(pipefd[0], buffer, BUFFER_SIZE)) > 0) {
        if (write(file, buffer, bytesRead) == -1) {
            perror("write");
            exit(1);
        }
    }

    close(file);
    close(pipefd[0]);
    exit(0);
}

void handleParentProcess(int pipefd[2], const char* filename) {
    close(pipefd[0]);

    int file = open(filename, O_RDONLY);
    if (file == -1) {
        perror("open");
        exit(1);
    }

    off_t offset = 0;
    ssize_t bytesSent;
    while ((bytesSent = sendfile(pipefd[1], file, &offset, BUFFER_SIZE)) > 0) {
        if (bytesSent == -1) {
            perror("sendfile");
            break;
        }
    }

    close(file);
    close(pipefd[1]);
}
