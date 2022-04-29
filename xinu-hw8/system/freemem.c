/**
 * @file freemem.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2020.  All rights reserved. */

#define SCARG(type, args)  (type)(*args++)
#include <xinu.h>

/**
 * Frees a block of heap-allocated memory.
 *
 * @param memptr
 *      Pointer to memory block allocated with memget().
 *
 * @param nbytes
 *      Length of memory block, in bytes.  (Same value passed to memget().)
 *
 * @return
 *      ::OK on success; ::SYSERR on failure.  This function can only fail
 *      because of memory corruption or specifying an invalid memory block.
 */
syscall sc_freemem(int *args)
{
    register struct memblock *block, *next, *prev;
    ulong top;
    int ps =0;
    ps = disable();
 
    void *memptr = SCARG(void*, args);
    ulong nbytes = SCARG(ulong, args);    

/* make sure block is in heap */
    if ((0 == nbytes)
        || ((ulong)memptr < freelist.base)
        || ((ulong)memptr > freelist.base + freelist.bound))
    {
	restore(ps);
        return SYSERR;
    }

    block = (struct memblock *)memptr;
    nbytes = (ulong)roundmb(nbytes);

    /* TODO:
     *      - Disable interrupts
     *      - Find where the memory block should
     *        go back onto the freelist (based on address)
     *      - Find top of previous memblock
     *      - Make sure block is not overlapping on prev or next blocks
     *      - Coalesce with previous block if adjacent
     *      - Coalesce with next block if adjacent
     *      - Restore interrupts
     */
    block->length = nbytes;
    top = ((ulong)block) + block->length;
    
    memblk *temp;
    temp = freelist.head;
    
    if((top - 1) > freelist.bound)
    {	
	restore(ps);
	return SYSERR;
    }

    if(temp == NULL)
    {
	freelist.head = block;
	freelist.head->length = nbytes;
	freelist.head->next = NULL;
	freelist.size += nbytes;
	restore(ps);
	return OK;
    }

 while(temp != NULL)
    {
	ulong tTop;
	tTop = ((ulong)temp) + temp->length - 1;
	if(((((ulong)block) <= ((ulong)temp)) && (((ulong)temp) < top)) || ((((ulong)block) <= tTop) && (tTop < top)))
	{
	    restore(ps);
	    return SYSERR;
	}
	else temp = temp->next;
    }

    prev = freelist.head;  

    while((prev->next != NULL) && ((ulong)prev->next < (ulong)block))
    {
	prev = prev->next;
    }

    next = prev->next;
    

    if((ulong)block < (ulong)prev)
    {
	if(top == (ulong)prev)
	{
	    block->length += prev->length;
	    block->next = prev->next;
	    freelist.head = block;
	}	
	else
	{
	    block->next = freelist.head;
	    freelist.head = block;	    
	}
    }
    else if(next == NULL)
    {  
	if((((ulong)prev) + prev->length) == (ulong)block)
	{
	    prev->length += nbytes;
	}
	else
	{
	    block->next = prev->next;
	    prev->next = block;
	}
	
    }
    else 
    {	
	ulong prevTop = ((ulong)prev) + prev->length;
	
	if((prevTop == (ulong)block) && ((((ulong)block) + block->length) == (ulong)next))
	{
	    prev->next = next->next;
	    prev->length += nbytes;
	    prev->length += next->length;
	}
	else if(prevTop == (ulong)block)
	{
	    prev->length += nbytes;
	}
	else if((((ulong)block) + block->length) == (ulong)next)
	{
	    block->next = next->next;
	    block->length += next->length;
	    prev->next = block;
	}
	else
	{
	    prev->next = block;
	    block->next = next;
	}	
    }

    freelist.size += nbytes;

    restore(ps);    

    return OK;
}
