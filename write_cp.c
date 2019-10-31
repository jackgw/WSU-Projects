/*********** write_cp.c file ****************/

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

int write_file(int fd, char buf[], int nbytes)
{
	/* Verify fd is opened for W or RW or Append */
	if (running->fd[fd]->mode == 1 || running->fd[fd]->mode == 2 || running->fd[fd]->mode == 2)
	{
		return mywrite(fd, buf, nbytes);
	}
	else
	{
		printf("File not opened for W or RW or Append");
	}
}

int mywrite(int fd, char buf[], int nbytes)
{
	int lbk;							// Logical block
	int startbyte;				// byte to start writing at
	int remain;						// Number of bytes remaining in the logical block
	int count;						// Number of bytes written
	int blk;
	int writesize;
	int iblk, dbl_iblk;
	int ioff;
	int i;
	char *cq;
	char *cp;
	char writebuf[BLKSIZE], ibuf[BLKSIZE], dbl_ibuf[BLKSIZE];
	OFT *oftp;
	MINODE *mip;
	
	oftp = running->fd[fd];
	count = 0;
	cq = buf;
	mip = oftp->mptr;

	while (nbytes > 0)
	{
		/* Compute lbk and startbyte */
		lbk = oftp->offset / BLKSIZE;
		startbyte = oftp->offset % BLKSIZE;
		
		/* Get block from direct, indirect, or double indirect blocks */
		if (lbk < 12)		// Direct block
		{
			if(mip->INODE.i_block[lbk] == 0)		// No block yet, must allocate
			{
				mip->INODE.i_block[lbk] = balloc(mip->dev);
			}
			
			blk = mip->INODE.i_block[lbk];
		}
		else if (lbk >= 12 && lbk < 256 + 12)		// Indirect Block
		{
			/* Indirect container block not allocated */
			if (!mip->INODE.i_block[12])
			{
				mip->INODE.i_block[12] = balloc(mip->dev);
				
				/* Zero out new block */
				get_block(mip->dev, mip->INODE.i_block[12], ibuf);
				
				for (i=0; i<BLKSIZE; i++)
				{
					ibuf[i] = 0;
				}
				
				put_block(mip->dev, mip->INODE.i_block[12], ibuf);
			}
			
			/* Get blk munber */
			get_block(mip->dev, mip->INODE.i_block[12], ibuf);
			blk = ibuf[lbk - 12];
			
			/* Specific block not allocated yet */
			if (!blk)
			{
				ibuf[lbk - 12] = balloc(mip->dev);
				blk = ibuf[lbk - 12];
				put_block(mip->dev, mip->INODE.i_block[12], ibuf);
			}
		}
		else		// Double indirect block
		{
			/* Double indirect container clock not allocated */
			if (!mip->INODE.i_block[13])
			{
				mip->INODE.i_block[13] = balloc(mip->dev);
				
				/* Zero out new block */
				get_block(mip->dev, mip->INODE.i_block[13], ibuf);
				
				for (i=0; i<BLKSIZE; i++)
				{
					ibuf[i] = 0;
				}
				
				put_block(mip->dev, mip->INODE.i_block[13], ibuf);
			}
			
			/* Compute secondary block container */
			get_block(mip->dev, mip->INODE.i_block[13], ibuf);
			
			iblk = (lbk - 256 - 12) / 256;
			ioff = (lbk - 256 - 12) % 256;
			
			dbl_iblk = ibuf[iblk];
			
			/* if secondary container block is not allocated */
			if (!dbl_iblk)
			{
				ibuf[iblk] = balloc(mip->dev);
				dbl_iblk = ibuf[iblk];
				
				get_block(mip->dev, dbl_iblk, ibuf);
				
				for (i=0; i<BLKSIZE; i++)
				{
					ibuf[i] = 0;
				}
				
				put_block(mip->dev, mip->INODE.i_block[13], ibuf);
			}
			
			/* Get block number from secondary container block */
			get_block(mip->dev, dbl_iblk, dbl_ibuf);
			
			blk = dbl_ibuf[ioff];
			
			/* Specific block not yet allocated */
			if (!blk)
			{
				dbl_ibuf[ioff] = balloc(mip->dev);
				blk = dbl_ibuf[ioff];
				put_block(mip->dev, blk, ibuf);
			}
		}
		
		/* Write to data block */
		get_block(mip->dev, blk, writebuf);
		cp = writebuf + startbyte;
		remain = BLKSIZE - startbyte;
		
		/* Optimized Write */
		if (nbytes <= remain)		// space to write full nbytes from buf into fd
			writesize = nbytes;
		else										// limited by bytes left in buf
			writesize = remain;
		
		if (remain > 0)
		{
			strncpy(cp, cq, writesize);		// write bytes into cp from buf at cq
			cp += writesize;								// Inc cp and cq by readsize
			cq += writesize;
			count += writesize;
			nbytes -= writesize;
			remain -= writesize;
			oftp->offset += writesize;
			
			if (oftp->offset > mip->INODE.i_size)
			{
				mip->INODE.i_size += writesize;
			}
		}
		
		/* UNOPTIMIZED: 
		while (remain > 0)
		{
			*cp++ = *cq++;							// write one byte
			nbytes--;										// Decrement counts
			remain--;
			oftp->offset++;							// Advance offset
			if(oftp->offset > mip->INODE.i_size)		// Increment file size if needed
				mip->INODE.i_size++;
			if (nbytes <= 0)
				break;
		}
		*/
		
		put_block(mip->dev, blk, writebuf);		// Write writebuf to disk
	}
	
	mip->dirty = 1;
	printf("wrote %d bytes to fd=%d\n", count, fd);
	return nbytes;
}

int cp_file(char *src, char *dest)
{
	int fd, gd, n;
	char buf[BLKSIZE];

	fd = open_file(src, 0);		// Open source file for read
	gd = open_file(dest, 1);		// Open destination file for write
	
	while (n = read_file(fd, buf, BLKSIZE))
	{
		printf("n=%d\n", n);
		write_file(gd, buf, n);
	}
	
	close_file(fd);
	close_file(gd);
}

int mv_file(char *src, char *dest)
{
	int sino, dino;
	MINODE *smip, *dmip;

	/* Verify that src exists, and get INODE */
	sino = getino(src);
	dino = getino(dest);
	
	if (sino == 0)	// src does not exist, abort
	{
		return 0;
	}
	if (dino == 0)	// dest does not exist, create it.
	{
		creat_file(dest);
		dino = getino(dest);
	}
	
	smip = iget(running->cwd->dev, sino);
	dmip = iget(running->cwd->dev, dino);
	
	/* Check if source dev is the same as destination dev */
	if (smip->dev == dmip-> dev)
	{
		/* hard link src with dest */
		link_file(src, dest);
		
		/* unlink src */
		unlink_file(src);
	}
	else
	{
		/* cp src to dest */
		cp_file(src, dest);
		
		/* unlink src */
		unlink_file(src);
	}
}
