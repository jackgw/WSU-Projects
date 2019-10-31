/*********** read_cat.c file ****************/

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

int read_file(int fd, char buf[], int nbytes)
{
	/* Check for R or RW mode */
	if (running->fd[fd]->mode == 0 || running->fd[fd]->mode == 2)
	{
		return myread(fd, buf, nbytes);
	}
	else
	{
		printf("File not opened for R or RW\n");
		return 0;
	}
}

int myread(int fd, char buf[], int nbytes)
{
	int count;						// Number of bytes read
	int avil;							// Bytes currently available to read
	int lbk;							// Logical block
	int startbyte;				// byte to start reading at
	int remain;						// Number of bytes remaining in the logical block
	int blk;
	int iblk, dbl_iblk;
	int ioff;
	int readsize;
	char *cq;
	char *cp;
	char readbuf[BLKSIZE], ibuf[BLKSIZE], dbl_ibuf[BLKSIZE];
	OFT *oftp;
	MINODE *mip;

	oftp = running->fd[fd];			// Get oft pointer for opened file
	count = 0;
	avil = oftp->mptr->INODE.i_size - oftp->offset;		// Bytes currently available to read (size of file - offset within file))
	cq = buf;
	mip = oftp->mptr;

	while (nbytes != 0 && avil != 0)		// Until there are no more bytes to be read
	{
		/* Compute Logical Block number and startbyte for reading */
		
		lbk = oftp->offset / BLKSIZE;	
		startbyte = oftp->offset % BLKSIZE;
		printf("lblk: %d, startbyte: %d: ", lbk, startbyte);
		
		/* Read block numebr from direct, indirect, or double indirect block */
		if (lbk < 12) 		// lbk is direct blk
		{
			printf("Reading direct block...\n");
			blk = mip->INODE.i_block[lbk];
		}
		else if (lbk >= 12 && lbk < 256 + 12)		// lbk is indirect block
		{
			printf("Reading indirect block...\n");
			
			get_block(mip->dev, mip->INODE.i_block[12], ibuf);
			blk = ibuf[lbk - 12];
		}
		else		// lbk is double indirect block
		{
			printf("Reading double indirect block...\n");
			
			get_block(mip->dev, mip->INODE.i_block[13], ibuf);
			
			iblk = (lbk - 256 - 12) / 256;
			ioff = (lbk - 256 - 12) % 256;
			
			get_block(mip->dev, ibuf[iblk], dbl_ibuf);
			
			blk = dbl_ibuf[ioff];
		}

		/* read block data into readbuf */
		get_block(mip->dev, blk, readbuf);

		cp = readbuf + startbyte;				// Set cp to position to start reading
		remain = BLKSIZE - startbyte;		// Set remain to bytes left in the block
		printf("nbytes=%d, avil=%d, remain=%d, blk=%d : ", nbytes, avil, remain, blk);

		/* Optimized Read */
		if (nbytes <= remain && nbytes <= avil)		// space to read full nbytes from fd into buf
			readsize = nbytes;
		else if (nbytes <= avil)									// limited by bytes left in buf
			readsize = remain;
		else																			// limited by bytes left in fd
			readsize = avil;
		
		if (remain > 0)
		{
			strncpy(cq, cp, readsize);		// readbytes into cq from readbuf at cp
			cp += readsize;								// Inc cp and cq by readsize
			cq += readsize;
			count += readsize;
			nbytes -= readsize;
			remain -= readsize;
			avil -= readsize;
			oftp->offset += readsize;
			
			if (oftp->offset > mip->INODE.i_size)
			{
				mip->INODE.i_size += readsize;
			}
		}
		
		/*
		while (remain > 0)
		{
    	*cq++ = *cp++;             // copy byte from readbuf[] into buf[]
      oftp->offset++;           // advance offset 
      count++;                  // inc count as number of bytes read
      avil--; nbytes--;  remain--;
      if (nbytes <= 0 || avil <= 0) 
      	break;
    }
    */
	}

	printf("myread: read %d chars from fd %d\n", count, fd);
	return count;
}

int cat_file(char *pathname)
{
	char buf[BLKSIZE];
	int n_read, fd;
	int ino;
	MINODE *mip;

	/* Make sure pathname is a file */
	ino = getino(pathname);
	printf("Cat: pathname ino=%d\n", ino);
	if (ino == 0)
	{
		printf("Cat: file does not exist\n");
		return 0;
	}
	mip = iget(running->cwd->dev, ino);
	
	/* Check REG or LNK (NOT DIR)*/
	if (((mip->INODE.i_mode & 0xF000) != 0x8000) && (mip->INODE.i_mode & 0xF000) != 0xA000)
	{
		printf("Cat: not a file\n");
		iput(mip);
		return 0;
	}
	iput(mip);

	/* Open file for reading */
	printf("Cat: opening file...\n");
	fd = open_file(pathname, 0);
	
	if (fd == -1)
	{
		printf("Cat: open failed\n");
		return 0;
	}

	/* Read contents of file */
	printf("Cat: reading file...\n");
	while (n_read = read_file(fd, buf, BLKSIZE))
	{
		write(1, buf, n_read);
	}

	printf("Cat: closing file...\n");
	close_file(fd);
}
