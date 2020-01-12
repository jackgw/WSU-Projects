#include "ucode.c"

int in;

int main(int argc, char *argv[])
{
	char buf[1024], *cp;
	int n_read;
	int fd;

	/* Open file for reading */
	if (argc > 1)
		fd = open(argv[1], O_RDONLY);
	else
		fd = 0;

	if (fd < 0)
	{
		printf("L2U: open failed\n", 1);
		return 0;
	}

	/* Read contents of file */
	printf("========== L2U ==========\n", 1);
	while (read(fd, buf, 1))		// read input form file
	{
		if (buf[0]==27)
			break;
    cp = buf;
    if (('a' <= *cp) && (*cp <= 'z'))
      *cp = *cp + 'A' - 'a';
		write(1, buf, 1);
		if (*buf == '\n')
			mputc('\r');
	}

	printf("=========================\n", 1);
	close(fd);
}
