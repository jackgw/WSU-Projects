/*********** mkdir_creat.c file ****************/

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

/*********** Make Directory ***********/

int make_dir()
{
	MINODE *mip, *pip;
	char *parent, *child, parenttemp[256], childtemp[256];
	int pino;

	/* change mip to root or cwd */
	if (pathname[0] == '/')			// Absolute
	{
		mip = root;
		dev = root->dev;
	}
	else												// Relative
	{
		mip = running->cwd;
		dev = running->cwd->dev;
	}

	/* Get parent and child names */
	strcpy(parenttemp, pathname);
	parent = dirname(parenttemp);
	strcpy(childtemp, pathname);
	child = basename(childtemp);
	
	printf("parent: %s, child: %s\n", parent, child);

	/* Get minode of parent in memory */
	pino = getino(parent);
	pip = iget(dev, pino);

	/* parent must exist and be a dir */
	if (pip == 0 || (pip->INODE.i_mode & 0xF000) != 0x4000)
	{
		printf("Parent does not exist or is not a DIR\n");
		return 0;
	}

	/* child must not exist in parent dir */
	if (search(pip, child) != 0)
	{
		printf("Directory already exists\n");
		return 0;
	}

	mymkdir(pip, child);

	/* Increment parent link count by 1 */
	pip->INODE.i_links_count++;
	pip->dirty = 1;
	pip->INODE.i_atime = time(0L);

	iput(pip);
}

int mymkdir(MINODE *pip, char *name)
{
	MINODE *mip;
	int ino, bno;
	int i;

	/* Allocate inode and disk block for new DIR */
	ino = ialloc(dev);
	bno = balloc(dev);
	printf("ino: %d, bno: %d\n", ino, bno);

	/* load inode number into mip */
	mip = iget(dev, ino);

	/* Set up mip->INODE as a DIR */
	INODE *ip = &(mip->INODE);
	
	ip->i_mode = 0x41ED;					// DIR type and permissions
	ip->i_uid = running->uid;			// Owner uid
	ip->i_gid = running->gid;			// Group ID
	ip->i_size = BLKSIZE;					// Size in bytes
	ip->i_links_count = 2;				// Links = 2 (. and ..)
	ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);		// Current time
	ip->i_blocks = 2;							// LINUX: blocks count in 512-byte chunks
	ip->i_block[0] = bno;					// One data block
	
	for(i=1; i<=14; i++)
		ip->i_block[i] = 0;

	mip->dirty = 1;								// Mark minode as dirty
	iput(mip);										// Write INODE to disk

	/* Write . and .. entries into a buf[] of BLKSIZE */
	
	char buf[BLKSIZE];
	DIR *dp = (DIR *)buf;
	
	dp->inode = ino;
	dp->rec_len = 12;
	dp->name_len = 1;
	dp->name[0] = '.';

	dp = (char *)dp + 12;
	dp->inode = pip->ino;
	dp->rec_len = BLKSIZE-12;
	dp->name_len = 2;
	dp->name[0] = dp->name[1] = '.';

	put_block(dev, bno, buf);

	printf("------- Dir [%s] created -------\n", name);
	ls_dir(mip);

	/* Enter name entry into parent DIR */
	enter_name(pip, ino, name);
}

int enter_name(MINODE *pip, int myino, char *myname)
{
	int i;
	char buf[BLKSIZE], temp[256];
	DIR *dp;
	char *cp;

	int need_length = (4*  ((8 + strlen(myname) + 3) / 4));

	for (i=0; i<12; i++)					// Assume only 12 direct blocks
	{
		if (pip->INODE.i_block[i] == 0)
			break;

		/* Step to last entry in block */
		get_block(pip->dev, pip->INODE.i_block[i], buf);
		dp = (DIR *)buf;
		cp = buf;		

		while (cp + dp->rec_len < buf + BLKSIZE)
		{
			cp += dp->rec_len;
			dp = (DIR *)cp;
		}

		int ideal_length = (4*  ((8 + dp->name_len + 3) / 4));
		int remain = dp->rec_len - ideal_length;
		if (remain >= need_length)
		{
			printf("Trimming last entry...\n");
			/* Enter new entry and trim previous entry to ideal length */
			dp->rec_len = ideal_length;
			cp += dp->rec_len;
			dp = (DIR *)cp;

			strncpy(dp->name, myname, strlen(myname));
			dp->name[strlen(myname)] = 0;

			dp->name_len = strlen(myname);
			dp->rec_len = remain;
			dp->inode = myino;

			/* Write block to disk */
			put_block(pip->dev, pip->INODE.i_block[i], buf);
			return 1;
		}
	}

	printf("Allocating new block...\n");

	/* allocate new block */
	int blk = balloc(pip->dev);
	pip->INODE.i_size += BLKSIZE;
	
	get_block(pip->dev, blk, buf);
	dp = (DIR *)buf;
	
	strncpy(dp->name, myname, strlen(myname));
	dp->name[strlen(myname)] = 0;

	dp->name_len = strlen(myname);
	dp->rec_len = BLKSIZE;
	dp->inode = myino;

	/* Write block to disk */
	put_block(pip->dev, myino, buf);
	return 1;
}

/*********** Create File ***********/

int creat_file(char *pathname)
{
	MINODE *mip, *pip;
	char *parent, *child, parenttemp[256], childtemp[256];
	int pino;

	/* change mip to root or cwd */
	if (pathname[0] == '/')			// Absolute
	{
		mip = root;
		dev = root->dev;
	}
	else												// Relative
	{
		mip = running->cwd;
		dev = running->cwd->dev;
	}

	/* Get parent and child names */
	strcpy(parenttemp, pathname);
	parent = dirname(parenttemp);
	strcpy(childtemp, pathname);
	child = basename(childtemp);

	printf("parent: %s, child: %s\n", parent, child);
	printf("pathname: %s\n", pathname);

	/* Get minode of parent in memory */
	pino = getino(parent);
	pip = iget(dev, pino);

	/* parent must exist and be a dir */
	if (pip == 0 || (pip->INODE.i_mode & 0xF000) != 0x4000)
	{
		printf("Parent does not exist or is not a DIR\n");
		return 0;
	}

	/* child must not exist in parent dir */
	if (search(pip, child) != 0)
	{
		printf("Directory already exists\n");
		return 0;
	}

	mycreat(pip, child);

	/* Modify parent atime and dirty */
	pip->dirty = 1;
	pip->INODE.i_atime = time(0L);

	iput(pip);
}

int mycreat(MINODE *pip, char *name)
{
	MINODE *mip;
	int ino;
	int i;

	/* Allocate inode and disk block for new DIR */
	ino = ialloc(dev);
	printf("ino: %d\n", ino);

	/* load inode number into mip */
	mip = iget(dev, ino);

	/* Set up mip->INODE as a FILE */
	INODE *ip = &(mip->INODE);
	
	ip->i_mode = 0x81A4;					// FILE type and permissions
	ip->i_uid = running->uid;			// Owner uid
	ip->i_gid = running->gid;			// Group ID
	ip->i_size = 0;								// Size in bytes
	ip->i_links_count = 1;				// Links = 1 (. and ..)
	ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);		// Current time
	ip->i_blocks = 0;							// No data block

	mip->dirty = 1;								// Mark minode as dirty
	iput(mip);										// Write INODE to disk

	printf("------- File [%s] created -------\n", name);

	/* Enter name entry into parent DIR */
	enter_name(pip, ino, name);
	
	return ino;
}

/*********** Remove Directory ***********/
int remove_dir(char *pathname)
{
	int ino, pino, count, i;
	MINODE *mip, *pip;
	char buf[BLKSIZE];
	DIR *dp;
	char *cp, *parent, *child, parenttemp[256], childtemp[256];

	/* Get parent and child names*/
	strcpy(parenttemp, pathname);
	parent = dirname(parenttemp);
	strcpy(childtemp, pathname);
	child = basename(childtemp);
	printf("child name: %s, parent name: %s\n", child, parent);

	/* Get minode of pathname */
	ino = getino(pathname);
	mip = iget(dev, ino);

	/* check ownership */
	
	if (running->uid != 0 && mip->INODE.i_uid != running->uid)
	{
		printf("Invalid user permissions\n");
		iput(mip);
		return -1;
	}

	/* check dir type, not busy, empty */

	if (mip->refCount != 1)
	{
		printf("Busy: refcount = %d\n", mip->refCount);
		iput(mip);
		return -1;
	}

	if ((mip->INODE.i_mode & 0xF000) != 0x4000)
	{
		printf("Not dir\n");
		iput(mip);
		return -1;
	}

	if (mip->INODE.i_links_count < 2)
	{
		get_block(mip->dev, mip->INODE.i_block[0], buf);
		dp = (DIR *)buf;
		cp = buf;
	
		count = 0;
		while (cp < buf + BLKSIZE)
		{
			count++;
			cp += dp->rec_len;
			dp = (DIR *)cp;
		}
			
		if (count > 2)
		{
			printf("Not Empty");
			iput(mip);
			return -1;
		}
	}

	printf("Valid file\n");

	/* Deallocate block and inode */
	for (i=0; i<12; i++)
	{
		if (mip->INODE.i_block[i] == 0)
			continue;
		bdealloc(mip->dev, mip->INODE.i_block[i]);
	}

	idealloc(mip->dev, mip->ino);
	iput(mip);

	/* Get parent DIR into pip */
	pino = getino(parent);
	pip = iget(mip->dev, pino);

	/* Remove child's entry from parent DIR */
	rm_child(pip, mip->ino);

	/* Decrement pip's link_count by 1 */
	pip->INODE.i_links_count;
	pip->INODE.i_atime = pip->INODE.i_mtime = time(0L);

	/* Write parent data block back to disk */
	pip->dirty = 1;
	iput(pip);
	return 1;	
}

int rm_child(MINODE *pip, int ino)
{
	char buf[BLKSIZE];
	DIR *dp, *prev_dp;
	char *cp, *start_1, *start_2;
	int i, j;
	
	/* Find child entry */
	get_block(pip->dev, pip->INODE.i_block[0], buf);
	dp = (DIR *)buf;
	cp = buf;

	while (dp->inode != ino)
	{
		printf("Name: %s\n", dp->name);
		prev_dp = dp;
		cp += dp->rec_len;
		dp = (DIR *)cp;
	}
	
	printf("child entry found, name: %s\n", dp->name);

	/* Remove child entry */
	if (cp + dp->rec_len == buf + BLKSIZE)	// Last entry in data block
	{
		prev_dp->rec_len += dp->rec_len;
		put_block(pip->dev, pip->INODE.i_block[0], buf);
	}

	else if (dp->rec_len == BLKSIZE)	// First (only) entry in data block
	{
		bdealloc(pip->dev, pip->INODE.i_block[0]);
		pip->INODE.i_size -= BLKSIZE;

		for (j = 0; j < 12 && pip->INODE.i_block[j] != 0; j++)
		{
			pip->INODE.i_block[j] = pip->INODE.i_block[j + 1];
		}

		pip->INODE.i_block[j] = 0;
	}
	
	else	// Middle of a data block
	{
		start_1 = cp;

		cp += dp->rec_len;
		dp = (DIR *)cp;

		start_2 = cp;
	
		/* Advance to last entry */
		while (cp + dp->rec_len < buf + BLKSIZE)
		{
			cp += dp->rec_len;
			dp = (DIR *)cp;
		}

		/* At last entry */
		printf("last entry: %s\n", dp->name);
		dp->rec_len += start_2 - start_1;
		memmove(start_1, start_2, (cp + dp->rec_len) - start_2);

		/* write back block */
		put_block(pip->dev, pip->INODE.i_block[0], buf);
	}
}





