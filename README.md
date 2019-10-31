# CptS 360: Systems Programming in UNIX / Linux

Project Specification (https://www.eecs.wsu.edu/~cs360/proj19.html):

A. OBJECTIVE:
   Design and implement a Linux-compatible EXT2 file system.

B. SPECIFICATIONS:
1. Files:
   Files are exactly the same as they are in the Linux file system, i.e.
   we shall use the same EXT2 file system data structures for

      SuperBlock, GroupDescriptor, Bitmaps, Inode, Directory

   EXCEPTIONS:
      Only DIR and REG file types; no SPECIAL files.
      File size : No triple-indirect blocks.
   
2. Disks:
    Disks are "virtual disks" simulated by Linux files.  
    Disk I/O are simulated by Linux read()/write() operations on a BLKSIZE 
    basis. You may use Linux's mke2fs to create virtual disks

3. File names:
   As in Unix, each file is identified by a pathname, e.g. /a/b/c or x/y/z.

   If a pathname begins with "/",  it's relative to the / directory.
   Otherwise, it's relative to the Current Working Directory (CWD) of the 
   running process (see Processes below). 

4. Processes:
   Every file operation is performed by a process.
   The simulator starts with TWO processes:
       A process P1 with uid=0 (for SUPERUSER), and
       A process P2 with uid=1 (for ordinary user).
   The CWD of both processes are initially at the root directory /
  
   P1 runs first. P2 is in a readyQueue, which contains processes that are 
   ready to run.

   All processes executes the same code (i.e. the FS project), in which it

       loop forever{
            prompt for a command;
            execute the command;
       }
 
   Each command is performed by the current running process. A switch command
   changes the running process (to another READY process).

5. File System Commands and Operations:
   File operations will be executed as commands. The required commands are 
   listed below. LEVEL 1 is the MINIMUM requirements for passing.
    
              -------  LEVEL 1 ------------ 
               mount_root;
               mkdir, rmdir, ls, cd, pwd;
               creat, link,  unlink, symlink
               stat,  chmod, touch;

              -------  LEVEl 2 -------------
               open,  close,  read,  write
               lseek  cat,    cp,    mv

              -------  LEVEl 3 ------------ 
               mount, umount
               File permission checking
              -----------------------------
      
   All commands work exactly the same way as they do in Unix/Linux. 
