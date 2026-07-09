#include <signal.h>

int main()
{
    signal(SIGPIPE, SIG_IGN);

    // Server code
}