/**
 * @file getmem.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2020.  All rights reserved. */

#include <xinu.h>

/**
 * Allocate heap memory.
 *
 * @param nbytes
 *      Number of bytes requested.
 *
 * @return
 *      ::SYSERR if @p nbytes was 0 or there is no memory to satisfy the
 *      request; otherwise returns a pointer to the allocated memory region.
 *      The returned pointer is guaranteed to be 8-byte aligned.  Free the block
 *      with memfree() when done with it.
 */
void *getmem(ulong nbytes)
{
    register memblk *prev, *curr, *leftover;

    if (0 == nbytes)
    {
        return (void *)SYSERR;
    }

    /* round to multiple of memblock size   */
    nbytes = (ulong)roundmb(nbytes);

    /* TODO:
     *      - Disable interrupts
     *      - Traverse through the freelist
     *        to find a block that's suitable 
     *        (Use First Fit with remainder splitting)
     *      - Restore interrupts
     *      - return memory address if successful
     */
	int ps = disable();
		

	
	curr = (struct memblock *)(freelist.head);
	prev = (struct memblock *) &freelist;

	memhead *freehead = &freelist;
	prev = NULL;
//	kprintf("start");
	while(curr != NULL)
	{


		if((curr->length) > nbytes)
		{
			leftover = (struct memblock *)(nbytes + ((ulong)curr));
			leftover->length = (curr->length) - nbytes;
		
			freehead->size = freehead->size - nbytes;

			leftover->next = curr->next;
		if(prev!=NULL)
		{
			prev->next = leftover;
			
		}else{
			freehead->head = leftover;
		}
		
			curr->length = nbytes;
			restore(ps);
//			kprintf("return");
			return(curr);
		
		}
		

		prev = curr;
		curr = curr->next;
	
	
	}
//	kprintf("DONE");
	restore(ps);	
return (void *)SYSERR;
}