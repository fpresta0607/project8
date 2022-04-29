
/**
 * @file create.c
 * @provides create, newpid, userret
 *
 * COSC 3250 Assignment 6
 */
/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

/**
 *COSC 3250 - Project 7
 *@author [Timi Ladeinde]
 *Instructor [Dennis Brylow]
 *TA-BOT:MAILTO [Olurotimi.Ladeinde@marquette.edu, francesco.presta@marquette.edu]
 */

#include <xinu.h>

static pid_typ newpid(void);
void userret(void);
void *getstk(ulong);

/**
 * Create a new process to start running a function.
 * @param funcaddr address of function that will begin in new process
 * @param ssize    stack size in bytes
 * @param name     name of the process, used for debugging
 * @param nargs    number of arguments that follow
 * @return the new process id
 */
syscall create(void *funcaddr, ulong ssize, uint tickets, char *name, ulong nargs, ...)
{
    ulong *saddr;               /* stack address                */
    ulong pid;                  /* stores new process id        */
    pcb *ppcb;                  /* pointer to proc control blk  */
    ulong i;
    va_list ap;                 /* points to list of var args   */
    ulong pads = 0;             /* padding entries in record.   */
    void INITRET(void);

		
    if (ssize < MINSTK)
        ssize = MINSTK;
    ssize = (ulong)(ssize + 3) & 0xFFFFFFFC; 
    
    //roufnd up to even boundary
  ////// kprintf("ssize = %d",ssize); 
//saddr = (ulong*)getmem(ssize) + ssize;
//kprintf("0x%08X = saddr",saddr);  
    pid = newpid();
//kprintf("1");
  

    numproc++;
    ppcb = &proctab[pid];
	
    // TODO: Setup PCB entry for new process.
    strncpy(ppcb->name, name, PNMLEN);
    ppcb->stkptr = NULL;
    ppcb->state = PRSUSP;
    ppcb->stkbase = (ulong*)getmem(ssize);
    ppcb->stklen = ssize;
    ppcb->tickets = tickets;
    saddr = ((ulong)ppcb->stkbase)+ssize-4;	
//kpriintf("1");
    if(((ulong *)SYSERR) == ppcb->stkbase)
    {
	return SYSERR;    
    }	
    /* Initialize stack with accounting block. */
    *saddr = STACKMAGIC;
    *--saddr = pid;
    *--saddr = ppcb->stklen;
    *--saddr = (ulong)ppcb->stkbase;

    /* Handle variable number of arguments passed to starting function   */
    if (nargs)
    {
        pads = ((nargs - 1) / 4) * 4;
    }
    /* If more than 4 args, pad record size to multiple of native memory */
    /*  transfer size.  Reserve space for extra args                     */
    for (i = 0; i < pads; i++)
    {
        *--saddr = 0;
    }

     for (i = 0; i < 16; i++)
    {
	*--saddr = 0;
    }

    // TODO: Initialize process context.

	//saddr[CTX_SP] = (int) saddr;
	saddr[CTX_LR] = (int) userret;
	saddr[CTX_SP] = (int) funcaddr;	
	saddr[CTX_IP] = (ARM_MODE_SYS | ARM_F_BIT);
	
	// TODO:  Place arguments into activation record.
	//        See K&R 7.3 for example using va_start, va_arg and
	//        va_end macros for variable argument functions.
//	ppcb->stkptr = saddr;
//kprintf("\n0x%08X = stkptr\n0x%08X = stkbase",ppcb->stkptr,ppcb->stkbase);  
	va_start(ap,nargs);

	for(i = 0; i < nargs; i++){
		if(i<4){
			//kprintf("less than 4--> 0x%08X: %d\n",saddr[i],*saddr);
			saddr[CTX_R0 + i] = va_arg(ap,ulong);
		}else{
			//kprintf("0x%08x: %d\n",saddr[i],*saddr);
			saddr[CTX_PC + (i - 3)] = va_arg(ap,ulong);
		}
	}
	va_end(ap);


	ppcb->stkptr = saddr;
	
//kprintf("\n0x%08X = stkptr\n0x%08X = stkbase",ppcb->stkptr,ppcb->stkbase);  
    return pid;
}

/**
 * Obtain a new (free) process id.
 * @return a free process id, SYSERR if all ids are used
 */
static pid_typ newpid(void)
{
    pid_typ pid;                /* process id to return     */
    static pid_typ nextpid = 0;

    for (pid = 0; pid < NPROC; pid++)
    {                           /* check all NPROC slots    */
        nextpid = (nextpid + 1) % NPROC;
        if (PRFREE == proctab[nextpid].state)
        {
            return nextpid;
        }
    }
    return SYSERR;
}

/**
 * Entered when a process exits by return.
 */
void userret(void)
{
    kill(currpid);
}
