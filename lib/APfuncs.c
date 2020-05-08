/*
 *      APfuncs.c
 *
 * (c) by Samuel DEVULDER, 1996
 */

#ifdef __GNUC__
#ifdef amiga
#if __GNUC_MINOR__!=6
#include <proto/exec.h>
#include <proto/dos.h>
#else
extern struct ExecBase *SysBase;
#endif
#endif
#else
#ifdef __SASC
#define __USE_SYSBASE
#include <proto/exec.h>
#include <proto/dos.h>
#else
extern void *FindTask();
extern struct ExecBase *SysBase;
#endif
#endif

#include <stdio.h>
#ifdef __SASC
#include <stdarg.h>
#include <strings.h>
#else
#include <varargs.h>
#endif
#include <stdlib.h>
#ifndef __SASC
#include <sys/time.h>
#endif
#include <time.h>
#include <ctype.h>

#include <exec/execbase.h>

/****************************************************************************
 * Local defines
 ****************************************************************************/
/*
#define DEBUG 1
*/
 
#define STDERR          stdout      /* gcc seems to have trouble */
				    /* with the real stderr      */

#define STDLOG          (STDLOG_?STDLOG_:STDERR)
#define TABSIZE         4           /* our tabsize is 4 char. wide */
#define WARNING         "\n**** APURIFY WARNING !"
#define ERROR           "\n**** APURIFY ERROR !"

#define APLOG           "APlog"     /* env: variables */
#define APEXEC          "APexec"
#define APDOLL          "APdoll"
#define APSCR           "APscreen"

#define LVOAllocMem     (-0xC6)     /* for AP_Init() & AP_Close() */
#define LVOFreeMem      (-0xD2)

#define APopen          1           /* Some internal flags: lib opened ? */
#define APlock          2           /* lib locked ? */
#define APwarn          4           /* warning already done ? */
#define APreport        8           /* report enabled ? */
#define APreq           16          /* requester enabled ? */
#define APverbose       32          /* long verbose messages */

#define attr_rd         1           /* chunk attributes */
#define attr_wr         2
#define attr_sys        4
#define attr_fre        8
#define attr_stk        16

/****************************************************************************
 * General pupouse defines
 ****************************************************************************/

#ifdef  DEBUG
#define PRIVATE 
#else
#define PRIVATE        static
#endif
#define ifn(x)         if(!(x))                /* usefull define */
#define SCHK(s)        ((s)?(s):"NULL")

/****************************************************************************
 * Local types
 ****************************************************************************/

#if !defined(__GNUC__) && !defined(__unix__) && !defined(__SASC)
typedef unsigned long ULONG;
typedef unsigned char UBYTE;
#endif

typedef struct simplecli
	{
	char  pad1[60];
	ULONG Module;
	} simplecli;

typedef struct simpletask
	{
	char  pad1[58];
	ULONG StackLower,
	      StackUpper;
	char  pad2[62];
	ULONG SegList;
	char  pad3[40];
	ULONG CLI;
	ULONG *ReturnAddr;
	} simpletask;

typedef struct CallStack
	{
	struct CallStack *next;
	char  *AP_procname;
	ULONG  AP_ret;
	char  *AP_source;
	UWORD  AP_lineno;
	} CallStack;

typedef struct mchunk
	{
	ULONG deb,fin;              /* bounds included */
	char *name;
	UBYTE attr;
	} mchunk;

/****************************************************************************
 * Global variables
 ****************************************************************************/

#ifdef __PDC__
ULONG APflg          = 0;
#else
volatile ULONG APflg = 0;
#endif
char   *AP_procname  = "No procedure !";
char   *AP_source    = NULL;
UWORD   AP_lineno    = 0;
ULONG   AP_ret       = 0x00000000;
ULONG   AP_pc        = 0x00000000;

/****************************************************************************
 * Local variables
 ****************************************************************************/

PRIVATE int         c_pos       = 0;        /* for do_print() */
PRIVATE int         line_w      = 76;       /* our line width */
PRIVATE char        apdoll[80]  = {'0','x','\0'};
PRIVATE FILE       *STDLOG_;

PRIVATE simpletask *ThisTask;
PRIVATE ULONG       MallocProc;
PRIVATE ULONG       FreeProc;

PRIVATE CallStack  *stack       = NULL;
PRIVATE mchunk    **mfirst      = NULL;
PRIVATE int         nchunk      = 0;

PRIVATE int         totfreesize = 0;         /* total free memory kept      */
PRIVATE int         maxfreesize = 128*1024;  /* maximum free memory kept    */
PRIVATE int         immfreesize = 64*1024;   /* a block that is bigger than */
					     /* this is immediately freed   */

/****************************************************************************
 * Prototypes
 ****************************************************************************/

#ifdef  __SASC
#define STDARGS __stdargs
#else
#define STDARGS
#endif

#ifdef DEBUG
void                 dc(void);
#endif
extern void STDARGS *AsmAllocMem();
extern void STDARGS  AsmFreeMem();
extern void STDARGS *StdAlloc(ULONG,ULONG);
extern void STDARGS  StdFree(ULONG,ULONG);
void                 process_malloc(void);
void                 AP_Close(void);
mchunk              *addsyschunk(ULONG,ULONG,char *);

PRIVATE void         AP_exec_init(void);
PRIVATE void         AP_exec_close(void);
PRIVATE mchunk      *addchunk(ULONG,ULONG);
PRIVATE mchunk     **findchunk(mchunk *);
PRIVATE int          chunkcmp(mchunk *,mchunk *);

/****************************************************************************
 * MISC routines
 ****************************************************************************/

#ifdef __GNUC__
/*
 * getenv() does not access ENV: it seems. So use our own function.
 */
PRIVATE char *my_getenv(char *env)
{
    static char buffer[1024];
    FILE *f;
    char *s;

    if(s = getenv(env)) return s;
    strcpy(buffer,"/ENV/");
    strcat(buffer,env);
    if(f = fopen(buffer,"r")) {
	fgets(buffer,1023,f);
	fclose(f);
	buffer[1023]='\0';
	for(s=buffer;*s && *s!='\n';++s);
	*s='\0';
	return buffer;
    } else return NULL;
}
#define getenv my_getenv
/*
 * system() does not find rx
 */
PRIVATE int my_system(char *cmd)
{
    int ret;
    return SystemTagList(cmd,NULL);
}
#define system my_system
#endif

/*
 * Give an ascii representation of the block in a static array.
 */
PRIVATE char *blocks(mchunk *c)
	{
	static char buf[1024];
#ifndef __PDC__
	sprintf(buf,"[%s%08X(%d) %c%c%c%c (%s)]",
#else
	sprintf(buf,"[%s%08x(%d) %c%c%c%c (%s)]",
#endif
		apdoll,
		c->deb, c->fin-c->deb+1,
		(c->attr & attr_rd)  ? 'R':'-',
		(c->attr & attr_wr)  ? 'W':'-',
		(c->attr & attr_sys) ? 'S':'-',
		(c->attr & attr_fre) ? 'F':'-',
		SCHK(c->name));
	return buf;
	}

/*
 * printf() replacement with left-justification
 */
PRIVATE void do_dump(char *buff)
	{
	char *s;

	for(;*buff;buff = s)
		{
		int cut = 0;
		int cp  = c_pos;

		s = buff;
		
		while(!cut && (*s==' ' || *s=='\t'))
			{
			if(*s=='\t')
				{
				int ts = TABSIZE;
				while(s[1]=='\t') 
					{
					++s;
					ts += TABSIZE;
					}
				cp += ts;
				cp /= ts; 
				cp *= ts;
				}
			else    ++cp;
			++s;
			if(cp>line_w) cut=1;
			}
		while(!cut && *s!=' ' && *s!='\t' && *s!='\n' && *s)
			{
			++cp;
			if(cp>line_w) cut=1;
			++s;
			}
		
		s = buff;
		if(cut) 
			{
			while(*s==' ' || *s=='\t') ++s;
			fprintf(STDLOG,"\n");
			c_pos = 0;
			}
		cp = c_pos;
		while(*s==' ' || *s=='\t')
			{
			if(*s=='\t')
				{
				int ts = TABSIZE;
				while(s[1]=='\t')
					{
					++s;
					ts += TABSIZE;
					}
				cp += ts;
				cp /= ts;
				cp *= ts;
				}
			else    ++cp;
			++s;
			}
		while(c_pos<cp) {fprintf(STDLOG," ");++c_pos;}
		while(*s!=' ' && *s!='\t' && *s!='\n' && *s)
			{
			if(*s=='~') *s=' '; /* espace insecable */
			fprintf(STDLOG,"%c",*s++);
			++c_pos;
			}
		if(*s=='\n')
			{
			fprintf(STDLOG,"\n");
			c_pos = 0;
			++s;
			}
/*              buff = s;       */
		}
	}

#ifdef __SASC
PRIVATE void do_print(char *fmt,...)
	{
	va_list args;
	char *buff;
	va_start(args,fmt);

	ifn(buff = malloc(1024)) /* 1024 enough ? */
		{
		fprintf(STDLOG,WARNING" Can't alloc memory for \"");
		vfprintf(STDLOG, fmt, args);
		fprintf(STDLOG,"\"\n");
		return;
		}
	vsprintf(buff, fmt, args);
	va_end(args);
	do_dump(buff);
	free(buff);
	}
#else
PRIVATE void do_print(va_alist
#ifdef  _DCC
	,...
#endif
)       va_dcl
	{
	va_list args;
	char *fmt;
	char *buff;
	va_start(args);
	fmt = va_arg(args, char *);

	ifn(buff = malloc(1024)) /* 1024 enough ? */
		{
		fprintf(STDLOG,WARNING" Can't alloc memory for \"");
		vfprintf(STDLOG, fmt, args);
		fprintf(STDLOG,"\"\n");
		return;
		}
	vsprintf(buff, fmt, args);
	va_end(args);
	do_dump(buff);
	free(buff);
	}
#endif

/****************************************************************************
 * strings functions
 ****************************************************************************/

/*
 * A kind of strdup()
 */
PRIVATE char *strsave(char *s)
	{
	int l=strlen(s)+1;
	char *r;
	ifn(r=malloc(l)) return NULL;
	strcpy(r,s);
	return r;
	}

/*
 * save strcat(); !! 1st arg is freed
 */
PRIVATE char *catsave(char *s,char *t)
	{
	char *r;
	int l = 1;
	if(s) l += strlen(s);
	if(t) l += strlen(t);
	ifn(r=malloc(l)) return NULL;
	*r = '\0';
	if(s) {strcpy(r,s);free(s);}
	if(t) strcat(r,t);
	return r;
	}

/****************************************************************************
 * procname code routines
 ****************************************************************************/

/*
 * Returns the current procedure name in an allocated string
 */
PRIVATE char *procname(void)
	{
	char *ch = malloc(1024); /* I hope 1024 is enough */
	char *source,*name;
	UWORD line;

	if(!ch) return strsave("Not enough memory!");

	if(AP_procname)
		{
		source = AP_source;
		name   = AP_procname;
		line   = AP_lineno;
		}
	else if(stack)
		{
		source = stack->AP_source;
		name   = stack->AP_procname;
		line   = stack->AP_lineno;
		}
	else    {
		source = NULL;
		line   = 0;
		name   = "EMPTY AP_procname STACK !";
		}

	ch[0]='\0';
	if(source) sprintf(ch,"%s",source);
	if(line)   sprintf(ch+strlen(ch),":%d",line);
	sprintf(ch+strlen(ch),*ch?", %s":"%s",name);

	name = strsave(ch);free(ch);
	return name;
	}

/*
 * Find segment offset of address
 */
PRIVATE int findseg(char *s, ULONG pc, ULONG sl)
	{
	ULONG low,high;
	int segno = 0;
	int maxloop = 5000; /* to avoid infinite loops */

	sl <<= 2;
	while(sl && maxloop--)
		{
		low = sl-4; high = low + ((ULONG*)sl)[-1] - 1;
		if(low<=pc && pc<=high)
			{
#ifndef __PDC__
			sprintf(s," HUNK=%s%X OFFSET=%s%X",apdoll,segno,
							   apdoll,pc-sl-4);
#else
			sprintf(s," HUNK=%s%x OFFSET=%s%x",apdoll,segno,
							   apdoll,pc-sl-4);
#endif
			return 1;
			}
		sl = (*(ULONG*)sl)<<2;
		++segno;
		}
	return 0;
	}

/*
 * handle 68020+ extension word
 */
PRIVATE int skip_extension (UWORD *extension)
	{
	int next_extension_word = 1;

	/* base displacement */
	switch ((*extension & 0x0030) >> 4)
		{
		case 1: /* Null displacement */
		break;
		case 2: /* Word displacement */
		++next_extension_word;
		break;
		case 3: /* Long displacement */
		next_extension_word += 2;
		break;
		}

	/* outer displacement */
	switch (*extension & 0x0003)
		{
		case 1: /* Null displacement */
		break;
		case 2: /* Word displacement */
		++next_extension_word;
		break;
		case 3: /* Long displacement */
		next_extension_word += 2;
		break;
		}

	return next_extension_word;
	}

/*
 * returns the number of extension words used for ea 
 */
PRIVATE int skip_ea (UWORD *code)
	{
	short mode= (*code>>3)&7;
	short reg = (*code)&7;

	switch (mode)
		{
		case 0: /* Dn */
		case 1: /* An: should not occur for pea or jsr */
		case 2: /* (An)  */
		case 3: /* (An)+ */
		case 4: /* -(An) */
		return 0;

		case 5: /* N(An) */
		return 1;

		case 6: /* N(An,Dn*X) */
		if (code[1] & 0x100) return skip_extension(code+1);
		else return 1;
		
		case 7: /* mode 7: misc */ 
		switch(reg)
			{
			case 2:           /* d16(PC) */
			case 0: return 1; /* ABS.W */
			case 1: return 2; /* ABS.L */
			case 3:           /* N(PC,Dn) */
			if(code[1] & 0x100) return skip_extension(code+1);
			else return 1;
			case 4:           /* #N : illegal with pea && */
			default:
			return 0;
			}
		}
	}

/*
 * increase PC if it points to a pea followed by a jsr
 */
PRIVATE UWORD *hack_pc (UWORD *PC)
	{
#ifdef _unix_
	return NULL;
#else
	if((*PC & ~63)!=0x4840) return NULL; /* not pea */
	PC += 1+skip_ea(PC);
	if((*PC & ~63)==0x4e80) /* jsr */
		{
		PC += 1+skip_ea(PC);
		return PC;      
		}
	else if((*PC & ~255)==0x6100) /* bsr */
		{
		UWORD dis=*PC & 255;
		if(dis == 0) ++PC;         /* word offset */
		else if(dis == 255) PC+=2; /* long offset */
		return ++PC;
		}
	else return NULL;
#endif
	}

/*
 * Returns the current procedure name in an allocated string with PC
 * and offsets...
 */
PRIVATE char *fullprocname (void)
	{
	char ch[200];
	char *s;
	UWORD *PC=(void*)AP_pc,*PC2;

	/*
	 * increase PC if it points to a pea followed
	 * by a jsr
	 */
	if(PC2 = hack_pc(PC)) PC=PC2;

	s = procname();
#ifndef __PDC__
	sprintf(ch,"%s, PC=%s%08X",s,apdoll,PC);
#else
	sprintf(ch,"%s, PC=%s%08x",s,apdoll,PC);
#endif
	free(s);

	ifn(ThisTask) ThisTask = (simpletask *)FindTask(NULL);
	ifn(ThisTask) return strsave("FindTask(NULL) returned NULL !!!");

	if(ThisTask->SegList && 
	   !findseg(ch+strlen(ch),(ULONG)PC,((ULONG*)(ThisTask->SegList<<2))[3])
	   && ThisTask->CLI)
		{ /* from CLI */
		findseg(ch+strlen(ch),
			(ULONG)PC,((simplecli *)(ThisTask->CLI<<2))->Module);
		}
	return strsave(ch);
	}

/****************************************************************************
 * code for system chunks
 ****************************************************************************/

/*
 * Add memory chunks related with exec SegList
 */
PRIVATE void addsegchunk (ULONG sl,char *name)
	{
	ULONG high,bas;
	int maxloop=5000; /* to avoid infinite looping */

	sl <<= 2;
	while(sl && maxloop--)
		{
		bas = sl-4; high = bas + ((ULONG*)sl)[-1] - 1;
		ifn(addsyschunk(bas, high, name))
			{
			mchunk mc,**c;
			mc.deb  = bas;
			mc.fin  = high;
			if(!(c = findchunk(&mc)) ||
			   chunkcmp(&mc,*c)) 
				{
#ifndef __PDC__
				do_print(WARNING" Can't alloc block %s%08X -"
						" %s%08X (%s). That block will"
#else
				do_print(WARNING" Can't alloc block %s%08x -"
						" %s%08x (%s). That block will"
#endif 
						" be ignored.\n",
					 apdoll, bas, apdoll, high, name);
				}
			}
		sl = (*(ULONG*)sl)<<2;
		}
	}

/*
 * Process all the systems chunks: stack, seglist
 */
PRIVATE void process_sys_chunk (void)
	{
	int tmp;                          /* tmp must be someware in the stack */
	ULONG *sp=ThisTask->ReturnAddr-1; /* sp should point on the initial */
	mchunk *c;                        /* stack frame */

	ifn(c = addsyschunk(0,1023,"Basic 680x0 vectors"))
		{
		do_print(WARNING" Can't alloc block for basic vectors. That"
				" block will be ignored.\n");
		}
	else c->attr &= ~(attr_wr|attr_rd);

	ifn(c = addsyschunk(ThisTask->StackLower,
			    ThisTask->StackUpper - 1,
			    "standard stack frame of task"))
		{
		do_print(WARNING" Can't alloc block for standard stack frame"
				" of task. That block will be ignored:"
#ifndef __PDC__
				" %s%08X-%s%08X\n",
#else
				" %s%08x-%s%08x\n",
#endif

			 apdoll, ThisTask->StackLower,
			 apdoll, ThisTask->StackUpper - 1);
		}
	else c->attr |= attr_stk;

	/* sometimes the stack indicated by exec is not correct */
	/* in that case, we use CLI stuff */
	if((((ULONG)&tmp < ThisTask->StackLower) ||
	    ((ULONG)&tmp > ThisTask->StackUpper)) && ThisTask->CLI)
		{
		ULONG high = (ULONG)sp + 4 - 1,
		      bas  = (ULONG)sp + 4 - sp[1];
		ifn(c = addsyschunk(bas,high,"extra stack (CLI?) of task"))
			{
			do_print(WARNING" Can't alloc block for stack of"
					" task. That block will be ignored:"
#ifndef __PDC__
					" %s%08X-%s%08X\n",
#else
					" %s%08x-%s%08x\n",
#endif
				 apdoll, bas, apdoll, high);
			}
		else c->attr |= attr_stk;
		}

	addsegchunk(((ULONG*)(ThisTask->SegList<<2))[3],"segment process");

	if(ThisTask->CLI)
		{ /* from CLI */
		addsegchunk(((simplecli *)(ThisTask->CLI<<2))->Module,
			    "segment Module CLI");
		}
#ifdef __GNUC__
	/* ixemul stuff */

	/* ctype[] stuff */
	{
	mchunk mc;
	mc.deb = (ULONG)&_ctype_[0];
	mc.fin = (ULONG)&_ctype_[257];

	if((c = *findchunk(&mc)) && chunkcmp(c,&mc))
		{
		if(c = addsyschunk(mc.deb, mc.fin, "ctype[] array"))
			{
			c->attr &= ~attr_wr;
			}
		}
	}

	/* stdin/stderr/stdout stuff */
	{
	mchunk mc;

	mc.deb = (ULONG)stdin;
	mc.fin = mc.deb + sizeof(*stdin);
	if((c = *findchunk(&mc)) && chunkcmp(c,&mc))
		addsyschunk(mc.deb, mc.fin, "stdin datas");

	mc.deb = (ULONG)stdout;
	mc.fin = mc.deb + sizeof(*stdout);
	if((c = *findchunk(&mc)) && chunkcmp(c,&mc))
		addsyschunk(mc.deb, mc.fin, "stdout datas");

	mc.deb = (ULONG)stderr;
	mc.fin = mc.deb + sizeof(*stderr);
	if((c = *findchunk(&mc)) && chunkcmp(c,&mc))
		addsyschunk(mc.deb, mc.fin, "stderr datas");
	}
#endif
	}

/*
 * Display some warning
 */
PRIVATE void Warn (void)
	{
	char *s=fullprocname();
	fprintf(STDERR, WARNING" Library not opened (%s)\n", SCHK(s));
	if(s) free(s);
	fprintf(STDERR,"You should open the library in main() !\n");
	fprintf(STDERR,"Execution goes on without APurify until it is "
		       "(re)opened.\n");
	APflg |= APwarn;
	}

/*
 * Clear cache if OS can do it
 */
PRIVATE void AP_CacheClearU(void)
	{
	if(SysBase->LibNode.lib_Version>=36) CacheClearU();
	}

/****************************************************************************
 * public code
 ****************************************************************************/

/*
 * Open the stuff
 */
void AP_Init (void)
	{
	register ULONG *tmp;
	char *logname,*s;

	if(APflg & APopen) return; /* already opened */

	/* get $ or 0x */
	if(s = getenv(APDOLL))
		{
		strncpy(apdoll,s,79);
		apdoll[79]='\0';
		}

	/* setup STDLOG */
	if(logname = getenv(APLOG))
		{
		/* remove the ending \n that is left in PDC */
		{
		char *s=logname;
		while(*s++);s-=2;if(s>=logname && *s=='\n') *s='\0';
		}
		ifn(STDLOG_ = fopen(logname, "a"))
			{
			perror(logname);
			}
		}


	{ /* display some infos */
	time_t t=time(0);
	do_print("**** APurify started on %s",ctime(&t));
	}

	ThisTask    = (simpletask *)FindTask(NULL);
	mfirst      = malloc(2*sizeof(*mfirst));
	ifn(mfirst) exit(0);
	*mfirst++   = NULL;
	*mfirst     = NULL;
	nchunk      = 0;

	APflg       = APopen|APlock|APreport|APreq;

#ifndef __unix__
	Forbid();
	tmp         = (ULONG*)(((UBYTE*)StdAlloc)+14);
	MallocProc  = (ULONG)SetFunction((void*)*(ULONG*)4, LVOAllocMem, 
					 (void*)AsmAllocMem);
	*tmp        = MallocProc;
	tmp         = (ULONG*)(((UBYTE*)StdFree)+16);
	FreeProc    = (ULONG)SetFunction((void*)*(ULONG*)4, LVOFreeMem, 
					 (void*)AsmFreeMem);
	*tmp        = FreeProc;

	AP_CacheClearU();
	Permit();
#endif

	process_sys_chunk();
	atexit(AP_Close);
	process_malloc();

#ifdef DEBUG    
	dc();
#endif

	AP_exec_init();
	APflg      &= ~APlock;
	}

/*
 * openRAW
 */
PRIVATE FILE *openRAW(char *title)
{
    static char buf[256];
    FILE *f;
    char *s, *t=buf;

    strcpy(buf, "KRAW:");
    strcat(buf, title);
    strcat(buf, "/FONTtopaz.8");
    if((s=getenv(APSCR))) {
	strcat(buf,"/SCREEN");
	while(*t) ++t;
	while((*t=*s) && *s!='\n') ++t, ++s;
	*t = '\0';
    }
    f=fopen(buf, "r+");

    return f?f:fopen(buf+1, "r+");
}

/*
 * re Patched requester
 */
PRIVATE void repatched_req(char *what)
{
	FILE *console;

	console = openRAW("0/0/362/30/**** APURIFY REQUEST ****");

	fprintf(console?console:(STDERR),
			"%s is re-patched by an other task.\n"
			"Press any key when it is fixed...\07", what);
	fflush(console?console:(STDERR));
	if(console) {fgetc(console);fclose(console);}
	else getchar();
}

/*
 * Close it
 */
void AP_Close (void)
	{
	mchunk **c;
	int mustunlock;

	mustunlock=!(APflg & APlock);

	APflg |= APlock;
	ifn(APflg & APopen)
		{
/*              ifn(APflg & APwarn) Warn();
		APflg &= ~APlock;
*/              return;
		}

	for(c=mfirst;*c && ((*c)->attr & (attr_sys|attr_fre)); ++c);
	if(*c)
		{
		do_print(WARNING" Closing library without "
			 "deallocation of the following "
			 "block(s):\n");
		while(*c)
			{
			do_print("\t- %s\n",blocks(*c));
			for(++c;*c && ((*c)->attr&(attr_sys|attr_fre)); ++c);
			}
		for(c=mfirst;*c;++c)
			{mchunk *d=*c;
			if(d->name) free(d->name);
			if(d->attr&attr_fre) StdFree(d->deb, d->fin-d->deb+1);
			free(d);
			}
		free(mfirst-1);
		mfirst = NULL;
		nchunk = 0;
		}

#ifndef __unix__
retry:  Forbid();
	{ /* check if we've been re-patched */
	ULONG q=*(ULONG*)4;
	if(*(ULONG*)(q+2+LVOAllocMem)!=(ULONG)AsmAllocMem)
		{
		Permit();
		repatched_req("AllocMem()");
		Delay(50);
		goto retry;
		}
	if(*(ULONG*)(q+2+LVOFreeMem)!=(ULONG)AsmFreeMem)
		{
		Permit();
		repatched_req("FreeMem()");
		Delay(50);
		goto retry;
		}
	}
	SetFunction((void*)*(ULONG*)4, LVOAllocMem, (void*)MallocProc);
	SetFunction((void*)*(ULONG*)4, LVOFreeMem, (void*)FreeProc);
	AP_CacheClearU();
	Permit();
#endif

	{ /* display some infos */
	time_t t=time(0);
	do_print("\n**** APurify ended on %s",ctime(&t));
	}

	if(STDLOG_) {fclose(STDLOG_);STDLOG_=NULL;}

	AP_exec_close();

	APflg &= ~((mustunlock?APlock:0)|APopen);
	}

/*
 * Enable/Disable messages
 */
void AP_Report (int flag)
	{
	if(flag) APflg |=  APreport;
	else     APflg &= ~APreport;
	}

/*
 * Enable/Disable verbose mode
 */
void AP_Verbose(int flag)
	{
	if(flag) APflg |=  APverbose;
	else     APflg &= ~APverbose;
	}

/****************************************************************************
 * chunk manipulation routines
 ****************************************************************************/

/*
 * Compare 2 memory blocks with the following scheme:
 * []=c1   ()=c2
 *      -2    |     -1     |   0    |     1    |    2   |    3
 *    []   () |  [  ( ] )  | [ () ] | ( [ ) ]  | () []  | ( [] )
 */
PRIVATE int chunkcmp(mchunk *c1,mchunk *c2)
	{
	ULONG c1d,c1f,c2d,c2f;
	if((c1d = c1->deb) < (c2d = c2->deb))
		{
		if((c1f = c1->fin) <  c2d) return -2;
		if(c1f <= c2->fin) return -1;
		return 3;
		}
	if(c1d > (c2f = c2->fin))  return 2;
	if((c1f = c1->fin) <= c2f) return 0;
	if(c1f > c2f) return 1;
	}

#ifdef DEBUG
/*
 * dump complet des chunks... permet de voir si les listes sont bien
 * ordonnees
 */
PRIVATE void dc(void)
	{
	mchunk **c = mfirst;
	printf("dc(): ");
	while(*c)
		{
		printf("%s",blocks(*c));
		if(c[1]) printf(" %d ",chunkcmp(c[0],c[1]));
		++c;
		}
	printf("\n");
	}
#endif

/*
 * Find the memchunk that is the nearest of ck
 */
PRIVATE mchunk **findchunk(mchunk *ck)
	{
	register mchunk **array = mfirst,*cm;
	int first = 0;
	int last  = nchunk - 1;
	int mid;

	/* binary search */
	while( first < last ) 
		{
		mid = (first+last) >> 1;
		cm = array[mid];
		if(ck->fin < cm->deb)      last  = mid - 1;
		else if(ck->deb > cm->fin) first = mid + 1;
		else return( &array[ mid ] );
		}

	return &array[first];
	}

/*
 * internal add chunk in list.
 */
PRIVATE int i_addchunk(mchunk *c)
	{
	mchunk **mf,**d,**e;
	int last;
	
	++nchunk;
	ifn(mf=malloc((nchunk+2)*sizeof(*mf))) goto fail;
	*mf++=NULL;     /* sentinel */
	if(nchunk==1) {*mf=c;mf[1]=NULL;goto ok;}
	
	for(e=mf,d=mfirst;*d && (last=chunkcmp(*d,c))<-1;*e++=*d++);
	switch(last)
		{
		case 0:
/*              do_print(WARNING" Newly allocated block %s", blocks(c));
		do_print(" is already present!\n");
*/              goto fail;

		case -1:
		case 1:
		case 3:
		do_print(WARNING" Newly allocated block %s", blocks(c));
		do_print(" overlaps the already allocated block %s\n",
			 blocks(*d));
		goto fail;
		
		default:
		break;
		}
	*e++=c;
	for(;*e++=*d++;);
ok:     free(mfirst-1);
	mfirst = mf;
	return 1;
fail:   --nchunk;
	return 0;
	}

/*
 * Add a chunk starting at cd, ending at cf, with name 'name' and system
 * attribute.
 */
mchunk *addsyschunk(ULONG cd, ULONG cf, char *name)
	{
	mchunk *c;

	ifn(c = malloc(sizeof(*c))) return 0;
	c->deb  = cd;
	c->fin  = cf;
	c->name = strsave(name);
	c->attr = attr_rd|attr_wr|attr_sys;
	ifn(i_addchunk(c))
		{
		if(c->name) free(c->name);
		free(c);
		return NULL;
		}
	else return c;
	}

/*
 * Add a chunk starting at cd and ending at cf.
 */
PRIVATE mchunk *addchunk(ULONG cd, ULONG cf)
	{
	mchunk *c;

	ifn(c = malloc(sizeof(*c))) return NULL;
	c->deb  = cd;
	c->fin  = cf;
	c->name = procname();
	c->attr = attr_rd|attr_wr;
	ifn(i_addchunk(c))
		{
		if(c->name) free(c->name);
		free(c);
		return NULL;
		}
	else return c;
	}

/*
 * Give a chunk ready to delete.
 */
PRIVATE mchunk **chunk_to_delete(int len_to_add)
	{
	mchunk **cp,**bc = NULL;
	int bclen = len_to_add; /* we request at least that */

	for(cp=mfirst;*cp;++cp) if((*cp)->attr & attr_fre)
		{
		int len = (*cp)->fin - (*cp)->deb + 1;
		if(len > bclen)
		       {
		       bclen = len;
		       bc    = cp;
		       }
		}
	return bc;
	}

/*
 * Delete the chunk starting at cd and ending at cf.
 */
PRIVATE int subchunk(ULONG cd,ULONG cf)
	{
	mchunk **mf,**c,**d,**e,cc;
	int n,chunklen;
	
	ifn(n = nchunk-1)
		{char *s=procname();
		do_print(ERROR" Freeing memory in %s while no memory"
			      " block is allocated\n",SCHK(s));
		if(s) free(s);
		goto fail;
		}

	/* find the chunk in our list */
	cc.deb = cd;
	cc.fin = cf;
	c      = findchunk(&cc);
	if(cd!=(*c)->deb || cf!=(*c)->fin)
		{
#ifndef __PDC__
		do_print(ERROR" No memory block is matching %s%08X-%s%08X\n",
			 apdoll,cd,apdoll,cf);
#else
		do_print(ERROR" No memory block is matching %s%08x-%s%08x\n",
			 apdoll,cd,apdoll,cf);
#endif
		do_print("This occured while calling FreeMem() in %s\n",procname());
		if(c[-1]) do_print("Block is after %s\n",blocks(c[-1]));
		if(c[00]) do_print("Block is before %s\n",blocks(c[0]));
		goto fail;
		}

	/* check if that block has already been freed */
	if((*c)->attr & attr_fre)
		{
		do_print(ERROR" %s already freed",blocks(*c));
		do_print(" (%s).\n",procname());
		goto fail;
		}
	(*c)->attr |= attr_fre;
	(*c)->attr &= ~(attr_rd|attr_wr);

	/* check if we have to remove it */
	chunklen = cf - cd + 1;
	if(chunklen > immfreesize) goto remove;
	/* no: mark it as freed */
	if(totfreesize + chunklen <= maxfreesize)
		{
		totfreesize += chunklen;
		return 0;
		}
	/* no: delete another one and mark it as freed */
	if(d = chunk_to_delete(chunklen))
		{
		mchunk **src,**dst;
		ifn(mf=malloc((n+1)*sizeof(*mf))) goto remove;
		*mf++ = NULL;
		totfreesize -= (*d)->fin - (*d)->deb + 1;
		totfreesize += chunklen;
		src = mfirst;
		dst = mf;
		do {if(src == d) ++src;} while(*dst++ = *src++);
		free(mfirst-1);
		mfirst = mf;
		nchunk = n;
		return 0;
		}

remove: /* remove the chunk 'c' */
	ifn(mf=malloc((n+1)*sizeof(*mf))) goto fail;
	*mf++ = NULL;
	for(e=mf,d=mfirst;d!=c;*e++=*d++);
	for(++d;*e++=*d++;);
	free(mfirst-1);
	mfirst = mf;
	nchunk = n;
	return 1;

fail:   return 0;
	}

/*
 * Returns the list of calls
 */
PRIVATE char *CallChain(void)
	{
	char *ccstr,*source,*name;
	UWORD line;
	CallStack *stk = stack;

	if(AP_procname)
		{
		source = AP_source;
		name   = AP_procname;
		line   = AP_lineno;
		}
	else if(stk)
		{
		source = stk->AP_source;
		name   = stk->AP_procname;
		line   = stk->AP_lineno;
		stk    = stk->next;
		}
	else    {
		source = NULL;
		line   = 0;
		name   = "EMPTY AP_procname STACK !";
		}

	ccstr = strsave("(");
	if(source) ccstr = catsave(ccstr,source);
	if(line)
		{
		char buff[20];
		sprintf(buff,":%d",line);
		ccstr = catsave(ccstr,buff);
		}
	if(ccstr && *ccstr) ccstr = catsave(catsave(ccstr, ", "),name);
	else ccstr = catsave(ccstr,name);
	ccstr = catsave(ccstr,")");

	while(stk)
		{
		source = stk->AP_source;
		name   = stk->AP_procname;
		line   = stk->AP_lineno;
		stk    = stk->next;
		ccstr = catsave(ccstr," лл (");
		if(source) ccstr = catsave(ccstr,source);
		if(line)
			{
			char buff[20];
			sprintf(buff,":%d",line);
			ccstr = catsave(ccstr,buff);
			}
		if(ccstr && *ccstr) ccstr = catsave(catsave(ccstr, ", "),name);
		else ccstr = catsave(ccstr,name);
		ccstr = catsave(ccstr,")");
		}

	return ccstr;
	}

/****************************************************************************
 * APEXEC code
 ****************************************************************************/

/*
 * check whether APEXEC is set. if so, call it
 */
PRIVATE int stric(char **ps,char *t)
	{
	char *s = *ps;

	while(tolower(*s)==tolower(*t) && *s && *t) {++s;++t;}
	if(!*t) {*ps = s;return 0;} else return 1;
	}

PRIVATE void AP_exec(void)
	{
	char *env,*cmd,*s;
	char *source;
	UWORD line;
	int ret;
	static int hitno = 0;
	
	++hitno;

	if(AP_procname)
		{
		source = AP_source;
		line   = AP_lineno;
		}
	else if(stack)
		{
		source = stack->AP_source;
		line   = stack->AP_lineno;
		}
	else    return;
	
	ifn(source) return;
	ifn(env = getenv(APEXEC)) return;
	ifn(cmd = malloc(1024)) return; /* 1024 enough ? */
	for(s=cmd;*env && s<cmd+1020;)
		{
		ifn(stric(&env,"$SOURCE"))
			{
			char *t = source,*u;
			for(u=t;*u && *u!=' ' && *u!='\t';++u);
			if(*u) *s++='\"';
			while((*s = *t) && s<cmd+1020) {++s;++t;}
			if(*u) *s++='\"';
			}
		else ifn(stric(&env,"$LINE"))
			{
			char buff[16],*t=buff; 
			sprintf(buff,"%d", line); 
			while((*s = *t) && s<cmd+1020) {++s;++t;}
			}
		else ifn(stric(&env,"$HITNO"))
			{
			char buff[16],*t=buff; 
			sprintf(buff,"%d", hitno); 
			while((*s = *t) && s<cmd+1020) {++s;++t;}
			}
		else ifn(stric(&env,"$CALLCHAIN"))
			{
			char *t=CallChain();
			char *b=t;
			while((*s = *t) && s<cmd+1020) {++s;++t;}
			free(b);
			}
		else *s++=*env++;
		}
	*s='\0';
	ret=system(cmd);
	free(cmd);
	if(ret==0) return;
	exit(ret);
	}

PRIVATE void AP_exec_init(void)
	{
	char *env,*cmd,*s;

	ifn(env = getenv(APEXEC)) return;
	ifn(cmd = malloc(1024)) return; /* 1024 enough ? */
	for(s=cmd;*env && s<cmd+1020;)
		{
		if(!stric(&env,"$SOURCE") || !stric(&env,"$CALLCHAIN"))
			{
			char *t = "\"\"";
			while((*s = *t) && s<cmd+1020) {++s;++t;}
			}
		else if(!stric(&env,"$LINE") || !stric(&env,"$HITNO"))
			{
			char *t="0"; 
			while((*s = *t) && s<cmd+1020) {++s;++t;}
			}
		else *s++=*env++;
		}
	*s='\0';
	system(cmd);
	free(cmd);
	}

PRIVATE void AP_exec_close(void)
	{
	char *env,*cmd,*s;

	ifn(env = getenv(APEXEC)) return;
	ifn(cmd = malloc(1024)) return; /* 1024 enough ? */
	for(s=cmd;*env && s<cmd+1020;)
		{
		if(!stric(&env,"$SOURCE") || !stric(&env,"$CALLCHAIN"))
			{
			char *t = "\"\"";
			while((*s = *t) && s<cmd+1020) {++s;++t;}
			}
		else ifn(stric(&env,"$LINE"))
			{
			char *t="0"; 
			while((*s = *t) && s<cmd+1020) {++s;++t;}
			}
		else ifn(stric(&env,"$HITNO"))
			{
			char *t = "-1"; 
			while((*s = *t) && s<cmd+1020) {++s;++t;}
			}
		else *s++=*env++;
		}
	*s='\0';
	system(cmd);
	free(cmd);
	}

/****************************************************************************
 * some input/ouput code
 ****************************************************************************/

/*
 * code that is called in case of an error. You can put breakpoints
 * on it to stop just before the error had happened
 */
void AP_Err(void)
	{
	AP_exec();
	}

/*
 * code that is called in case of dangerous errors: You can stop the
 * program before it crashes.
 */
PRIVATE void AP_Req(void)
	{
	int rep;
	int ok;
	FILE *console;
	ifn(APflg & APreport) return;
	ifn(APflg & APreq) return;
	ifn(console = openRAW("0/0/384/69/**** APURIFY REQUEST ****"))
		{
		do_print(WARNING" Can't open requester for last error!\n");
		rep = 'i';
		goto end;
		}
	fprintf(console,"\033[1mThe last error detected by APurify may"
		 " crash your computer !\033[0m\07\n");
	fprintf(console,"You can:\n");
	fprintf(console,"\t-stop your program (reply 's')\n");
	fprintf(console,"\t-ignore that error (reply 'i')\n");
	fprintf(console,"\t-ignore all errors (reply 'a')\n");
	fprintf(console,"Your choice: ");fflush(console);
	ok=0;
	do      {
		rep=fgetc(console);
		if(rep=='s' || rep=='i' || rep=='a' || rep==EOF) ok=1;
		else {fprintf(console,"\07");fflush(console);}
		}
	while(!ok);
	fclose(console);
end:    if(rep=='i' || rep==EOF) 
		{
		do_print("Error ignored on user request\n");
		}
	else if(rep=='s') 
		{
		do_print("Execution stopped on user request\n");
	
		AP_Close();
		exit(20);
		}
	else    {
		do_print("Next errors ignored on user request\n");
		APflg &= ~APreq;
		}
	AP_Err();
	}

/*
 * Error-dumping code for access between two blocks
 */
PRIVATE void entre(char *type,
		  mchunk *c,
		  mchunk *c1,
		  mchunk *c2) /* acces entre 2 blocks */
	{
	ifn(APflg & APreport) return;

	ifn(APflg & APverbose)
		{ /* requested by  Joerg-Cyril Hoehle */
		do_print(ERROR" %s accessed ",blocks(c));
		ifn(c1)
			{
			do_print("before:\n\t%+d\t\t\t%s\n",
				c2->deb - c->fin,blocks(c2));
			}
		else ifn(c2)
			{
			do_print("after:\n\t%+d\t\t\t%s\n",
				c->deb - c1->fin,blocks(c1));
			}
		else
			{
			int d1,d2;
			do_print("between:\n");
			d1 = c->deb - c1->fin;
			if(d1<0) d1=-d1;
			d2 = c->fin - c2->deb;
			if(d2<0) d2=-d2;
			if(d1 <= d2)
				{
				do_print("\t%+d\t\t\t%s\n",
					 c->deb - c1->fin,blocks(c1));
				do_print("\t%+d\t\t\t%s\n",
					 c->fin - c2->deb,blocks(c2));
				}
			else
				{
				do_print("\t%+d\t\t\t%s\n",
					 c->fin - c2->deb,blocks(c2));
				do_print("\t%+d\t\t\t%s\n",
					 c->deb - c1->fin,blocks(c1));
				}
			}
		}
	else
		{
		do_print(ERROR" Accessing %s\n",blocks(c));
		do_print("Illegal %s ", type);

		ifn(c1)      do_print("before block %s (overstepping of %d"
				      " byte(s))",
				      blocks(c2), c2->deb - c->deb);
		else ifn(c2) do_print("after block %s (overstepping of %d"
				      " byte(s))",
				      blocks(c1), c->fin - c1->fin);
		else         {
			     do_print("between blocks ");
			     do_print("%s (overstepping of %d byte(s)) and ",
				      blocks(c1), c->deb - c1->fin);
			     do_print("%s (overstepping of %d byte(s))",
				      blocks(c2),c->fin - c2->deb);
			     }
		do_print("\n");
		}
	AP_Err();
	}

/*
 * Same as above but for an overstepping of a block.
 */
PRIVATE void achev(char *type,
		  mchunk *c,
		  mchunk *c1)   /* a cheval sur un block */
	{
	int d;

	ifn(APflg & APreport) return;

	d = c1->deb - c->deb;
	if(d<0) d = c1->fin - c->fin;
	ifn(APflg & APverbose)
		{
		do_print(ERROR" %s accessed across the %s boundary of:\n",
			blocks(c),d>0?"beginning":"ending");
		do_print("\t%+d\t\t\t%s\n",d,blocks(c1));
		}
	else
		{
		do_print(ERROR" Accessing %s\n", blocks(c));
		do_print("Overstepping %s on block %s ", type, blocks(c1));
		if(d<0) do_print("of %d byte(s) rightward\n", -d);
		else    do_print("of %d byte(s) leftward\n",   d);
		}
	AP_Err();
	}

/****************************************************************************
 * memory access check code
 ****************************************************************************/

/*
 * Check access for reading.
 */
void AP_rd(ULONG pt,ULONG len)
	{
	struct mchunk mc,**c;
	ifn(APflg & APopen) {ifn(APflg & APwarn) Warn();return;}
	APflg |= APlock;
#ifdef DEBUG
	{char *s=procname();
#ifndef __PDC__
	printf("Read : %08X-%08X par %s\n",pt,pt+len-1,SCHK(s));   /**/
#else
	printf("Read : %08x-%08x par %s\n",pt,pt+len-1,SCHK(s));   /**/
#endif
	if(s) free(s);
	}
#endif
	mc.deb  = pt;
	mc.fin  = pt + len - 1;
	/* will be set only if an error occur...don't need to waste time &
	   memory for that */
	mc.name = NULL;
	mc.attr = attr_rd;
	ifn(c = findchunk(&mc))
		{char *s=procname();
		do_print(ERROR" No memory block allocated (%s) !\n",SCHK(s));
		if(s) free(s);
		}
	else
		{
		switch(chunkcmp(&mc,*c))
			{
			case -2:
			mc.name = fullprocname();
			entre("reading", &mc, c[-1], c[0]);
			break;

			case 2:
			mc.name = fullprocname();
			entre("reading", &mc, c[0], c[1]);
			break;

			case -1:
			mc.name = fullprocname();
			achev("reading", &mc, c[0]);
			break;

			case 1:
			mc.name = fullprocname();
			achev("reading", &mc, c[0]);
			break;

			case 3:
			mc.name = fullprocname();
			do_print(ERROR" Strange, impossible error: block %s",
				 blocks(&mc));
			do_print("is overlapping block %s\n", blocks(c[0]));
			break;

			case 0:
			ifn((mc.attr & c[0]->attr) == mc.attr)
				{
				mc.name = fullprocname();
				if(APflg & APreport)
{
ifn(APflg & APverbose)
	{
	if(c[0]->attr & attr_fre)
		do_print(ERROR" %s accessed on freed memory:\n",
			 blocks(&mc));
	else
		do_print(ERROR" %s accessed on a read-protected block:\n",
			 blocks(&mc));

	do_print("\t%+d\t\t\t%s\n", mc.deb-c[0]->deb, blocks(c[0]));
	}
else
	{
	do_print(ERROR" Accessing %s\n", blocks(&mc));
	do_print("Protection error in reading block %s\n", blocks(c[0]));
	}
}
				AP_Err();
				}
			else if(c[0]->attr & attr_stk)
				{
				ULONG tmp;
				ULONG stackbase = (ULONG)&tmp;

				if((c[0]->deb < stackbase) &&
				   (stackbase < c[0]->fin) &&
				   (pt < stackbase))
					{
					mc.name = fullprocname();
if(APflg & APreport)
	{
	LONG oldbase = c[0]->deb;
	c[0]->deb = stackbase;
	do_print(ERROR" %s below stack pointer:\n",blocks(&mc));
	do_print("\t%+d\t\t\t%s\n", mc.deb-c[0]->deb, blocks(c[0]));
	c[0]->deb = oldbase;
	}
					AP_Err();
					}
				}
			break;
			}
		}

	if(mc.name) free(mc.name);
	APflg &= ~APlock;
	}

/*
 * Check access for writing.
 */
void AP_wr(ULONG pt, ULONG len)
	{
	struct mchunk mc,**c;
	ifn(APflg & APopen) {ifn(APflg & APwarn) Warn();return;}
	APflg |= APlock;
#ifdef  DEBUG
	{char *s=procname();
#ifndef __PDC__
	printf("Write: %08X-%08X par %s\n",pt,pt+len-1,SCHK(s));   /**/
#else
	printf("Write: %08x-%08x par %s\n",pt,pt+len-1,SCHK(s));   /**/
#endif
	if(s) free(s);
	}
#endif
	mc.deb  = pt;
	mc.fin  = pt + len - 1;
	/* will be set only if an error occur...don't need to waste time &
	   memory for that */
	mc.name = NULL;
	mc.attr = attr_wr;
	ifn(c = findchunk(&mc))
		{char *s=procname();
		do_print(ERROR" No memory block allocated (%s)\n", SCHK(s));
		if(s) free(s);
		}
	else
		{
		switch(chunkcmp(&mc,c[0]))
			{
			case -2:
			mc.name = fullprocname();
			entre("writing", &mc, c[-1], c[0]);
			AP_Req();
			break;

			case 2:
			mc.name = fullprocname();
			entre("writing", &mc, c[0], c[1]);
			AP_Req();
			break;

			case -1:
			mc.name = fullprocname();
			achev("writing", &mc, c[0]);
			AP_Req();
			break;

			case 1:
			mc.name = fullprocname();
			achev("writing", &mc, c[0]);
			AP_Req();
			break;

			case 3:
			mc.name = fullprocname();
			do_print(ERROR" Strange, impossible error: block %s ",
				 blocks(&mc));
			do_print("is overlapping block %s\n",blocks(c[0]));
			AP_Req();
			break;

			case 0:
			ifn((mc.attr & c[0]->attr) == mc.attr)
				{
				mc.name = fullprocname();
				if(APflg & APreport)
{
ifn(APflg & APverbose)
	{
	if(c[0]->attr & attr_fre)
		do_print(ERROR" %s accessed on freed memory:\n",
			 blocks(&mc));
	else
		do_print(ERROR" %s accessed on a write-protected block:\n",
			 blocks(&mc));

	do_print("\t%+d\t\t\t%s\n", mc.deb-c[0]->deb, blocks(c[0]));
	}
else
	{
	do_print(ERROR" Accessing %s\n", blocks(&mc));
	do_print("Protection error in writing block %s\n", blocks(c[0]));
	}
}
				AP_Req();
				}
			else if(c[0]->attr & attr_stk)
				{
				ULONG tmp;
				ULONG stackbase = (ULONG)&tmp;

				if((c[0]->deb < stackbase) &&
				   (stackbase < c[0]->fin) &&
				   (pt < stackbase))
					{
					mc.name = fullprocname();
if(APflg & APreport)
	{
	LONG oldbase = c[0]->deb;
	c[0]->deb = stackbase;
	do_print(ERROR" %s below stack pointer:\n",blocks(&mc));
	do_print("\t%+d\t\t\t%s\n", mc.deb-c[0]->deb, blocks(c[0]));
	c[0]->deb = oldbase;
	}
					AP_Err();
					}
				}
			break;
			}
		}
	if(mc.name) free(mc.name);
	APflg &= ~APlock;
	}

/****************************************************************************
 * context handling
 ****************************************************************************/

/*
 * Push current context (AP_procname, AP_ret) on a stack to prevent
 * it's destruction by recursive calls.
 */
int AP_protect(void)
	{
	CallStack *s;
	APflg |= APlock;
	ifn(s = malloc(sizeof(*s)))
		{
		APflg &= ~APlock;
		return 0;
		}
	s->next        = stack;
	s->AP_procname = AP_procname;
	s->AP_ret      = AP_ret;
	s->AP_source   = AP_source;
	s->AP_lineno   = AP_lineno;
	AP_procname    = NULL;
	AP_source      = NULL;
	AP_lineno      = 0;
	stack          = s;
	APflg &= ~APlock;
	return 1;
	}

/*
 * Pop the saved context.
 */
void AP_unprotect(void)
	{
	CallStack *s;
	APflg |= APlock;
	s = stack;stack = s->next;
	AP_procname = s->AP_procname;
	AP_ret      = s->AP_ret;
	AP_source   = s->AP_source;
	AP_lineno   = s->AP_lineno;
	free(s);
	APflg &= ~APlock;
	}

/****************************************************************************
 * Memory (de)allocation code
 ****************************************************************************/

/*
 * C procedure that will be called by Execbase/AllocMem(). It is used
 * by all programs. If it is not the tested program that calls this
 * function, we call the ROM routine.
 */
#ifdef  __SASC
__saveds
#endif
ULONG PubAllocMem(ULONG len, ULONG type)
	{
	register ULONG p;

#ifdef _DCC
	geta4();
#endif
	if((simpletask *)FindTask(NULL)!=ThisTask) goto normal;

	ifn(APflg & APopen)
		{
		ifn(APflg & APwarn) Warn();
		goto normal;
		}
	if(APflg & APlock) goto normal;

	APflg |= APlock;

	if(p = (ULONG)StdAlloc(len, type))
	ifn(addchunk(p, p + len - 1)) {StdFree(p, len);p = (ULONG)NULL;}

#ifdef DEBUG
	{char *s=procname();
#ifndef __PDC__
	fprintf(STDERR, "Malloc(%d,%d)=%08X par %s\n", len, type, p, SCHK(s));
#else
	fprintf(STDERR, "Malloc(%d,%d)=%08x par %s\n", len, type, p, SCHK(s));
#endif
	if(s) free(s);
	}
	dc(); /**/
#endif
	APflg &= ~APlock;
/*      Permit(); */
	return p;
normal:
	p = (ULONG)StdAlloc(len, type);
	return p;
	}

/*
 * C procedure that will be called by Execbase/FreeMem(). It is used
 * by all programs. If it is not the tested program that calls this
 * function, we call the ROM routine.
 */
#ifdef  __SASC
__saveds
#endif
void PubFreeMem(ULONG ptr, ULONG len)
	{
#ifdef _DCC
	geta4();
#endif
	if((simpletask *)FindTask(NULL)!=ThisTask) goto normal;
	ifn(APflg & APopen)
		{
		ifn(APflg & APwarn) Warn();
		goto normal;
		}
	if(APflg & APlock) goto normal;

	APflg |= APlock;
#ifdef DEBUG
	{char *s=procname();
#ifndef __PDC__
	fprintf(STDERR, "Free(%08X,%d) par %s\n", ptr, len, SCHK(s));
#else
	fprintf(STDERR, "Free(%08x,%d) par %s\n", ptr, len, SCHK(s));
#endif
	if(s) free(s);
	}
#endif
	if(subchunk(ptr, ptr + len - 1)) StdFree(ptr, len);

	APflg &= ~APlock;
	return;
normal:
	StdFree(ptr, len);
	}

/*
#ifdef _unix_
simpletask *FindTask() {}
Enable() {}
void *AsmAllocMem() {}
SetFunction() {}
void StdFree() {}
Disable() {}
void *StdAlloc() {}
void AsmFreeMem() {}

main()
{
printf("%d\n",sizeof(mchunk));
}
#endif
*/
