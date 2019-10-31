/************* cd_ls_pwd.c file **************/

/**** globals defined in main.c file ****/
extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC   proc[NPROC], *running;
extern char   gpath[256];
extern char   *name[64];
extern int    n;
extern int fd, dev;
extern int nblocks, ninodes, bmap, imap, inode_start;
extern char line[256], cmd[32], pathname[256];

#define OWNER  000700
#define GROUP  000070
#define OTHER  000007

change_dir()
{
	if (pathname == 0)
	{
		iput(running->cwd);
		running->cwd = root;
	}
	else
	{
		int ino = getino(pathname);
		MINODE *mip = iget(dev, ino);

		//verify mip->INODE is a DIR
		if ((mip->INODE.i_mode & 0xF000) == 0x4000)
		{
			iput(running->cwd);
			running->cwd = mip;
		}
		else
		{
			printf("cd failed: not a directory\n");
		}
	}
}


int list()
{
	/* List CWD */
  if (strcmp(pathname, "") == 0)
	{
		ls_dir(running->cwd);
	}
	/* List pathname DIR */
	else
	{
		int ino = getino(pathname);
		MINODE *mip = iget(dev, ino);

		/* Verify mip->INODE is a DIR */
		if ((mip->INODE.i_mode & 0xF000) == 0x4000)
		{
			ls_dir(mip);
		}
		else
		{
			printf("ls failed: not a directory\n");
		}

		iput(mip);
	}
}

int ls_dir(MINODE *mip)
{
	char buf[BLKSIZE], temp[64], *cp;
	DIR *dp;

	get_block(dev, mip->INODE.i_block[0], buf);
	
	dp = (DIR *)buf;
	cp = buf;

	while(cp < buf + BLKSIZE)
	{
		strncpy(temp, dp->name, dp->name_len);
		temp[dp->name_len] = 0;

		ls_file(dp->inode, temp);

		cp += dp->rec_len;
		dp = (DIR *)cp;
	}
}

int ls_file(int ino, char *name)
{
	int i;
	char ftime[64];
	char *t1 = "xwrxwrxwr-------";
	char *t2 = "----------------";
	char *link;

	MINODE *mip = iget(dev, ino);
	INODE *ip = &(mip->INODE);

	/* Print d|-|l by file type */
	if ((ip->i_mode & 0xF000) == 0x8000)
		printf("%c", '-');
	if ((ip->i_mode & 0xF000) == 0x4000)
		printf("%c", 'd');
	if ((ip->i_mode & 0xF000) == 0xA000)
		printf("%c", 'l');
	
	/* print r|w|x|- for imode bits */
	for (i=8; i >= 0; i--)
	{
		if (ip->i_mode & (1 << i))		// print r|w|x
			printf("%c", t1[i]);
		else
			printf("%c", t2[i]);				// or print '-'
	}
	
	/* Print i_links_count, gid, uid, i_size */
	printf("%4d ", ip->i_links_count);
	printf("%4d ", ip->i_gid);
	printf("%4d ", ip->i_uid);
	printf("%8d ", ip->i_size);

	/* Print time */
	strcpy(ftime, ctime(&ip->i_ctime));
	ftime[strlen(ftime)-1] = 0;
	printf("%s ", ftime);

	/* Print name */
	printf("%s", name);

	/* Check link */
	if ((ip->i_mode & 0xF000) == 0xA000)
	{
		link = read_link(name);
		printf(" -> %s", link);
	}

	printf("\n");

	iput(mip);
}

int pwd(MINODE *wd)
{
	if (wd == root)
		printf("/");
	else
		rpwd(wd);

	printf("\n");
}

int rpwd(MINODE *wd)
{
	char buf[BLKSIZE], *cp;
	char my_name[50];
	int myino, parentino;
	DIR *dp;
	MINODE *pip;

	if (wd == root)
		return 0;

	/* from i_block[0] of wd->INODE: get myino of . parentino of .. */
	get_block(wd->dev, wd->INODE.i_block[0], buf);
	dp = (DIR *)buf;
	myino = dp->inode;

	cp = buf + dp->rec_len;
	dp = (DIR *)cp;

	parentino = dp->inode;
	
	pip = iget(wd->dev, parentino);

	/* from pip->INODE.i_block[0]: get myname string as LOCAL */
	get_block(pip->dev, pip->INODE.i_block[0], buf);
	dp = (DIR *)buf;
	cp = buf;

	while (dp->inode != wd->ino)
	{	
		cp += dp->rec_len;
		dp = (DIR *)cp;
	}

	strcpy(my_name, dp->name);
	my_name[dp->name_len] = 0;

	rpwd(pip);  // recursive call rpwd() with pip
	iput(pip);

	printf("/%s", my_name);
}



