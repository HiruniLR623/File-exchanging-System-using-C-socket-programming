#include "header.h"
#include <sys/wait.h>

void sig_chld(int signo)
{
	pid_t	pid;
	int	stat;

	printf("SIGCHLD Received \n");
	
	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);
	return;
}
