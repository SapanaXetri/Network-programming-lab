#include <iostream>
#include <unistd.h>
#include <sys/select.h>

int main() {

    fd_set readfds;

    while (true) {

        FD_ZERO(&readfds);

        FD_SET(STDIN_FILENO, &readfds);

        std::cout << "Waiting for keyboard input..." << std::endl;

        select(STDIN_FILENO + 1,
               &readfds,
               NULL,
               NULL,
               NULL);

        if (FD_ISSET(STDIN_FILENO, &readfds)) {

            char buffer[100];

            read(STDIN_FILENO,
                 buffer,
                 sizeof(buffer));

            std::cout << "Input received." << std::endl;
            break;
        }
    }

    return 0;
}