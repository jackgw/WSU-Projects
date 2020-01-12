/*
*  Jack Wharton
*  11506329
*  CptS 460 Final Project
*  12/6/2019
*/

#include "ucode.c"

int console;
int uart1;
int uart2;

int parent()
{
  int pid, status;

  while(1)
  {
    printf("INIT : wait for ZOMBIE child\n", 1);
    pid = wait(&status);
    if (pid==console)
    {
      printf("INIT : fork new console login\n", 1);
      console = fork();
      if (console)  // parent
        continue;
      else
        exec("login /dev/tty0");   // child: do login process
    }
    if (pid==uart1)
    {
      printf("INIT : fork new ttyS0 login\n", 1);
      uart1 = fork();
      if (uart1)
        continue;
      else
        exec("login /dev/ttyS0");
    }
    if (pid==uart2)
    {
      printf("INIT : fork new ttyS1 login\n", 1);
      uart2 = fork();
      if (uart2)
        continue;
      else
        exec("login /dev/ttyS1");
    }
    printf("INIT : burried orphan child %d\n", pid, 1);
  }
}

int main(int argc, char *argv[ ])
{
  int in, out;                        // FD for terminal I/O

  in = open("/dev/tty0", O_RDONLY);   // FD 0
  out = open("/dev/tty0", O_WRONLY);  // display to console
  printf("\nINIT : fork login process on console\n", 1);

  console = fork();

  if (console)
  {
    close(in); close(out);

    in = open("/dev/ttyS0", O_RDONLY);   // FD 0
    out = open("/dev/ttyS0", O_WRONLY);  // display to UART1
    printf("\nINIT : fork login process on UART1\n", 1);

    uart1 = fork();
  }

  if (console && uart1)
  {
    close(in); close(out);

    in = open("/dev/ttyS1", O_RDONLY);   // FD 0
    out = open("/dev/ttyS1", O_WRONLY);  // display to UART2
    printf("\nINIT : fork login process on UART2\n", 1);

    uart2 = fork();
  }

  if (console && uart1 && uart2)  // Parent: wait for any child to finish and fork new console when needed
  {
    close(in); close(out);
    in = open("/dev/tty0", O_RDONLY);   // FD 0
    out = open("/dev/tty0", O_WRONLY);  // display to console
    parent();
  }
  else if (console && uart1)      // UART 2 Child: exec login on ttyS1
    exec("login /dev/ttyS1");
  else if (console)               // UART 1 Child: exec login on ttyS0
    exec("login /dev/ttyS0");
  else                            // Console Child: exec login on tty0
    exec("login /dev/tty0");
}
