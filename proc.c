/*********** proc.c file ****************/

/**** globals defined in main.c file ****/
extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC   proc[NPROC], *running, *readyQueue, *freeList;
extern char   gpath[256];
extern char   *name[64];
extern int    n;
extern int    fd, dev;
extern int    nblocks, ninodes, bmap, imap, inode_start;
extern char   line[256], cmd[32], pathname[256];
extern char		*status[3];

int enqueue(PROC **queue, PROC *p) 		// Enqueue a proc into the specified queue (ready or free)
{
  PROC *q = *queue;
  
  if (q == 0)		// Queue is empty
  {
  	*queue = p;
  	p->nextProc = q;
  }
  else					// Insert at end of queue
  {
  	while (q->nextProc)
    	q = q->nextProc;
            
    p->nextProc = q->nextProc;
    q->nextProc = p;
  }
}

PROC *dequeue(PROC **queue) 				// Dequeue a proc from the specified queue and return the proc
{
	PROC *p = *queue;
     
	if (p)
		*queue = (*queue)->nextProc;
        
	return p;
}

int fork_proc()
{
	PROC *p = 0;
	PROC *temp = 0;
	int i;
	
	/* find first free proc in proc[] */
	p = dequeue(&freeList);
	
  if (!p){
     printf("FORK: no more proc\n");
     return(-1);
  }
  
  /* Initialize the new proc */
	p->status = READY;
	p->ppid = running->pid;
	p->parentProc = running;
	p->cwd = running->cwd;
	p->uid = running->uid;
	
	/* Insert new proc as child of running proc */
	if (running->childProc == 0)
	{
		running->childProc = p;
	}
	else
	{
		temp = running->childProc;
		while (temp->siblingProc != 0)
		{
			temp = temp->siblingProc;
		}
		temp->siblingProc = p;
	}
	
	printf("P%d forked a child P%d\n", running->pid, p->pid);
	enqueue(&readyQueue, p);
	return p->pid;
}

int ps_procs()
{ 
  PROC *p;
	
	p = running;
	
	printf("P%d[%d]", p->pid, p->uid);
	while (p->nextProc)
	{
		p = p->nextProc;
		printf(" => P%d[%d]", p->pid, p->uid);
	}
	
	printf("\n");
}

int cs_proc()
{
	PROC *p;
	
	p = dequeue(&readyQueue);
	running = readyQueue;
	enqueue(&readyQueue, p);
}

int kill_proc()
{
	PROC *p;
	
	p = dequeue(&readyQueue);
	
	if (p->pid == 0)	// can't kill P0
	{
		printf("KILL: can't kill P0\n");
		enqueue(&readyQueue, p);
	}
	
	running = readyQueue;
	enqueue(&freeList, p);
}
