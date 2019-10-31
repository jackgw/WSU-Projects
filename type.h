/*************** type.h file ************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <libgen.h>
#include <string.h>
#include <sys/stat.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

SUPER *sp;
GD    *gp;
INODE *ip;
DIR   *dp;   

#define BLKSIZE           1024
#define ISIZE              128

#define BITS_PER_BLOCK    (8*BLOCK_SIZE)
#define INODES_PER_BLOCK  (BLOCK_SIZE/sizeof(INODE))

// Default dir and regulsr file modes
#define DIR_MODE          0040777 
#define FILE_MODE         0100644
#define SUPER_MAGIC       0xEF53
#define SUPER_USER        0

// Proc status
#define FREE              0
#define BUSY              1
#define READY             2

// Table sizes
#define NMINODE          64
#define NMOUNT            4
#define NPROC             8
#define NFD              16
#define NOFT             32

typedef struct minode{
  INODE INODE;
  int dev, ino;
  int refCount;
  int dirty;
  // for level-3
  int mounted;
  struct Mount *mountptr;
}MINODE;

typedef struct oft{
  int  mode;
  int  refCount;
  MINODE *mptr;
  int  offset;
}OFT;

typedef struct proc{
  struct proc *next;
  int          pid;
  int          uid;
  int          gid;
  int          ppid;
  int          status;
  MINODE      *cwd;
  OFT         *fd[NFD];
  
  struct Proc *nextProc;
  struct Proc *parentProc;
  struct Proc *childProc;
  struct Proc *siblingProc;
}PROC;


// Mount Table structure
typedef struct Mount{
        int    dev;   
        int    ninodes;
        int    nblocks;
        int    imap, bmap, iblk; 
        struct minode *mounted_inode;
        char   name[256]; 
        char   mount_name[64];
} MOUNT;
