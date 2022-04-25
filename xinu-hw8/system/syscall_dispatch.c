/**
 * @file syscall_dispatch.c
 * Translate user_* functions to system calls, dispatch to proper sc_*
 * functions, and complete.
 */
/* Embedded Xinu, Copyright (C) 2009, 2020.  All rights reserved. */

#include <xinu.h>

/* for syscall argument parsing */
#define SCARG(type, args)  (type)(*args++)

/* macro to make user->kernel syscalls */
#define SYSCALL(num) int status; \
	asm("swi %0" : : "i"(SYSCALL_##num)); \
	asm("mov %0, r0" : "=r"(status)); \
	return status;


/* syscall wrapper prototypes */
syscall sc_none(int *);
syscall sc_yield(int *);
syscall sc_getc(int *);
syscall sc_putc(int *);
syscall sc_getmem(int *);
syscall sc_freemem(int *);
syscall sc_ptcreate(int *);
syscall sc_join(int *);
syscall sc_lock(int *);
syscall sc_unlock(int *);

/* table for determining how to call syscalls */
const struct syscall_info syscall_table[] = {
    { 5, (void *)sc_none },     /* SYSCALL_NONE      = 0  */
    { 0, (void *)sc_yield },    /* SYSCALL_YIELD     = 1  */
    { 1, (void *)sc_none },     /* SYSCALL_SLEEP     = 2  */
    { 1, (void *)sc_none },     /* SYSCALL_KILL      = 3  */
    { 2, (void *)sc_none },     /* SYSCALL_OPEN      = 4  */
    { 1, (void *)sc_none },     /* SYSCALL_CLOSE     = 5  */
    { 3, (void *)sc_none },     /* SYSCALL_READ      = 6  */
    { 3, (void *)sc_none },     /* SYSCALL_WRITE     = 7  */
    { 1, (void *)sc_getc },     /* SYSCALL_GETC      = 8  */
    { 2, (void *)sc_putc },     /* SYSCALL_PUTC      = 9  */
    { 2, (void *)sc_none },     /* SYSCALL_SEEK      = 10 */
    { 4, (void *)sc_none },     /* SYSCALL_CONTROL   = 11 */
    { 1, (void *)sc_none },     /* SYSCALL_GETDEV    = 12 */
    { 4, (void *)sc_ptcreate},	/* SYSCALL_PTCREATE  = 13 */
    { 2, (void *)sc_join},	/* SYSCALL_PTJOIN    = 14 */
    { 1, (void *)sc_lock},	/* SYSCALL_PTLOCK    = 15 */
    { 1, (void *)sc_unlock},  /* SYSCALL_PTUNLOCK  = 16 */	
    { 1, (void *)sc_getmem},	/* SYSCALL_GETMEM    = 17 */
    { 2, (void *)sc_freemem},	/* SYSCALL_FREEMEM   = 18 */
	
};

int nsyscall = sizeof(syscall_table) / sizeof(struct syscall_info);

/**
 * Syscall dispatch routine.  Given a syscall code and pointer to
 * arguments, change execution to function.  Otherwise, generate error
 * saying no such syscall.
 * @param code syscall code to execute
 * @param args pointer to arguments for syscall
 */
int syscall_dispatch(int code, int *args)
{
    if (0 <= code && code < nsyscall)
    {
        return (*syscall_table[code].handler) (args);
    }
    kprintf("ERROR: unknown syscall %d!\r\n", code);
    return SYSERR;
}

/**
 * syscall wrapper for *no* function, just a test/demo.
 * @param args expands to: none.
 */
syscall sc_none(int *args)
{
    return OK;
}

syscall user_none(void)
{
    SYSCALL(NONE);
}

/**
 * syscall wrapper for yield().
 * @param args expands to: none.
 */
syscall sc_yield(int *args)
{
    /* this may change thread of execution, allow exceptions */
    // exlreset();

    return resched();
}

syscall user_yield(void)
{
    SYSCALL(YIELD);
}

/**
 * syscall wrapper for getc().
 * @param args expands to: int descrp
 */
syscall sc_getc(int *args)
{
    int descrp = SCARG(int, args);
    if (0 == descrp)
    {
        return kgetc();
    }
    return SYSERR;
}

syscall user_getc(int descrp)
{
    SYSCALL(GETC);
}

/**
 * @param args expands to: int descrp, char character
 */
syscall sc_putc(int *args)
{
    int descrp = SCARG(int, args);
    char character = SCARG(char, args);

    if (0 == descrp)
        return kputc(character);
    return SYSERR;
}
 user_putc(int descrp, char character)
{
    SYSCALL(PUTC);
}


syscall sc_getmem(int *args)
{
    ulong nbytes = SCARG(ulong, args);  
    return (syscall)getmem_real(nbytes);
}

void * getmem(ulong nbytes)
{
    SYSCALL(GETMEM); 
}

/* syscall wrapper for freemem().
* @param args expands to: int descrp, void *memptr, ulong nbytes 
**/ 
 syscall sc_freemem(int *args)
 {	
	 void *memptr = SCARG(ulong, args);
         ulong nbytes = SCARG(ulong, args);

	 return freemem_real(memptr, nbytes);
 }
syscall freemem(void *memptr, ulong nbytes)
{
        SYSCALL(FREEMEM);
} 
                   