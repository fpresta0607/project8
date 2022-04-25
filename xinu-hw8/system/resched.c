/**
 * @file resched.c
 * @provides resched
 *
 * COSC 3250 Assignment 4
 */
/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

#include <xinu.h>

extern void ctxsw(void *, void *);
/**
 * Reschedule processor to next ready process.
 * Upon entry, currpid gives current process id.  Proctab[currpid].pstate 
 * gives correct NEXT state for current process if other than PRREADY.
 * @return OK when the process is context switched back
 */


unsigned int totalTickets(void){
	unsigned int total = 0;
	int i;
	//find total tickets for all ready and current processes
	for(i=0;i<NPROC;i++){
		if((&proctab[i])->state == PRREADY){
			total += (&proctab[i])->tickets;
		}
	}
	return total;
}

int chooseWinner(unsigned int total){
	unsigned int winner;
	int count = 0,i;
	
	winner = random(total);
	
	if(winner == 0){
		winner = winner + 1;
	}
	for(i=0;i<NPROC;i++){
		if((&proctab[i])->state == PRREADY){
			count += (&proctab[i])->tickets;
			
			if(count >= winner){
				return i;
			}
		}
	}
	return i;
}

syscall resched(void) {
    pcb *oldproc;               /* pointer to old process entry */
    pcb *newproc;               /* pointer to new process entry */
    uint total = 0;
    int ps;//process state
    ps = disable();
    oldproc = &proctab[currpid];

    /* place current process at end of ready queue */
    if (PRCURR == oldproc->state)
    {
        oldproc->state = PRREADY;
        enqueue(currpid, readylist);
    }

    /**
     * We recommend you use a helper function for the following:
     * TODO: Get the total number of tickets from all processes that are in current and ready states.
     * Utilize the random() function to pick a random ticket. 
     * Traverse through the process table to identify which proccess has the random ticket value.
     * Remove process from queue.
     * Set currpid to the new process.
     */

    total = totalTickets(); /*Finds the total number of tickets*/
    currpid = chooseWinner(total); /*Sets the current pid to the process that contains the winning ticket*/
    remove(currpid); /*Removes the new currpid from the ready list becuase it is going to be  the current running process*/      
 
    newproc = &proctab[currpid];
    newproc->state = PRCURR;    /* mark it currently running*/

#if PREEMPT
    preempt = QUANTUM;
#endif
    //kprintf("[%d %d]", oldproc - proctab, newproc - proctab);
    //kprintf("[%d, %d]\r\n", oldproc - proctab, newproc - proctab);
    ctxsw(&oldproc->stkptr, &newproc->stkptr);

    restore(ps);
    /* The OLD process returns here when resumed. */
    return OK;  
}
