#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

void daemonize()
{
    pid_t pid;

    // First fork
    pid = fork();

    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }

    // Parent exits
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    // Create new session
    if (setsid() < 0)
    {
        exit(EXIT_FAILURE);
    }

    // Second fork
    pid = fork();

    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    // Reset file mode creation mask
    umask(0);

    // Change working directory
    chdir("/");

    // Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Redirect stdin to /dev/null
    open("/dev/null", O_RDONLY);

    // Redirect stdout to /dev/null
    open("/dev/null", O_WRONLY);

    // Redirect stderr to /dev/null
    open("/dev/null", O_RDWR);
}

int main()
{
    daemonize();

    while (true)
    {
        std::ofstream logfile("/tmp/daemon.log", std::ios::app);

        logfile << "Daemon is running. PID = "
                << getpid()
                << std::endl;

        logfile.close();

        sleep(5);
    }

    return 0;
}