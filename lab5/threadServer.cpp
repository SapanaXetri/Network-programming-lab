#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void* handleClient(void* arg)
{
    int client_fd = *(int*)arg;
    delete (int*)arg;    // Free dynamically allocated memory

    char buffer[BUFFER_SIZE];

    while (true)
    {
        int bytes = recv(client_fd,
                         buffer,
                         BUFFER_SIZE - 1,
                         0);

        if (bytes <= 0)
            break;

        buffer[bytes] = '\0';

        std::cout << "Thread "
                  << pthread_self()
                  << " received: "
                  << buffer
                  << std::endl;

        send(client_fd,
             buffer,
             bytes,
             0);
    }

    std::cout << "Client disconnected." << std::endl;

    close(client_fd);

    pthread_exit(NULL);
}

int main()
{
    int server_fd = socket(AF_INET,
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

    if (bind(server_fd,
             (sockaddr*)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("Bind");
        return 1;
    }

    listen(server_fd, 5);

    std::cout << "Multithreaded Server listening on port "
              << PORT << std::endl;

    while (true)
    {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd,
                               (sockaddr*)&client_addr,
                               &client_len);

        if (client_fd < 0)
        {
            perror("Accept");
            continue;
        }

        std::cout << "Client connected." << std::endl;

        pthread_t tid;

        int* pclient = new int;
        *pclient = client_fd;

        if (pthread_create(&tid,
                           NULL,
                           handleClient,
                           pclient) != 0)
        {
            perror("pthread_create");
            close(client_fd);
            delete pclient;
            continue;
        }

        // Automatically release thread resources when it exits
        pthread_detach(tid);
    }

    close(server_fd);

    return 0;
}