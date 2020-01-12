#include "ucode.c"

char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";

int ls_file(char *filename)
{
  struct stat fstat, *sp;
  int r, i;
  char sbuf[256];

  sp = &fstat;
  r = stat(filename, sp);   // syscall in ucode.c

  if ((sp->st_mode & 0xF000) == 0x4000)
    printf("%c", 'd', 1);
  else if ((sp->st_mode & 0xF000) == 0x8000)
    printf("%c", '-', 1);
  else if ((sp->st_mode & 0xF000) == 0xA000)
    printf("%c", 'l', 1);

  for (i=8; i>=0; i--)
  {
    if (sp->st_mode & (1<<i))
      printf("%c", t1[i], 1);
    else
      printf("%c", t2[i], 1);
  }

  printf(" %d ", sp->st_nlink, 1); // link count
  printf("%d ", sp->st_gid, 1);
  printf("%d ", sp->st_uid, 1);   // uid
  printf("%d ", sp->st_size, 1);     // file size

  printf("%s", filename, 1);
  if ((sp->st_mode & 0xF000) == 0xA000)
  {
    r = readlink(filename, sbuf);
    printf(" -> %s", sbuf, 1);
  }

  printf("\n", 1);
}

int ls_dir(char *filename)
{
  char name[128], buf[1024], *cp;
  int dir;
  DIR *dp;

  // open DIR to read names
  dir = open(filename, O_RDONLY);

  if (dir < 0)
  {
    printf("ls open failed\n", 1);
    return;
  }

  read(dir, buf, 1024);
  cp = buf;
  dp = (DIR *)buf;

  while (cp < 1024 + buf)
  {
    strcpy(name, dp->name);
    name[dp->name_len] = '\0';
    ls_file(name);
    cp += dp->rec_len;
    dp = (DIR *)cp;
  }
}

int main(int argc, char *argv[])
{
  struct stat st;
  char cwd[1024];

  getcwd(cwd);


printf("================ ls ================\n", 1);
  if (argc == 1)
  {
    ls_dir(cwd);
  }
  else
  {
    stat(argv[1], &st);
    if ((st.st_mode & 0xF000) == 0x4000)
      ls_dir(argv[1]);
    else
      ls_file(argv[1]);
  }
  printf("====================================\n", 1);

}
