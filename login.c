#include "ucode.c"

int in, out, err;
char usrname[128], usrpassword[128];
char *userinfo[32];

void usertok(char *line)
{
  char *cp;
  cp = line;
  int i = 0;

  while (*cp != 0){
       while (*cp == ':') *cp++ = 0;
       if (*cp != 0)
           userinfo[i++] = cp;
       while (*cp != ':' && *cp != 0) cp++;
       if (*cp != 0)
           *cp = 0;
       else
            break;
       cp++;
  }
  userinfo[i] = 0;
}

char *readline_file(char *line)
{
  char c;
  line = 0;
  while (c = getc())
  {
    printf("[%c]\n", c, 1);
    if (c == EOF)
      break;
    if (c == '\n')
      break;
    strcat(line, &c);
  }
}

/* Upon entry, argv[0] = login, argv[1] = /dev/ttyx */

main(int argc, char *argv[])
{
  int pwd_file;
  char line [128];
  char pcmd [128];

  //printf("argv[1]: %s", argv[1], 1);

  // close FD 0,1 inherited from INIT
  close(in); close(out);

  // open argv[1] 3 times as in(0), out(1), and err(2)
  in = open(argv[1], 0);
  out = open(argv[1], 1);
  err = open(argv[1], 2);

  printf("LOGIN : %s opened as stdin = 0, stdout = 1, stderr = 2\n", argv[1], 1);

  // settty(argv[1]); set tty name string in PROC.tty
  settty(argv[1]);

  // Get login from user and check if valid account
  while (1)
  {
    close(in);
    in = open(argv[1], 0);

    printf("login: ", 1);
    gets(usrname);
    printf("password: ", 1);
    gets(usrpassword);

    close(in);
    in = open("/etc/passwd", O_RDONLY);

    while (getline(line))   // for each line in the password file
    {
      usertok(line);
      if ((strcmp(usrname, userinfo[0]) == 0) && (strcmp(usrpassword, userinfo[1]) == 0))   // valid account
      {
        chuid(atoi(userinfo[3]), atoi(userinfo[2]));
        chdir(userinfo[5]);

        close(pwd_file);
        close(in);
        in = open(argv[1], 0);  // change in back to the console/UART

        strcpy(pcmd, userinfo[6]);
        pcmd[strlen(pcmd)-1] = 0;
        strcat(pcmd, " ");
        strcat(pcmd, argv[1]);

        exec(pcmd);   // exec in the form "program /dev/ttyx"
        return;
      }
    }
  }
}
