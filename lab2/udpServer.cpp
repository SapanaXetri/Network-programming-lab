#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    // Create UDP socket
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (server_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Server address
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    std::cout << "UDP Server listening on port " << PORT << std::endl;

    char buffer[BUFFER_SIZE];

    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);

    // Receive message
    int bytes = recvfrom(server_fd,
                         buffer,
                         BUFFER_SIZE - 1,
                         0,
                         (sockaddr*)&client_addr,
                         &client_len);

    if (bytes < 0) {
        perror("Receive failed");
        close(server_fd);
        return 1;
    }

    buffer[bytes] = '\0';

    std::cout << "Received from "
              << inet_ntoa(client_addr.sin_addr)
              << ":" << ntohs(client_addr.sin_port)
              << std::endl;

    std::cout << "Message: " << buffer << std::endl;

    // Reply
    const char* reply = "Hello from UDP Server!";

    sendto(server_fd,
           reply,
           strlen(reply),
           0,
           (sockaddr*)&client_addr,
           client_len);

    close(server_fd);

    return 0;
}