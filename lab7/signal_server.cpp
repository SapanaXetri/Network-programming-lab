#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int server_fd;

// Handle Ctrl+C (SIGINT)
void signalHandler(int sig)
{
    std::cout << "\nSignal " << sig
              << " received." << std::endl;

    std::cout << "Closing server socket..."
              << std::endl;

    close(server_fd);

    std::cout << "Server terminated successfully."
              << std::endl;

    exit(0);
}

int main()
{
    // Register signal handler
    signal(SIGINT, signalHandler);

    server_fd = socket(AF_INET,
                       SOCK_STREAM,
                       0);

    if (server_fd < 0)
    {
        perror("Socket");
        return 1;
    }

    int opt = 1;

    setsockopt(server_fd,
               SOL_SOCKET,
               SO_REUSEADDR,
               &opt,
               sizeof(opt));

    sockaddr_in server_addr{};

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_fd,
         (sockaddr*)&server_addr,
         sizeof(server_addr));

    listen(server_fd, 5);

    std::cout << "Server listening on port "
              << PORT << std::endl;

    while (true)
    {
        sockaddr_in client{};
        socklen_t len = sizeof(client);

        int client_fd = accept(server_fd,
                               (sockaddr*)&client,
                               &len);

        if (client_fd < 0)
            continue;

        std::cout << "Client connected."
                  << std::endl;

        char buffer[BUFFER_SIZE];

        int bytes = recv(client_fd,
                         buffer,
                         BUFFER_SIZE - 1,
                         0);

        if (bytes > 0)
        {
            buffer[bytes] = '\0';

            std::cout << "Received: "
                      << buffer
                      << std::endl;

            send(client_fd,
                 buffer,
                 bytes,
                 0);
        }

        close(client_fd);
    }

    close(server_fd);

    return 0;
}