#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/sendfile.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>

const int BUFFER_SIZE = 4096;
const mode_t FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

class FileTransfer {
public:
    static void transferFile(const std::string& sourceFilename, const std::string& destFilename) {
        int pipefd[2];
        createPipe(pipefd);

        pid_t pid = fork();
        if (pid == -1) {
            throw std::runtime_error("Failed to fork: " + getErrorString());
        }

        if (pid == 0) {
            handleChildProcess(pipefd, destFilename);
        } else {
            handleParentProcess(pipefd, sourceFilename);
            waitForChildProcess(pid);
        }
    }

private:
    static void createPipe(int pipefd[2]) {
        if (pipe(pipefd) == -1) {
            throw std::runtime_error("Failed to create pipe: " + getErrorString());
        }
    }

    static void handleChildProcess(int pipefd[2], const std::string& destFilename) {
        close(pipefd[1]);  // Close unused write end

        int destFile = openFile(destFilename, O_WRONLY | O_CREAT | O_TRUNC);
        receiveFile(pipefd[0], destFile);

        close(destFile);
        close(pipefd[0]);
        exit(0);
    }

    static void handleParentProcess(int pipefd[2], const std::string& sourceFilename) {
        close(pipefd[0]);  // Close unused read end

        int sourceFile = openFile(sourceFilename, O_RDONLY);
        sendFile(sourceFile, pipefd[1]);

        close(sourceFile);
        close(pipefd[1]);
    }

    static int openFile(const std::string& filename, int flags) {
        int file = open(filename.c_str(), flags, FILE_MODE);
        if (file == -1) {
            throw std::runtime_error("Failed to open file " + filename + ": " + getErrorString());
        }
        return file;
    }

    static void receiveFile(int sourcefd, int destfd) {
        char buffer[BUFFER_SIZE];
        ssize_t bytesRead;
        while ((bytesRead = read(sourcefd, buffer, BUFFER_SIZE)) > 0) {
            if (write(destfd, buffer, bytesRead) == -1) {
                throw std::runtime_error("Failed to write to destination file: " + getErrorString());
            }
        }

        if (bytesRead == -1) {
            throw std::runtime_error("Failed to read from pipe: " + getErrorString());
        }
    }

    static void sendFile(int sourcefd, int destfd) {
        off_t offset = 0;
        ssize_t bytesSent;
        while ((bytesSent = sendfile(destfd, sourcefd, &offset, BUFFER_SIZE)) > 0) {
            // sendfile successful, continue
        }

        if (bytesSent == -1) {
            throw std::runtime_error("Failed to send file: " + getErrorString());
        }
    }

    static void waitForChildProcess(pid_t pid) {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            throw std::runtime_error("Failed to wait for child process: " + getErrorString());
        }
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            std::cerr << "Child process exited with non-zero status: " << WEXITSTATUS(status) << std::endl;
        }
    }

    static std::string getErrorString() {
        return std::string(strerror(errno));
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file>\n";
        return 1;
    }

    try {
        FileTransfer::transferFile(argv[1], "filerecv");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
