#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int sock = socket(AF_INET,
                      SOCK_STREAM,
                      0);

    sockaddr_in server_addr{};

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    inet_pton(AF_INET,
              "127.0.0.1",
              &server_addr.sin_addr);

    connect(sock,
            (sockaddr*)&server_addr,
            sizeof(server_addr));

    char message[BUFFER_SIZE];

    while (true)
    {
        std::cout << "Enter message: ";
        std::cin.getline(message, BUFFER_SIZE);

        if (strcmp(message, "exit") == 0)
            break;

        send(sock,
             message,
             strlen(message),
             0);

        char buffer[BUFFER_SIZE];

        int bytes = recv(sock,
                         buffer,
                         BUFFER_SIZE - 1,
                         0);

        if (bytes <= 0)
            break;

        buffer[bytes] = '\0';

        std::cout << "Server: "
                  << buffer
                  << std::endl;
    }

    close(sock);

    return 0;
}