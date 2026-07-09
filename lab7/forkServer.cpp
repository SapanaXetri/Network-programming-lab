#include <sys/wait.h>
#include <signal.h>

void sigchld_handler(int)
{
    while (waitpid(-1, NULL, WNOHANG) > 0)
    {
        std::cout << "Child process terminated."
                  << std::endl;
    }
}

int main()
{
    signal(SIGCHLD, sigchld_handler);

    // fork() server code
}