#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd, max_fd, activity;

    int client_socket[MAX_CLIENTS];

    for (int i = 0; i < MAX_CLIENTS; i++)
        client_socket[i] = 0;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

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
        return 1;
    }

    listen(server_fd, 5);

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {

        fd_set readfds;

        FD_ZERO(&readfds);

        FD_SET(server_fd, &readfds);

        max_fd = server_fd;

        // Add all client sockets
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = client_socket[i];

            if (sd > 0)
                FD_SET(sd, &readfds);

            if (sd > max_fd)
                max_fd = sd;
        }

        activity = select(max_fd + 1,
                          &readfds,
                          NULL,
                          NULL,
                          NULL);

        if (activity < 0) {
            perror("Select");
            continue;
        }

        // New client connection
        if (FD_ISSET(server_fd, &readfds)) {

            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);

            client_fd = accept(server_fd,
                               (sockaddr*)&client_addr,
                               &client_len);

            std::cout << "New client connected: "
                      << client_fd << std::endl;

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = client_fd;
                    break;
                }
            }
        }

        // Check all clients
        for (int i = 0; i < MAX_CLIENTS; i++) {

            int sd = client_socket[i];

            if (FD_ISSET(sd, &readfds)) {

                char buffer[BUFFER_SIZE];

                int bytes = recv(sd,
                                 buffer,
                                 BUFFER_SIZE - 1,
                                 0);

                if (bytes == 0) {

                    std::cout << "Client "
                              << sd
                              << " disconnected."
                              << std::endl;

                    close(sd);
                    client_socket[i] = 0;
                }
                else {

                    buffer[bytes] = '\0';

                    std::cout << "Client "
                              << sd
                              << ": "
                              << buffer
                              << std::endl;

                    send(sd,
                         buffer,
                         bytes,
                         0);
                }
            }
        }
    }

    close(server_fd);

    return 0;
}