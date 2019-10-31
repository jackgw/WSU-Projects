/************* link_unlink.c file **************/

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

int link_file(char *pathname, char *newname)
{
	int oino, pino;
	MINODE *omip, *pmip;
	char *parent, *child, parenttemp[256], childtemp[256];

	/* Get inode of original file */
	oino = getino(pathname);
	omip = iget(dev, oino);

	/* Check REG or LNK (NOT DIR)*/
	if (((omip->INODE.i_mode & 0xF000) != 0x8000) && (omip->INODE.i_mode & 0xF000) != 0xA000)
	{
		printf("invalid file mode\n");
		iput(omip);
		return 0;
	}
	
	/* get names of parent and child */
	strcpy(parenttemp, newname);
	parent = dirname(parenttemp);

	strcpy(childtemp, newname);
	child = basename(childtemp);

	/* newfile must not yet exist */
	if (getino(newname) != 0)
	{
		iput(omip);
		return 0;
	}

	/* Add newname entry */
	pino = getino(parent);
	pmip = iget(dev, pino);

	enter_name(pmip, oino, child);

	/* Increment INODE links_count */
	omip->INODE.i_links_count++;
	omip->dirty = 1;
	iput(omip);
	iput(pmip);
}

int unlink_file(char *pathname)
{
	int ino, pino, i;
	MINODE *mip, *pmip;
	char *parent, *child, parenttemp[256], childtemp[256];

	/* Get parent DIR */
	strcpy(parenttemp, pathname);
	parent = dirname(parenttemp);

	strcpy(childtemp, pathname);
	child = basename(childtemp);

	pino = getino(parent);
	pmip = iget(dev, pino);

	/* get pathname minode */
	ino = getino(pathname);
	mip = iget(dev, ino);
	
	/* Check REG or LNK (not DIR)	*/
	if (((mip->INODE.i_mode & 0xF000) != 0x8000) && (mip->INODE.i_mode & 0xF000) != 0xA000)
	{
		printf("invalid file mode\n");
		iput(mip);
		return 0;
	}

	/* check ownership */
	if (running->uid != 0 && mip->INODE.i_uid != running->uid)
	{
		printf("Invalid user permissions\n");
		iput(mip);
		return 0;
	}

	/* Remove child from parent dir */
	rm_child(pmip, mip->ino);
	pmip->dirty = 1;
	iput(pmip);
	
	/* Decrement INODE's link_count */
	if (mip->INODE.i_links_count > 0)
	{
		mip->dirty = 1;
	}
	else	// remove filename
	{
		for (i=0; i<12; i++)
		{
			if (mip->INODE.i_block[i] == 0)
				continue;
			bdealloc(mip->dev, mip->INODE.i_block[i]);
		}
		idealloc(mip->dev, mip->ino);
	}

	iput(mip);
}

int sym_link(char *pathname, char *newname)
{
	int oino, pino, ino;
	MINODE *pmip, *mip;
	char *parent, *child, parenttemp[256], childtemp[256];

	/* Verify old file DOES exist */
	oino = getino(pathname);	

	if (oino == 0)
		return 0;
	
	/* Get parent and child names */
	strcpy(parenttemp, newname);
	parent = dirname(parenttemp);

	strcpy(childtemp, newname);
	child = basename(childtemp);

	/* Verify new file does NOT exist */
	if (getino(newname) != 0)
		return 0;

	/* Get parent MINODE */
	pino = getino(parent);
	pmip = iget(dev, pino);
	
	/* Creat new LNK file */
	mycreat(pmip, child);

	ino = getino(newname);
	mip = iget(dev, ino);

	iput(pmip);

	/* Change type to LNK */
	mip->INODE.i_mode = 0xA000;
	
	/* write name into i_block[] */
	memcpy(mip->INODE.i_block, pathname, strlen(pathname));
	mip->INODE.i_size = strlen(pathname);
	
	iput(mip);
}

char *read_link(char *filename)
{
	int ino;
	MINODE *mip;
	char link[256];

	ino = getino(filename);
	mip = iget(dev, ino);

	if ((mip->INODE.i_mode & 0xF000) != 0xA000)
	{
		return 0;
	}
	
	strcpy(link, mip->INODE.i_block);
	iput(mip);

	return link;
}
