#include "ucode.c"

char home[128];
char shcwd[128];
int in, out;

int tokenize(char *line, char *storage[], char delim);

int processing_loop(int myargc, char *myargv[])
{
	int pid; /* Process ID */
	int i;
	int exitstatus;
	char line[1024];

	//clear contents of argv[]
	for (i = 0; myargv[i] != 0; i++)
		myargv[i] = 0;
	myargc = 0;

	getcwd(shcwd);

	// get command line from user and tokenize
	printf("SH:%s # ", shcwd, 1);
	gets(line);
	line[strlen(line)] = 0;
	myargc = tokenize(line, myargv, ' ');

	// cd command: change directory
	if (strcmp(myargv[0], "cd")==0)
	{
  	if (myargv[1])
		{
			if (chdir(myargv[1]) != 0)
				printf("Directory change failed\n");
		}
		else
			chdir(home);
	}

	// exit command: exit program
	else if (strcmp(myargv[0], "exit")==0)
		exit(0);

	// All other commands: make new child process
	else
	{
		pid = fork(); // fork a child process

		if (pid)
			pid = wait(&exitstatus);   // wait for child to finish executing
		else
			execute_child(myargc, myargv);		// execute child process
	}
}

int execute_child(int myargc, char *myargv[])
{
  // HAS TO BE ABLE TO RECURSIVELY DO MULTIPLE PIPES AND REDIRECTS

	int i;
	char cmd[1024];

	// Check the arguments for a pipe starting at the end
	for (i=(myargc-1); i>=0; i--)
	{
		if (strcmp(myargv[i], "|") == 0)
		{
			// will recursively call execute_child to create more pipes if needed
			do_pipe(i, myargv, myargc-(i+1), &myargv[i+1]);
			return;
		}
	}

	// No further pipes needed in current command line
	// Do file Reditection
	if (redirect(myargc, myargv))
		return;

	// Made it here: command line has no more pipes or redirects
	// format command strings and call exec()
	if (myargc > 0)
		strcpy(cmd, myargv[0]);
	for (i = 1; i<myargc; i++)
	{
		strcat(cmd, " ");
		strcat(cmd, myargv[i]);
	}
	if (myargc > 0)
		if (strcmp(myargv[0], "more") == 0)		// need to add /dev/ttyx as an argument to more
		{																			// as stdin can be redirected elsewhere and
			strcat(cmd, " ");										// we still need to get input from the user
			strcat(cmd, argv[1]);
		}
		exec(cmd);
}

int do_pipe(int argc1, char *argv1[], int argc2, char *argv2[])
{
	int pid, pd[2];
	int status;

	pipe(pd);
	pid = fork();

	if (pid)	// parent reader
	{
		close(pd[1]);
		dup2(pd[0], 0);

		execute_child(argc2, argv2);
	}
	else	// child writer
	{
		close(pd[0]);
		dup2(pd[1], 1);

		execute_child(argc1, argv1);
	}
}

int redirect(int myargc, char *myargv[])
{
	int i;
	int fd;

	for (i = 0; i < myargc; i++)
	{
		if (strcmp(myargv[i], "<") == 0) // Read
		{
			close(fd);
			fd = open(myargv[i+1], O_RDONLY);
			dup2(fd, 0);

			execute_child(i, myargv);
			return 1;
		}
		if (strcmp(myargv[i], ">") == 0) // Write
		{
			close(fd);
			fd = open(myargv[i+1], O_WRONLY | O_CREAT);
			dup2(fd, 1);

			execute_child(i, myargv);
			return 1;
		}
		if (strcmp(myargv[i], ">>") == 0) // Append
		{
			close(fd);
			fd = open(myargv[i+1], O_RDWR | O_APPEND | O_CREAT);
			dup2(fd, 1);

			execute_child(i, myargv);
			return 1;
		}
	}
	return 0; // Failed (none found)
}

int tokenize(char *line, char *storage[], char delim)
{
  char *cp;
  cp = line;
  int i = 0;

  while (*cp != 0){
    while (*cp == delim)
      *cp++ = 0;

    if (*cp != 0)
      storage[i++] = cp;

    while (*cp != delim && *cp != 0) cp++;
      if (*cp != 0)
        *cp = 0;
      else
        break;

    cp++;
  }
  storage[i] = 0;
  return i;
}

/* Upon entry from login process, argv[0] = sh, argv[1] = "/dev/ttyx" */

int main(int argc, char* argv[])
{
  getcwd(home);   // save home directory
	int myargc;
	char *myargv[128];

	/* Execute command loop */
	while (1)
	{
		processing_loop(myargc, myargv);
	}
}
