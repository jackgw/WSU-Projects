#include "ucode.c"

int main(int argc, char *argv[])
{
  int fd, gd;
  int n;
  char buf[1024];

  if (argc < 3)
  {
    printf("cp usage: cp [file 1] [file 2]\n", 1);
    return;
  }

  fd = open(argv[1], O_RDONLY);
  gd = open(argv[2], O_WRONLY | O_CREAT);

  if (fd < 0 || gd < 0)
    return;

  // Check that fd is a regular file
  if (!is_reg(argv[1]) || !is_reg(argv[2]))
    return;

  // Check that fd and gd are not the same file
  if (fd == gd)
    return;

  while(read(fd, buf, 1))
    write(gd, buf, 1);

  close(fd);
  close(gd);
}

int is_reg(char *filename)
{
  struct stat mystat, *sp;
  sp = &mystat;
  stat(filename, sp);
  if ((sp->st_mode & 0xF000) == 0x8000)
    return 1;
  return 0;
}
