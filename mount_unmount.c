/*********** mount_unmount.c file ****************/

/**** globals defined in main.c file ****/
extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC   proc[NPROC], *running;
extern MOUNT MountTable[NMOUNT];
extern char   gpath[256];
extern char   *name[64];
extern int    n;
extern int    fd, dev;
extern int    nblocks, ninodes, bmap, imap, inode_start;
extern char   line[256], cmd[32], pathname[256];

int mount_fs(char *filesys, char *mount_point)
{
	int ino, newdev, fd;
	MINODE *mip;
	MOUNT *mptr = 0;
	SUPER *sp;			// Super and Group Descriptor for reading new mount info
	GD *gp;
	int i;
	int mounted = 0;
	char buf[BLKSIZE];
	
	/* Display mounted table and check whether filesys is already mounted */
	for (i=0; i<NMOUNT; i++)
	{
		if (MountTable[i].dev != -1)
		{
			printf("%s mounted on %s\n", MountTable[i].name, MountTable[i].mount_name);
		}
		
		if (strcmp(MountTable[i].name, filesys)==0)
		{
			mounted = 1;
		}
	}
	
	if ((strcmp(filesys, "")==0) && (strcmp(mount_point, "")==0))
	{
		return 0;
	}
	else if (mounted == 1)
	{
		printf("MOUNT: file system already mounted\n");
		return 0;
	}
	
	/* Allcate a free MOUNT table entry for the system */
	for (i=0; i<NMOUNT; i++)
	{
		if (MountTable[i].dev == -1)		// dev = -1 means free
		{
			mptr = &(MountTable[i]);
		}
	}
	
	if (mptr == 0)
	{
		printf("MOUNT: no more mounts available\n");
		return 0;
	}
	
	/* Open filesys for read/write */
  if ((fd = open(filesys, O_RDWR)) < 0)		// have to use original open here as the file is outside of the current filesys
	{
    printf("MOUNT: open %s failed\n", filesys);  
		return 0;
  }

  newdev = fd;

  /* Check EXT2 File system */
  printf("Checking EXT2 FS...");
  
  get_block(newdev, 1, buf);			// Get super block of new disk
  sp = (SUPER *)buf;

  if (sp->s_magic != 0xEF53)
	{
      printf("MOUNT: magic = %x is not an ext2 filesystem\n", sp->s_magic);
      return 0;
  }     
  printf("OK\n");
  
  get_block(newdev, 2, buf); 		// Get group descriptor of new disk
  gp = (GD *)buf;
  
  /* Find mount_point */
  ino = getino(mount_point);
  mip = iget(dev, ino);
  
  /* check if mount_point is a dir */
	if ((mip->INODE.i_mode & 0xF000) != 0x4000)
	{
		printf("MOUNT: not a directory\n");
		iput(mip);
		return 0;
	}
	
	/* check mount_point is not busy */
	if (mip->refCount != 1)
	{
		printf("Busy: refcount = %d\n", mip->refCount);
		iput(mip);
		return 0;
	}
	
	/* Record new dev in MOUNT table */
	mptr->dev = newdev;
	mptr->ninodes = sp->s_inodes_count;
  mptr->nblocks = sp->s_blocks_count;
  mptr->bmap = gp->bg_block_bitmap;
  mptr->imap = gp->bg_inode_bitmap;
  mptr->iblk = gp->bg_inode_table;
  strcpy(mptr->name, filesys);
	
	/* mark mount_point's inode as being mounted and let it point to MOUNT table entry, which points back to mount_point minode */
	mip->mounted = 1;
	mip->mountptr = mptr;
	
	strcpy(mptr->mount_name, mount_point);
	mptr->mounted_inode = mip;
	
	return 1;
}

int umount_fs(char *filesys)
{
	int i;
	MOUNT *mptr = 0;
	MINODE *mip;
	
	/* Check that filesys is mounted */
	for (i=0; i<NMOUNT; i++)
	{
		if (strcmp(MountTable[i].name, filesys)==0)
		{
			mptr = &MountTable[i];
		}
	}
	
	if (mptr == 0)
	{
		printf("UMOUNT: filesys not mounted\n");
		return 0;
	}
	
	/* Check if any file is still active in the mounted filesys */
	for (i=0; i<NMINODE; i++)
	{
		if (minode[i].dev == mptr->dev)
		{
			printf("UMOUNT: busy\n");
			return 0;
		}
	}
	
	/* find and reset mount_point's inode */
	mip = mptr->mounted_inode;
	mip->mounted = 0;
	mip->mountptr = 0;
	
	iput(mip);
}
