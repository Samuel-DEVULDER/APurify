#undef __GNUC__

#ifdef __unix__
#include "../lib/APfuncs.c"
#else
#include "lib/APfuncs.c"
#endif

struct ExecBase execbase, *SysBase = &execbase;

void *FindTask(char *name) 
	{
	static simpletask st;
	static long seglistarray[8];
	static long _seglist[8];
	long *seglist=&_seglist[1];
	int pile;
	st.BasPileTache=((ULONG)&pile)-40000;
	st.HautPileTache=((ULONG)&pile)+1000;
	seglist[-1]=80;
	seglist[0]=0;
	seglistarray[3]=((ULONG)(seglist))>>2;
	st.SegList=((ULONG)(seglistarray))>>2;
	st.CLI=0;
	AP_pc = (ULONG)&seglist[1];
	return &st;
	}

geta4() {}

CacheClearU() {}

Permit() {}
void *AsmAllocMem() {}
ULONG SetFunction() {}
void StdFree(ptr,len) 
	{
	free((void*)ptr);
	}
Forbid() {}
void *StdAlloc(len,type) 
	{
	return malloc(len);
	}
void AsmFreeMem() {}

/*****************************************************************************
 * realmain 
 *****************************************************************************/
#include <stdio.h>

#define malloc(x) zz_malloc(x)
#define free(x)   zz_free(x)

void *zz_malloc(len)
	{
	int *pt;
	ifn(pt=(void*)PubAllocMem(len+4,0)) return pt;
	*pt=len+4;
	return ++pt;
	}

void zz_free(ptr)
	void *ptr;
	{
	int *p=ptr-4;
	PubFreeMem(p,*p);
	}

#define pr(x) {AP_procname=#x;}
#define cl(x) {AP_protect();x;AP_unprotect();}

int statica;

static test_debordement(a)
	int *a;
	{
	int i;
	pr(test_debordement);
	++(short*)a;
	AP_rd(a,4);
	/*	i=a[0];*/
	}

static test(p)
	int *p;
	{
	int z;
	pr(test);
	AP_rd(p,4);
	z = *p;
	}

static freeall(a)
	int *a;
	{
	pr(freeall);
	cl(free(a));
	}

static proc_fin()
	{
	register long *b;
	pr(proc_fin);
	b = (long*)4;
	AP_rd(b,4);
	/*	b = (long*)*b;*/
	/*	*b = *(long*)4;*/
	cl(AP_Close());
	}

main()
	{
	register int *a,*b;
	int c;
	pr(main);
	/*	extern int *malloc(); /**/
	/*	getchar(); */
	cl(AP_Init());
	/*	cl(AP_Verbose(1));/**/
	cl(a=malloc(4));
	cl(malloc(400));
	cl(malloc(12000));
	if(a)
		{
		cl(b=malloc(15))
		if(b) 
			{
			AP_rd(&b[-10],4);
			AP_wr(&a[0],4);
			AP_rd(&b[0],4);
			a[0]=b[0];c=b[-10];cl(free(b));
			}
		AP_rd(&a[1],4);
		if(a[1] == 0) c=32;
		cl(test(&c));b=&c;
		AP_rd(b,4);
		*b=0;
		b = &statica;
		AP_rd(b,4);
		*b = 15;
		cl(test_debordement(a));
		cl(freeall(a));
		cl(proc_fin());
		cl(test_debordement(a));
		}
	return 0;
	}
