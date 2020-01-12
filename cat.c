#include "ucode.c"

int in;

int main(int argc, char *argv[])
{
	int fd;
	char buf[1024];
	int n;

	/* Open file for reading */
	if (argc > 1)
		fd = open(argv[1], O_RDONLY);
	else
		fd = 0;

	if (fd < 0)
	{
		printf("Cat: open failed\n", 1);
		return 0;
	}

	/* Read contents of file */
	printf("========== Cat ==========\n", 1);
	while (read(fd, buf, 1))		// read input line form file
	{
		if (buf[0]==27)
			break;
		write(1, buf, 1);
		if (buf[0]=='\n')
			mputc('\r');
	}

	printf("=========================\n", 1);
	close(fd);
}
