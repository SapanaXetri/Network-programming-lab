#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    // Create UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sock);
        return 1;
    }

    const char* message = "Hello from UDP Client!";

    // Send message
    sendto(sock,
           message,
           strlen(message),
           0,
           (sockaddr*)&server_addr,
           sizeof(server_addr));

    char buffer[BUFFER_SIZE];

    socklen_t server_len = sizeof(server_addr);

    // Receive reply
    int bytes = recvfrom(sock,
                         buffer,
                         BUFFER_SIZE - 1,
                         0,
                         (sockaddr*)&server_addr,
                         &server_len);

    if (bytes < 0) {
        perror("Receive failed");
        close(sock);
        return 1;
    }

    buffer[bytes] = '\0';

    std::cout << "Server says: " << buffer << std::endl;

    close(sock);

    return 0;
}