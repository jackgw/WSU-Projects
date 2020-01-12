#include "ucode.c"

int main(int argc, char *argv[])
{
  char c, *cp, input, line[1024];
	int n_read, lines;
  int fd, ufd;

	/* Open file for reading */
	if (argc > 2)  // including /dev/ttyx
  {
		fd = open(argv[1], O_RDONLY);
    ufd = open(argv[2], O_RDONLY);
  }
	else
  {
    fd = 0;
    ufd = open(argv[1], O_RDONLY);
  }


	if (fd < 0 || ufd < 0)
	{
		printf("More: open failed\n", 1);
		return 0;
	}

	/* Read contents of file */
	printf("========== More ==========\n", 1);
  lines = 23;
  cp = line;
	while (read(fd, &c, 1))		// read input line form file
	{
    if (c==27)
			break;
    if (c == '\n')
    {
      *cp++ = '\n';
      *cp++ = '\r';
      *cp = 0;
      if (lines == 0)
      {
        read(ufd, &c, 1);  // get char from user
        if (c == ' ')
          lines = 23;
        else
          lines = 1;
      }
      write(1, line, strlen(line));
      lines--;
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
