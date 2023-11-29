#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main()
{
    pid_t  pid;
	/* fork another process */
	pid = fork();
	if (pid < 0) 
    { 
        /* error occurred */
		fprintf(stderr, "Fork Failed");
		exit(-1);
	}
	else if (pid == 0) 
    { 
      /* child process */
        printf("This is child process, pid=%d\n", getpid());
		execlp("/bin/ls", "ls", NULL);
        printf("Child process finished\n"); /*这句话不会被打印，除非execlp调用未成功*/

	}
	else 
    { 
      /* parent process */
	  /* parent will wait for the child to complete */
        printf("This is parent process, pid=%d\n", getpid());
		wait (NULL);
		printf ("Child Complete");
		exit(0);
	}
}

