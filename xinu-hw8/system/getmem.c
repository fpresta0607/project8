/**
 * @file getmem.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2020.  All rights reserved. */

#define SCARG(type, args)  (type)(*args++)

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
void * sc_getmem(int *args)
{
    ulong nbytes = SCARG(ulong, args);
    register memblk *prev, *curr, *leftover;
	int ps = 0;
	ps = disable();
   
     if (0 == nbytes)
    {
	restore(ps);
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
	
 curr = freelist.head;
    curr->length = freelist.head->length;
    curr->next = freelist.head->next;

    while(curr != NULL) 
    {
	if(curr->length < nbytes) 
	{       
		prev = curr;
		prev->length = curr->length;
		prev->next = curr->next;
		curr = curr->next;
		curr->length = prev->next->length;
		curr->next = prev->next->next;
	}
	else break;
    }

    if(curr != NULL)
    {	
	   
	memblk *start = (memblk *)(((ulong)curr) + nbytes);	
	start->length = curr->length - nbytes;
	freelist.size -= nbytes;

	if(curr->length == nbytes)
	{
	    if((curr == freelist.head) && (curr->next == NULL)) 
	    {
		freelist.head->length -= nbytes;
		freelist.head = freelist.head->next;
	    }
	    else if(curr == freelist.head)
	    {
		freelist.head->next = curr->next->next;
		freelist.head->length = curr->next->length;
		freelist.head = curr->next;
	    }
	    else if(curr->next == NULL) prev->next = NULL;
	    else prev->next = curr->next;

	    restore(ps);
	    return (void *)curr;
	}
	else
	{
	    if(curr == freelist.head) 
	    {
		start->next = freelist.head->next;
		freelist.head = start;
		
		restore(ps);
		return (void *)curr;
	    }
	    else
	    {
		start->next = curr->next;	
		leftover = start;
		prev->next = leftover;
		
		restore(ps);
		return (void *)curr;
	    }
	}	
    }

    restore(ps);   
    return (void *)SYSERR;
}
