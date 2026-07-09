#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080

int main() {

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    connect(sock, (sockaddr*)&server, sizeof(server));

    // Set socket to non-blocking mode
    fcntl(sock, F_SETFL, O_NONBLOCK);

    char buffer[1024];

    while (true) {

        int bytes = recv(sock, buffer, sizeof(buffer), 0);

        if (bytes > 0) {
            buffer[bytes] = '\0';
            std::cout << "Received: " << buffer << std::endl;
            break;
        }

        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            std::cout << "No data available..." << std::endl;
            sleep(1);
        }
    }

    close(sock);
}