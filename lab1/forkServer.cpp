#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Reap terminated child processes to avoid zombies
void sigchld_handler(int) {
    while (waitpid(-1, nullptr, WNOHANG) > 0);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("Socket");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listen");
        close(server_fd);
        return 1;
    }

    signal(SIGCHLD, sigchld_handler);

    std::cout << "Concurrent Server listening on port "
              << PORT << std::endl;

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd,
                               (sockaddr*)&client_addr,
                               &client_len);

        if (client_fd < 0) {
            perror("Accept");
            continue;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork");
            close(client_fd);
            continue;
        }

        // Child process
        if (pid == 0) {
            close(server_fd);

            char buffer[BUFFER_SIZE];

            while (true) {
                int bytes = recv(client_fd,
                                 buffer,
                                 BUFFER_SIZE - 1,
                                 0);

                if (bytes <= 0)
                    break;

                buffer[bytes] = '\0';

                std::cout << "Client "
                          << getpid()
                          << ": "
                          << buffer
                          << std::endl;

                send(client_fd,
                     buffer,
                     bytes,
                     0);
            }

            std::cout << "Client disconnected (PID "
                      << getpid()
                      << ")" << std::endl;

            close(client_fd);
            exit(0);
        }

        // Parent process
        close(client_fd);
    }

    close(server_fd);
    return 0;
}