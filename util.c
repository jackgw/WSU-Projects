/*********** util.c file ****************/

/**** globals defined in main.c file ****/
extern MINODE minode[NMINODE];
extern MINODE *root;
extern MOUNT MountTable[NMOUNT];
extern PROC   proc[NPROC], *running;
extern char   gpath[256];
extern char   *name[64];
extern int    n;
extern int    fd, dev;
extern int    nblocks, ninodes, bmap, imap, inode_start;
extern char   line[256], cmd[32], pathname[256];

int get_block(int dev, int blk, char *buf)
{
	lseek(dev, (long)blk*BLKSIZE, 0);
	read(dev, buf, BLKSIZE);
}   

int put_block(int dev, int blk, char *buf)
{
	lseek(dev, (long)blk*BLKSIZE, 0);
	write(dev, buf, BLKSIZE);
}   

int tst_bit(char *buf, int bit)
{
	int i, j;
	i = bit/8; j=bit%8;
	if (buf[i] & (1 << j))
  	return 1;
	return 0;
}

int set_bit(char *buf, int bit)
{
	int i, j;
	i = bit/8; j=bit%8;
	buf[i] |= (1 << j);
}

int clr_bit(char *buf, int bit)
{
	int i, j;
	i = bit/8; j=bit%8;
	buf[i] &= ~(1 << j);
}

int ialloc(int dev)
{
  int  i;
  char buf[BLKSIZE];

  // read inode_bitmap block
  get_block(dev, imap, buf);

  for (i=1; i < ninodes; i++){
    if (tst_bit(buf, i)==0){
    	set_bit(buf,i);
    	put_block(dev, imap, buf);
    	return i+1;
    }
  }

  return 0;
}

int balloc(int dev)
{
  int  i;
  char buf[BLKSIZE];

  // read inode_bitmap block
  get_block(dev, bmap, buf);

  for (i=1; i < nblocks; i++){
    if (tst_bit(buf, i)==0){
    	set_bit(buf,i);
    	put_block(dev, bmap, buf);
    	return i+1;
    }
  }

  return 0;
}

int idealloc(int dev, int blk)
{
	int  i;
  char buf[BLKSIZE];

	if (blk > ninodes)
	{
		printf("blk out of range\n");
		return 0;
	}
	
  // read inode_bitmap block
  get_block(dev, imap, buf);
  clr_bit(buf, blk-1);
	
	// write block back
	put_block(dev, imap, buf);

  return 1;
}

int bdealloc(int dev, int blk)
{
	int  i;
  char buf[BLKSIZE];

	if (blk > nblocks)
	{
		printf("blk out of range\n");
		return 0;
	}
	
  // read inode_bitmap block
  get_block(dev, bmap, buf);
  clr_bit(buf, blk-1);
	
	// write block back
	put_block(dev, bmap, buf);

  return 1;
}

int tokenize(char *pathname)
{
	char *s;
	int i = 0;
	
	char *tempstr = (char *)malloc(sizeof(char)*strlen(pathname));
	
	strcpy(tempstr, pathname);

	s = strtok(tempstr, "/");
	
	while (s){
		name[i] = s;
		s = strtok(0, "/");
		i++;
	}
	
	return i;
}

// return minode pointer to loaded INODE
MINODE *iget(int dev, int ino)
{
  int i;
  MINODE *mip;
  char buf[BLKSIZE];
  int blk, disp;
  INODE *ip;

  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    if (mip->dev == dev && mip->ino == ino){
      mip->refCount++;
      //printf("found [%d %d] as minode[%d] in core\n", dev, ino, i);
      return mip;
    }
  }
    
  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    if (mip->refCount == 0){
      //printf("allocating NEW minode[%d] for [%d %d]\n", i, dev, ino);
      mip->refCount = 1;
      mip->dev = dev;
      mip->ino = ino;

      // get INODE of ino to buf    
      blk  = (ino-1) / 8 + inode_start;
      disp = (ino-1) % 8;

      //printf("iget: ino=%d blk=%d disp=%d\n", ino, blk, disp);

      get_block(dev, blk, buf);
      ip = (INODE *)buf + disp;
      // copy INODE to mp->INODE
      mip->INODE = *ip;

      return mip;
    }
  }   
  printf("PANIC: no more free minodes\n");
  return 0;
}

iput(MINODE *mip)
{
  int i, block, offset;
  char buf[BLKSIZE];
  INODE *ip;

  if (mip==0) 
    return;

  mip->refCount--;
 
  if (mip->refCount > 0) return;
  if (!mip->dirty)       return;
 
  /* write back */
  //printf("iput: dev=%d ino=%d\n", mip->dev, mip->ino); 

  block =  ((mip->ino - 1) / 8) + inode_start;
  offset =  (mip->ino - 1) % 8;

  /* first get the block containing this inode */
  get_block(mip->dev, block, buf);

  ip = (INODE *)buf + offset;
  *ip = mip->INODE;

  put_block(mip->dev, block, buf);

} 

int search(MINODE *mip, char *dirname)
{
  char sbuf[BLKSIZE], temp[256];
	char *cp;
	DIR *dp;
	int i;

	for (i=0; i < 12; i++)	 // assume DIR at most 12 direct blocks
	{
		if (mip->INODE.i_block[i] == 0)
		{
			return 0;
		}

		get_block(mip->dev, mip->INODE.i_block[i], sbuf);

		dp = (DIR *)sbuf;
		cp = sbuf;

		while(cp < sbuf + BLKSIZE)
		{
			strncpy(temp, dp->name, dp->name_len);
			temp[dp->name_len] = 0;
			
			if (strcmp(temp, dirname) == 0)
			{
				//printf("%s found. inode = %d\n", dirname, dp->inode);
				return dp->inode;
			}	

			cp += dp->rec_len;
			dp = (DIR *)cp;
		}
	}
	printf("%s not found.\n", dirname);
	return 0;
}

int getino(char *pathname)
{
  int i, j, ino, blk, disp;
  INODE *ip;
  MINODE *mip;

  if (strcmp(pathname, "/")==0)
      return 2;

	if (strcmp(pathname, ".")==0)
			return running->cwd->ino;

  if (pathname[0]=='/')
  {
    mip = iget(root->dev, 2);
  }
  else
  {
    mip = iget(running->cwd->dev, running->cwd->ino);
  }

  n = tokenize(pathname);

  for (i=0; i<n; i++)
	{
    ino = search(mip, name[i]);
		
		if (ino==0)
		{
      iput(mip);
      printf("name %s does not exist\n", name[i]);
      return 0;
    }

    iput(mip);
    mip = iget(dev, ino);
    
    /* Handle upwards mount traversal */
    if (mip->ino == 2 && mip->dev != root->dev)
		{
			printf("upwards mount point hit\n");
			
			/* locate Mount table from dev number */
			for (j=0; j<NMOUNT; j++)
			{
				if (MountTable[j].dev == mip->dev)
				{
						iput(mip);
						mip = MountTable[j].mounted_inode;
				}
			}
			
			ino = mip->ino;
			dev = mip->dev;
		}
    
    /* Handle downwards mount traversal */
		else if (mip->mounted == 1)	
		{
			printf("downwards mount point hit\n");
			iput(mip);
			dev = mip->mountptr->dev;
			ino = 2;
			mip = iget(dev, 2);
		}
  }

  iput(mip);
  return ino;
}

