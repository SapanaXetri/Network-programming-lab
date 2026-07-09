#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080

int sock;

void sigio_handler(int)
{
    char buffer[1024];

    int bytes = recv(sock,
                     buffer,
                     sizeof(buffer),
                     0);

    if (bytes > 0) {

        buffer[bytes] = '\0';

        std::cout << "Received: "
                  << buffer
                  << std::endl;
    }
}

int main()
{
    sock = socket(AF_INET,
                  SOCK_STREAM,
                  0);

    sockaddr_in server{};

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    inet_pton(AF_INET,
              "127.0.0.1",
              &server.sin_addr);

    connect(sock,
            (sockaddr*)&server,
            sizeof(server));

    signal(SIGIO, sigio_handler);

    fcntl(sock,
          F_SETOWN,
          getpid());

    int flags = fcntl(sock,
                      F_GETFL);

    fcntl(sock,
          F_SETFL,
          flags | O_ASYNC);

    std::cout << "Waiting for SIGIO..." << std::endl;

    while (true)
        pause();

    close(sock);

    return 0;
}