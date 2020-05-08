/*
 *	utils.c
 *
 * (c) by Samuel DEVULDER, 1995
 */

#include "apf.h"
#ifdef __GNUC__
/*#include <sys/time.h>*/	/* dosextens.h needs it ! */
#endif
#include <exec/exec.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#ifdef __SASC
#include <proto/dos.h>
#include <proto/exec.h>
#endif

/*
 * s_malloc: a 'safe' malloc()
 */
void *s_malloc(unsigned long len)
	{
	void *p;
	p=malloc(len);
	ifn(p)	{
		printf("%s: Out of mem (%d)\n",ProgName,len);
		exit(5);
		}
	return p;
	}

/*
 * s_calloc: a 'safe' calloc()
 */
void *s_calloc(long nelt,long eltsize)
	{
	void *p;
	p=calloc(nelt,eltsize);
	ifn(p)	{
		printf("%s: Out of mem (%d)\n",ProgName,nelt*eltsize);
		exit(5);
		}
	return p;
	}

/*
 * s_strdup: a 'safe' strdup()
 */
char *s_strdup(char *s)
	{
	s = strdup(s);
	ifn(s)	{
		printf("%s: Out of mem (\"%S\")\n",s);
		exit(5);
		}
	return s;
	}

/*
 * returns file basename: a:b/c/d ==> d
 */
char *basename(char *s)
	{
	char *t;
	for(t=s;*t;++t);
	while(t>s && !(t[-1]=='/' || t[-1]==':')) --t;

	return t;
	}

/*
 * returns file extension: a:b/c/d.e ==> e
 */
char *extension(char *s)
	{
	char *t;
	for(t=s;*t;++t);
	while(t>s && t[-1]!='.' && t[-1]!='/' && t[-1]!=':') --t;
	if(t>s && t[-1]=='.') return t;
	else return "";
	}

/*
 * returns file without its extension: a:b/c/d.e ==> a:b/c/d (malloc used)
 */
char *noextension(char *s)
	{
	char *t;

	s = s_strdup(s);
	for(t=s;*t;++t);
	while(t>s && t[-1]!='.' && t[-1]!='/' && t[-1]!=':') --t;
	if(t>s && t[-1]=='.') t[-1]='\0';
	return s;
	}

/*
 * duplicates commandline
 */
void duplicate_AC_AV(int ac, char **av)
	{
	char **tav;

	AC = ac;
	AV = tav = s_calloc(ac+1,sizeof(*av));

	while(ac--) *tav++=*av++;
	*tav = NULL; /* ANSI NEEDS NULL IN THE END */
	}

/*
 * add one argument in the end
 */
void add_arg(char *narg)
	{
	char **av1,**av2,**av3;
	int ac;

	ac  = ++AC;
	av3 = av2 = AV;
	av1 = AV = s_calloc(ac+1,sizeof(*AV));
	while(--ac) *av1++=*av2++;
	*av1++ = s_strdup(narg);
	*av1 = NULL;
	free(av3);
	}

/*
 * delete last argument
 */
void sub_arg(void)
	{
	char **av1,**av2,**av3;
	int ac;

	ac  = --AC;
	av3 = av2 = AV;
	av1 = AV = s_calloc(ac+1,sizeof(*AV));
	free(av2[ac]);	
	while(ac--) *av1++=*av2++;
	*av1 = NULL;
	free(av3);
	}

/*
 * splits the arguments into 2 arguments
 */
void split_arg(int split_point, char *where)
	{
	char **av1,**av2,**av3;
	int ac,_AC;

	ifn(*where) return;

	_AC = ++AC;
	av3 = av2 = AV;
	av1 = AV  = s_calloc(_AC+1,sizeof(*AV));

	if(SourceIdx > split_point) ++SourceIdx;
	if(DestIdx   > split_point) ++DestIdx;
	if(LinkIdx   > split_point) ++LinkIdx;

	for(ac = 0; ac<split_point; ++ac) *av1++ = *av2++;
	*av1++ = *av2++;++ac;
	*av1++ = s_strdup(where);++ac;
	*where='\0';

	while(ac<_AC) {*av1++=*av2++;++ac;}
	*av1=NULL;	/* ANSI needs this */

	free(av3);
	}

/*
 * enable or disable the volume requester
 */
PRIVATE void set_req(bool ok)
	{
	static ULONG wd = 0;
	struct Process *pr;
	
	pr = (void*)FindTask(NULL);

	if(pr->pr_Task.tc_Node.ln_Type - NT_PROCESS) return;

	if(ok)	{
		pr->pr_WindowPtr = (APTR)wd;
		}
	else	{
		wd = (ULONG)pr->pr_WindowPtr;
		pr->pr_WindowPtr = (APTR)-1;
		}
	}

/*
 * check the existance of a C file (.c is append)
 */
int access_c(char *n)
	{
	char *fn;
	int ret;

	fn=s_malloc(strlen(n)+3);
	strcpy(fn,n);
	strcat(fn,".c");
	set_req(0);
	ret=access(fn,0);
	set_req(1);
	free(fn);
	return ret>=0;
	}

/*
 * run the current line
 */
void execute(void)
	{
	int l;
	int ac;
	char *cmd,*s,*t;

	l=strlen(AV[0])+1;
	for(ac=1;ac<AC;++ac) if(*AV[ac]) l+=1+strlen(AV[ac]);

	cmd = s_malloc(l);

	for(s=cmd,t=AV[0];*s=*t;++s,++t);
	for(ac=1;ac<AC;++ac) if(*AV[ac]) {*s++=' ';for(t=AV[ac];*s=*t;++s,++t);}
	*s='\0';

	if(verbose) printf("%s: \033[33m%s\033[31m\n",ProgName, cmd);
	if(l=system(cmd)) exit(l);
	free(cmd);
	}

/*
 * run apurify
 */
void apurify(char *parser,char *opt,char *src,char *dst)
	{
	char *cmd;
	int l;

	cmd = s_malloc(1+strlen(parser)+1+
			 (opt?strlen(opt)+1:0)+
			 (source_level?6+strlen(AV[SourceIdx]):0)+
			 strlen(src)+4+strlen(dst));

	strcpy(cmd,parser);strcat(cmd," ");
	if(opt) {strcat(cmd,opt);strcat(cmd," ");}
	if(source_level) {strcat(cmd,"-sl ");strcat(cmd,AV[SourceIdx]);strcat(cmd," ");}
	strcat(cmd,src);strcat(cmd," -o ");
	strcat(cmd,dst);

	if(verbose) printf("%s: \033[33m%s\033[31m\n",ProgName, cmd);
	if(l=system(cmd)) exit(l);
	free(cmd);
	}

/*
 * a kind of rm...
 */
void Unlink(file)
	char *file;
	{
	if(verbose) printf("%s: \033[33mdelete %s\033[31m\n",ProgName,file);
	unlink(file);
	}

/*
 * a kind of cp...
 */
void copy(src,dst)
	char *src;
	char *dst;
	{
	long f,g;
	char *buff;
	int i,l;
	
	buff = s_malloc(l=4096);

	if(verbose) printf("%s: \033[33mcopy %s to %s\033[31m\n",ProgName,src,dst);
 
	ifn(f = Open(src, MODE_OLDFILE))
		{
		printf("%s: Can't open file \"%s\" for intput\n",
			ProgName,src);
		exit(5);
		}
	ifn(g = Open(dst, MODE_NEWFILE))
		{
		Close(f);
		printf("%s: Can't open file \"%s\" for output\n",
			ProgName,dst);
		exit(5);
		}
	while (i = Read(f, buff, l)) if (Write(g, buff, i) != i)
		{
		if(IoErr())
			{
			printf("%s: Error in copying %s to %s !\n",
				ProgName,src,dst);
			Close(f);
			Close(g);
			exit(5);
			}
		else	break;
		}
	Close(f);
	Close(g);
	free(buff);
	}

/*
 * sets the source file
 */
void set_source(int ac)
	{
	if(SourceIdx)
		{
		printf("%s: Source already set to \"%s\" !",
			ProgName,AV[SourceIdx]);
		printf(" What is \"%s\" ?\n",AV[ac]);
		exit(5);
		}
	SourceIdx = ac;
	if(ac>=AC)
		{
		printf("%s: Missing argument after \"%s\"\n",
			ProgName,AV[ac-1]);
		exit(5);
		}
	}

/*
 * sets the dest file
 */
void set_dest(int ac)
	{
	if(DestIdx)
		{
		printf("%s: Destination already set to \"%s\" !",
			ProgName,AV[SourceIdx]);
		printf(" What is \"%s\" ?\n",AV[ac]);
		exit(5);
		}
	DestIdx = ac;
	if(ac>=AC)
		{
		printf("%s: Missing argument after \"%s\"\n",
			ProgName,AV[ac-1]);
		exit(5);
		}
	}

/*
 * compile with source and arg
 */
void process(char *src,char *to,char *dst,char *lnk)
	{
	char **av    = AV;
	char *source = av[SourceIdx];
	char *to2    = av[DestIdx-1];
	char *dest   = av[DestIdx];

	if(src) av[SourceIdx] = src;
	if(to)  av[DestIdx-1] = to;
	if(dst) av[DestIdx]   = dst;
	
	if(lnk)	{
		if(LinkIdx) 
			{
			char *link = av[LinkIdx];

			av[LinkIdx] = lnk;
			execute();
			av[LinkIdx] = link;
			}
		else 	{
			add_arg(lnk);
			execute();
			sub_arg();
			av=AV;
			}
		}
	else execute();

	if(src) av[SourceIdx] = source;
	if(to)  av[DestIdx-1] = to2;
	if(dst) av[DestIdx]   = dest;
	}
