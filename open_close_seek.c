/*********** open_close_seek.c file ****************/

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

int open_file(char *filename, int flag)
{
	int ino, i;
	MINODE *mip;

	/* Get filename inode */
	if (filename[0]=='/')
		dev = root->dev;
	else
		dev = running->cwd->dev;

	ino = getino(filename);
	printf("Open: filename ino: %d\n", ino);
	
	/* check if file needs to be created */
	if (ino == 0)
	{
		creat_file(filename);
		ino = getino(filename);
	}

	mip = iget(dev, ino);

	/* check mip->INODE.i_mode to verify REG file and permissions */
	if (!S_ISREG(mip->INODE.i_mode))
	{
		printf("Not a regular file\n");
		return -1;
	}

	if (running->uid != 0 && running->uid != mip->INODE.i_uid)
	{
		printf("Not owner or SU: permission denied\n");
		return -1;
	}

	/* Check file already open with incompatable mode */
	for(i=0; i<NFD; i++)
	{
		if (running->fd[i] != 0)
		{
			if (running->fd[i]->mptr->ino == mip->ino && running->fd[i]->mode != 0)
			{
				printf("File open with incompatable mode\n");
				return -1;
			}
		}
	}

	/* Allocate a FREE OpenFileTable (OFT) and fill in values */
	OFT *oftp = malloc(sizeof(OFT));
	
	oftp->mode = flag;		// 0|1|2|3 for R|W|RW|APPEND
	oftp->refCount = 1;
	oftp->mptr = mip;

	/* Set OFT Offset */
	switch(flag)
	{
		case 0:			// R: offset = 0
			oftp->offset = 0;
			break;
		case 1:			// W: Truncate size to 0
			truncate(mip);
			oftp->offset = 0;
			break;
		case 2:			// RW: Do not truncate
			oftp->offset = 0;
			break;
		case 3:			// Append: add offset of inode size
			oftp->offset = mip->INODE.i_size;
			break;
		default:
			printf("Invalid mode\n");
			return -1;
	}

	/* find the smallest i in running PROC's fd[] where fd[i] is null */
	for (i=0; i<NFD; i++)
	{
		if (running->fd[i] == 0)
			break;
	}
	
	if (running->fd[i] != 0)
	{
		printf("No free OFT in fd[]\n");
		return -1;
	}
	
	/* set running->fd[i] to oftp */
	running->fd[i] = oftp;

	/* Update INODE's time */
	if (flag == 0)
	{
		mip->INODE.i_atime = time(0L);
	}
	else
	{
		mip->INODE.i_atime = time(0L);
		mip->INODE.i_mtime = time(0L);
	}

	/* Mark MINODE as dirty */
	mip->dirty = 1;

	/* Return File Descriptor (i) */
	return i;
}

int truncate(MINODE *mip)
{
	int i, j;
	char indirect_buf[BLKSIZE], dbl_indirect_buf[BLKSIZE];
	int *indirect_blk, *dbl_indirect_blk;

	/* Release Direct blocks */
	printf("Releasing Direct Blocks...\n");
	for (i=0; i < 12; i++)
	{
		bdealloc(mip->dev, mip->INODE.i_block[i]);
	}

	/* Release Indirect Blocks */
	printf("Releasing Indirect Blocks...\n");
	if (mip->INODE.i_block[12] != 0)
	{
		get_block(dev, mip->INODE.i_block[12], indirect_buf);
		indirect_blk = (int *)indirect_buf;

		for (j = 0; j < 256; j++)
		{
			bdealloc(mip->dev, *indirect_blk);
			indirect_blk++;
		}
	}

	
	/* Release Double Indirect Blocks */
	printf("Releasing Double Indirect Blocks...\n");
	if (mip->INODE.i_block[13] != 0)
	{
		get_block(dev, mip->INODE.i_block[13], indirect_buf);
		indirect_blk = indirect_buf;

		for (i = 0; i < 256; i++)
		{
			get_block(dev, *indirect_blk, dbl_indirect_buf);
			dbl_indirect_blk = (int *)dbl_indirect_buf;

			for (j = 0; j < 256; j++)
			{
				bdealloc(mip->dev, *dbl_indirect_blk);
				dbl_indirect_blk++;
			}

			indirect_blk++;
		}
	}

	/* Update time, size, dirty */
	mip->INODE.i_atime = mip->INODE.i_mtime = time(0L);
	mip->INODE.i_size = 0;
	mip->dirty = 1;
}

int close_file(int fd)
{
	OFT *oftp;

	/* Verify fd in range */
	if (fd >= NFD)
		return 0;
	
	/* Verify running->fd[fd] is pointing to an OFT */
	if (running->fd[fd] == 0)
		return 0;

	/* Close running fd */
	printf("closing fd...\n");
	oftp = running->fd[fd];
	running->fd[fd] = 0;
	oftp->refCount--;
	
	if (oftp->refCount > 0)
		return 1;

	/* Last user: dispose of MINODE */
	iput(oftp->mptr);

	return 1;
}

int lseek_file(int fd, int position)
{
	int originalPosition;
	OFT *oftp;

	/* Fint OFT entry from fd */
	oftp = running->fd[fd];
	
	/* Verify valid position */
	if (position < 0 || position >= oftp->mptr->INODE.i_size)
	{
		printf("lseek: invalid position\n");
		return -1;
	}

	/* Change offset to new position and return old position */
	originalPosition = oftp->offset;
	oftp->offset = position;

	return originalPosition;
}

int pfd()
{
	int i;
	char mode[7];

	printf("fd |  mode  | offset | INODE\n");

	/* Print all currently open files */
	for (i=0; i<NFD; i++)
	{
		if (running->fd[i] != 0)
		{
			switch(running->fd[i]->mode)
			{
				case 0:			// R
					strcpy(mode, "READ");
					break;
				case 1:			// W
					strcpy(mode, "WRITE");
					break;
				case 2:			// RW
					strcpy(mode, "R/W");
					break;
				case 3:			// Append
					strcpy(mode, "APPEND");
					break;
			}

			printf("%2d | %6s | %6d | [%d, %d]\n", i, mode, running->fd[i]->offset, running->fd[i]->mptr->dev, running->fd[i]->mptr->ino);
		}
	}
}
