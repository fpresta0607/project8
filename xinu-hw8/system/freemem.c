/**
 * @file freemem.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2020.  All rights reserved. */

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
syscall freemem(void *memptr, ulong nbytes)
{
    register struct memblock *block, *next, *prev;
    ulong top;

    /* make sure block is in heap */
    if ((0 == nbytes)
        || ((ulong)memptr < freelist.base)
        || ((ulong)memptr > freelist.base + freelist.bound))
    {
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
	int ps = disable();
		
		
	next = (struct memblock*)freelist.head;
	prev = (struct memblock*)&freelist;
	while(next!=NULL){
		if(block < next) {
			if(prev < block){
				top = prev->length + nbytes;//retrive top of previous memblock
				if(top != prev->length){
					if(top != next->length)	{
						prev->next = block;
						block->next = next;
					}
				}
			}
		}
		else if(block == next){
			if(prev < block){
				top = prev->length + nbytes;//retrive top of previous memblock
				if(top != prev->length){
					if(top != next->length){
						next->length = next->length + block->length;
					}
				}
			}
		}
		next = next->next;
		prev = next;
    }
	restore(ps);   

    return OK;
}
