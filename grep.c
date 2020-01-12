#include "ucode.c"

int main(int argc, char *argv[])
{
  int fd;
	char c, *cp, line[1024];
	int n;

	/* Open file for reading */
	if (argc > 2)
		fd = open(argv[2], O_RDONLY);
  else
    fd = 0;

	if (fd < 0)
	{
		printf("Grep: open failed\n", 1);
		return 0;
	}

	/* Read contents of file */
	printf("========== Grep ==========\n", 1);
  cp = line;
	while (n = read(fd, &c, 1))		// read input line form file
	{
    if (c==27)
			break;
    if ((c == '\r') || (c == '\n'))
    {
      *cp++ = '\n';
      *cp++ = '\r';
      *cp = 0;
      if (strstr(line, argv[1]))    // check if argv[1] is substr of line
        write(1, line, strlen(line));
      cp = line;
    }
    else
    {
      *cp++ = c;
      *cp = 0;
    }
	}

	printf("==========================\n", 1);
	close(fd);
}
