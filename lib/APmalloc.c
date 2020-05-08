/* 
 * Malloc.c - routines for managing memory for GCC and  SAS/C
 * 
 * These come from PDC release 3.3 (C) 1989 Lionel Hummel.
 *
 * I use it to avoid the usage of Allocate()/Deallocate()
 * that puts APurify in confusion. I also modified it a bit
 * to use ADD2EXIT().
 *
 * Samuel DEVULDER, 1995
 *
 * calloc     allocates a cleared array of blocks
 * free       frees a previously allocated block of memory
 * malloc     allocates a block of memory
 * realloc    alter buffer size; same contents, new ptr 
 *
 * I've used things found in the libnix1.0 source package (ADD2EXIT)
 * but I must say that I've done it without knowing it... I hope
 * it work well anyway.
 */

#include <exec/types.h>
#include <exec/memory.h>

#if defined(__GNUC__)
#include "stabs.h"   /* from libnix source package */
#if __GNUC_MINOR__!=6
#include <proto/exec.h>
#else
extern struct ExecBase *SysBase;
extern void *AllocMem(ULONG,ULONG);
#endif
#elif defined(__SASC)
#define __USE_SYSBASE
#include <proto/exec.h>
#else  /* dcc */
extern struct ExecBase *SysBase;
extern void *AllocMem(ULONG,ULONG);
#endif

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct memchunk 
        {
        struct memchunk    *next;
        struct memchunk    *prev;
        long                size;
        } MEMCHUNK;


#define ZERO_REQUEST 0L

/*
 * sentinel: always last and first  elem. in list.
 *
 * sentinel.prev == real last elem in list
 * sentinel.next == real first elem in list
 */
static MEMCHUNK sentinel = {&sentinel, &sentinel, 0};
static int locked;
extern ULONG APflg;
#define APlock 0x2

/*
 * Free any allocated memory on exit()...
 */
#ifdef __PDC__
extern void  (*_freeall)();
typedef unsigned long size_t;
#endif

#ifdef __SASC
__stdargs               /* on en a besoin car sinon slink voit 2 _STD */
                        /* et appel 2 fois cette fonction -> deadlock */
#endif
void _STDexitmalloc(void)
        {
        MEMCHUNK    *mp, *mp1;
        
        for ( mp = sentinel.prev; mp != &sentinel; ) 
                {
                mp1 = mp->prev;
                FreeMem( (APTR) mp, (ULONG) mp->size );
                mp = mp1;
                }
        }

long addsyschunk(long ,long, char *);
void process_malloc(void)
        {
        MEMCHUNK *mp;
        locked = 1;
        for (mp = sentinel.prev; mp != &sentinel; mp = mp->prev)
                {
/*              printf("%08x(%d)\n",mp,mp->size);
*/              addsyschunk((long)mp,((long)mp)+mp->size-1,"allocation during startup");
                }
        locked = 0;
        }

void *malloc(size_t size)
        {
        MEMCHUNK *mp;
        ULONG chunksize;

        if (size == 0) return ZERO_REQUEST;

        chunksize = size + sizeof(MEMCHUNK);
#ifndef __PDC__
        mp = AllocMem( chunksize, (ULONG) MEMF_ANY);
#else
        mp = AllocMem( chunksize, (ULONG) MEMF_CLEAR);
#endif
        if ( mp == NULL ) {errno=ENOMEM; return NULL;}
#ifdef  __PDC__
        _freeall = _STDexitmalloc;
#endif
        if(!locked && !(APflg&APlock))
                {
                /* Keep the forward and backward links.
                 */
                sentinel.prev->next = mp;
                mp->prev = sentinel.prev;
                sentinel.prev = mp;
                mp->next = &sentinel;
                }
        else    mp->prev = mp->next = NULL;

        mp->size = chunksize;
        
        return ++mp;
        }

void free(void *p)
        {
        MEMCHUNK *mp, *prevmp, *nextmp;
        
        if(!p) return;

        mp = (MEMCHUNK*)((char*)p - sizeof(MEMCHUNK));
        
        /* Sanity check: the prev link should point to us. Do nothing if bad.
         */
        prevmp = mp->prev;
        if(prevmp)
                {
                nextmp = mp->next;
                if (prevmp->next != mp )
                        {
                        fprintf(stderr,"free(3): bad pointer 0x%08x "
                                       "(0x%08x already freed?)\n",p,mp);
                        return;
                        }
                /*
                 * unlink that memchunk
                 */
                prevmp->next = nextmp;
                nextmp->prev = prevmp;
                }

        FreeMem( (APTR) mp, (ULONG) mp->size );

        return;
        }

void *calloc(size_t nelem, size_t elsize)
        {
        register char *newmem;
        register size_t   totsize;
        
        totsize = nelem * elsize;
        if(newmem = malloc(totsize)) bzero(newmem, totsize);
        
        return(newmem);
        }

void *realloc(void *ptr,size_t size)
        {
        MEMCHUNK *mp, *prevmp;
        char *newmem;

        if( ptr == NULL) return malloc(size);
        if( size == 0) 
                {
                free(ptr);
                return NULL;
                }

        mp = (MEMCHUNK *) ((char*)ptr - sizeof(MEMCHUNK));

        /* Sanity check: the prev link should point to us. Do nothing if bad.
         */
        prevmp = mp->prev;
        if ( prevmp && prevmp->next != mp )
                {
                fprintf(stderr,"realloc(3): bad pointer 0x%08x "
                               "(0x%08x already freed?)\n",ptr,mp);
                return (0);
                }

        if(newmem = malloc(size))
                { /* sam */
                int oldsize = mp->size - sizeof(MEMCHUNK);
                size = ( (size > oldsize) ? oldsize : size);

                bcopy(ptr, newmem, size);
                free(ptr);
                }

        return(newmem);
        }

#ifdef __GNUC__
ADD2EXIT(_STDexitmalloc,-50);
#endif
