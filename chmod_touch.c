/*********** chmod_touch.c file ****************/

/**** globals defined in main.c file ****/
extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC   proc[NPROC], *running;
extern char   gpath[256];
extern char   *name[64];
extern int    n;
extern int    fd, dev;
extern int    nblocks, ninodes, bmap, imap, inode_start;
extern char   line[256], cmd[32], pathname[256];

int change_mode(int mode)
{
	int ino = getino(pathname);
	MINODE *mip = iget(dev, ino);

	mip->INODE.i_mode |= mode;

	mip->dirty = 1;
	iput(mip);
}

int touch_atime()
{
	int ino = getino(pathname);
	MINODE *mip = iget(dev, ino);

	mip->INODE.i_atime = time(0L);

	mip->dirty = 1;
	iput(mip);
}

int stat_file()
{
	struct stat myst;

	int ino = getino(pathname);
	MINODE *mip = iget(dev, ino);

	myst.st_dev = mip->dev;										// Device
	myst.st_ino = mip->ino;										// Inode number

	myst.st_mode = mip->INODE.i_mode;					// Protection
	myst.st_nlink = mip->INODE.i_links_count;	// Number of hard links
	myst.st_uid = mip->INODE.i_uid;						// User ID of owner
	myst.st_gid = mip->INODE.i_gid;						// Group ID of owner
	myst.st_size = mip->INODE.i_size;					// Total size
	myst.st_blocks = mip->INODE.i_blocks;			// Number of blocks allocated
	myst.st_atime = mip->INODE.i_atime;				// Last access time
	myst.st_mtime = mip->INODE.i_mtime;				// Last Modification time
	myst.st_ctime = mip->INODE.i_ctime;				// Last change time

	myst.st_blksize = BLKSIZE;								// Blocksize for system I/O

	iput(mip);

	/* Print information */
	printf("Dev: %d\n", myst.st_dev);
	printf("Ino: %d\n", myst.st_ino);
	printf("Mode: %o (octal)\n", myst.st_mode);
	printf("UID: %d\n", myst.st_uid);
	printf("GID: %d\n", myst.st_gid);
	printf("Nlink: %d\n", myst.st_nlink);
	printf("Size: %d\n", myst.st_size);
	printf("Access Time: %s", ctime(&myst.st_atime));
	printf("Mod Time: %s", ctime(&myst.st_mtime));
	printf("Change Time: %s", ctime(&myst.st_ctime));
}
