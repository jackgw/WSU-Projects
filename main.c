/****************************************************************************
*                   JGW ext2 file system project                           *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <sys/stat.h>
#include <ext2fs/ext2_fs.h>

#include "type.h"

MINODE minode[NMINODE];
MINODE *root;
PROC   proc[NPROC], *running, *freeList, *readyQueue;
MOUNT MountTable[NMOUNT];

char   gpath[256]; 				// global for tokenized components
char   *name[64];  				// assume at most 64 components in pathname
int    n;          				// number of component strings

int    fd, dev;
int    nblocks, ninodes, bmap, imap, inode_start;
char   line[256], cmd[32], pathname[256];

char *status[3] = { "FREE", "BUSY", "READY" };			// for use with proc management

#include "util.c"
#include "open_close_seek.c"
#include "read_cat.c"
#include "write_cp.c"
#include "link_unlink.c"
#include "cd_ls_pwd.c"
#include "mkdir_creat.c"
#include "chmod_touch.c"
#include "mount_unmount.c"
#include "proc.c"

int init()
{
  int i, j;
  MINODE *mip;
  PROC   *p;
  MOUNT *mptr;

  printf("init()\n");

	/* Initialize all minodes */
  for (i=0; i<NMINODE; i++)
	{
    mip = &minode[i];
    mip->dev = mip->ino = 0;
    mip->refCount = 0;
    mip->mounted = 0;
    mip->mountptr = 0;
  }

	/* Initialize all procs */
  for (i=0; i<NPROC; i++)
	{
    p = &proc[i];
    p->pid = i;
    p->uid = 0;
    p->cwd = 0;
    p->status = FREE;
    p->childProc = 0;
    p->parentProc = 0;
    p->siblingProc = 0;
    p->nextProc = 0;
    for (j=0; j<NFD; j++)
      p->fd[j] = 0;
      
    enqueue(&freeList, p);
  }
  
  /* Initialize all mounts */
  for (i=0; i<NMOUNT; i++)
  {
		mptr = &MountTable[i];
		mptr->dev = -1;
		mptr->ninodes = 0;
		mptr->nblocks = 0;
		mptr->imap = 0;
		mptr->bmap = 0;
		mptr->iblk = 0;
		mptr->mounted_inode = 0;
		strcpy(mptr->name, "");
		strcpy(mptr->mount_name, "");
	}
}

// load root INODE and set root pointer to it
int mount_root(char *sysname)
{  
	MOUNT *mptr;
  printf("mount_root()\n");
  
  root = iget(dev, 2);
  
  /* Add mount to mount table */
  mptr = &MountTable[0];
  mptr->dev = dev;
	mptr->ninodes = ninodes;
	mptr->nblocks = nblocks;
	mptr->imap = imap;
	mptr->bmap = bmap;
	mptr->iblk = inode_start;
	mptr->mounted_inode = root;
	strcpy(mptr->name, sysname);
	strcpy(mptr->mount_name, "/");
}

int main(int argc, char *argv[ ])
{
  int i, ino;
  char buf[BLKSIZE], arg[256], disk[256];
  PROC *p2;
  
  system("clear");
  
  if (argc > 1)
    strcpy(disk, argv[1]);
  else
  {
  	printf("Enter rootdev name (RETURN for mydisk): ");
		fgets(disk, 256, stdin);
		disk[strlen(disk) - 1] = 0;
		if (strcmp(disk, "")==0)
		{
			strcpy(disk, "mydisk");
		}
  }

	printf("========================================================\n");

  printf("checking EXT2 FS...");
  if ((fd = open(disk, O_RDWR)) < 0)
	{
    printf("open %s failed\n", disk);  
		exit(1);
  }

  dev = fd;

  /* Read super block at 1024 */
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;

  /* Verify it's an ext2 file system */
  if (sp->s_magic != 0xEF53)
	{
      printf("magic = %x is not an ext2 filesystem\n", sp->s_magic);
      exit(1);
  }     
  printf("OK\n");
  ninodes = sp->s_inodes_count;
  nblocks = sp->s_blocks_count;

	printf("ninodes = %d nblocks = %d\n", ninodes, nblocks);

	/* Read Group Descriptor */
  get_block(dev, 2, buf); 
  gp = (GD *)buf;

  bmap = gp->bg_block_bitmap;
  imap = gp->bg_inode_bitmap;
  inode_start = gp->bg_inode_table;
  printf("bmp=%d imap=%d inode_start = %d\n", bmap, imap, inode_start);

  init();  
  mount_root(disk);

  printf("root refCount = %d\n", root->refCount);
  
  printf("creating P0 as running process\n");
  running = dequeue(&freeList);
  running->ppid = 0;
  running->status = READY;
  running->cwd = iget(dev, 2);
  enqueue(&readyQueue, running);
  
  printf("creating P1 process under different user\n");
  p2 = dequeue(&freeList);
  p2->ppid = 0;
  p2->status = READY;
  p2->cwd = iget(dev, 2);
  p2->uid = 2;
  enqueue(&readyQueue, p2);
  
  printf("root refCount = %d\n", root->refCount);

  while(strcmp(cmd, "quit") != 0)
	{
		printf("========================Commands========================\n");
		printf("ls cd pwd mkdir creat rmdir link unlink symlink touch \nchmod stat cat open close pfd mount umount quit\n");
		printf("========================================================\n");
    printf("input command : ");
    fgets(line, 128, stdin);
    line[strlen(line)-1] = 0;
    if (line[0]==0)
      continue;
    pathname[0] = 0;
    cmd[0] = 0;
    
    sscanf(line, "%s %s %s", cmd, pathname, arg);
    printf("cmd=%s pathname=%s\n", cmd, pathname);

		if (strcmp(cmd, "ls")==0)
			list();
		if (strcmp(cmd, "cd")==0)
			change_dir();
		if (strcmp(cmd, "pwd")==0)
    	pwd(running->cwd);
		if (strcmp(cmd, "mkdir")==0)
			make_dir(pathname);
		if (strcmp(cmd, "creat")==0)
			creat_file(pathname);
		if (strcmp(cmd, "rmdir")==0)
			remove_dir(pathname);
		if (strcmp(cmd, "link")==0)
			link_file(pathname, arg);
		if (strcmp(cmd, "unlink")==0)
			unlink_file(pathname);
		if (strcmp(cmd, "symlink")==0)
			sym_link(pathname, arg);
		if (strcmp(cmd, "chmod")==0)
			change_mode(strtol(arg, NULL, 8));		// Convert string to octal int
		if (strcmp(cmd, "touch")==0)
			touch_atime();
		if (strcmp(cmd, "stat")==0)
			stat_file();
		if (strcmp(cmd, "cat")==0)
			cat_file(pathname);
		if (strcmp(cmd, "pfd")==0)
			pfd();
		if (strcmp(cmd, "open")==0)
			open_file(pathname, atoi(arg));
		if (strcmp(cmd, "close")==0)
			close_file(atoi(pathname));
		if (strcmp(cmd, "cp")==0)
			cp_file(pathname, arg);
		if (strcmp(cmd, "mv")==0)
			mv_file(pathname, arg);
		if (strcmp(cmd, "mount")==0)
			mount_fs(pathname, arg);
		if (strcmp(cmd, "umount")==0)
			umount_fs(pathname);
		if (strcmp(cmd, "cs")==0)
			cs_proc();
		if (strcmp(cmd, "ps")==0)
			ps_procs();
		if (strcmp(cmd, "fork")==0)
			fork_proc();
		if (strcmp(cmd, "kill")==0)
			kill_proc();
		if (strcmp(cmd, "quit")==0)
			quit();
  }
}
 
int quit()
{
  int i;
  MINODE *mip;
  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    if (mip->refCount > 0 && mip->dirty)
      iput(mip);
  }
}
