/*
 *	args.c
 *
 * (c) by Samuel DEVULDER, 1995
 */

#include "apf.h"

typedef struct args
	{
	char *arg;
	char *desc;
	char *(*proc)(
#ifdef	__SASC
		     ...
#endif
		     );
	void *argptr;
	char *argdesc;
	} args;

#define HAS_ARG(s)	(*(s)=='*')
#define GIVE_ARG	((char*)-1)
#define GIVE_DEFAULT	NULL

void usage(void);

void wrong_arg(char *a)
	{
	printf("%s: Incorrect argument ! (\"%s\")\n",ProgName,a);
	usage();
	}

#ifndef __PDC__
#define FTYPE PRIVATE char *
#else
#define FTYPE char *
#endif

FTYPE set_ON(char *n,bool *val)  {*val = TRUE;return NULL;}
FTYPE set_OFF(char *n,bool *val) {*val = FALSE;return NULL;}
FTYPE set_ON_OFF(char *n,bool *val)
	{
	if(n==GIVE_ARG)			return "1=ON|0=OFF";
	if(n==GIVE_DEFAULT)		return (*val)?"ON":"OFF";
	ifn(stricmp(n,"1"))		(*val)=TRUE;
	else ifn(stricmp(n,"ON"))	(*val)=TRUE;
	else ifn(stricmp(n,"0"))  	(*val)=FALSE;
	else ifn(stricmp(n,"OFF"))	(*val)=FALSE;
	else				return (char*)(1);
	return NULL;
	}
FTYPE set_STR(char *n,char **val,char *arg)
	{
	if(n==GIVE_ARG)     return arg;
	if(n==GIVE_DEFAULT) return (*val);
	(*val) = n;
	return NULL;
	}

PRIVATE args targs[]=
	{
	{"?",	"To display this usage",			(void*)usage},
	{"-v",	"Use verbose mode",				set_ON,&verbose},
	{"-sl", "Use source-level information",			set_ON,&source_level},
	{"*-mitparser", "Specifies parser for mit syntax", 	set_STR,&mit_parser,"path"},
	{"*-motparser", "Specifies parser for mot syntax",	set_STR,&mot_parser,"path"},
	{"*-gcclib",	"Specifies apurify library for gcc",	set_STR,&gcclib,"library"},
	{"*-pdclib",	"Specifies apurify library for pdc",	set_STR,&pdclib,"library"},
	{"*-dcclib",	"Specifies apurify library for dcc",	set_STR,&dcclib,"library"},
	{"*-saslib",	"Specifies apurify library for sas",	set_STR,&saslib,"library"},

	{NULL,NULL,NULL,NULL,NULL}	/* sentinel */
	};

/*
 * generate automatically the usage according to the array
 */
void usage(void)
	{
	int maxa,hasa;
	args *ta;
	printf("\nUsage: %s [-revinfo] [\033[3moptions\033[23m] <command line>\n",ProgName);
	printf("\nWhere \033[3moptions\033[23m can be:\n");
	printf("\n");
	maxa=hasa=0;
	for(ta=targs;ta->arg;++ta)
		{
		char *s=ta->arg;
		int l;
		if(HAS_ARG(s)) 
			{
			++s;
			l = 1 + strlen(s) +
			    strlen((*ta->proc)(GIVE_ARG,ta->argptr,ta->argdesc));
			}
		else l = strlen(s);
		if(l>maxa) maxa = l;
		}
	for(ta=targs;ta->arg;++ta)
		{
		char *s=ta->arg;
		int i,ha=0;
		printf("  ");
		if(HAS_ARG(s))
			{
			char *t;
			ha=1;
			++s;
			printf("%s \033[2m%s\033[22m",s,
				t=(*ta->proc)(GIVE_ARG,ta->argptr,ta->argdesc));
			i = 1 + strlen(s) + strlen(t);
			}
		else	
			{
			printf("%s",s);
			i = strlen(s);
			}
		for(;i<maxa;++i) printf(" ");
		printf(" : %s",ta->desc);
		if(ha && (s=(*ta->proc)(GIVE_DEFAULT,ta->argptr,ta->argdesc))) 
			printf(" (def=\"%s\")",s);
		printf("\n");
		}
	printf("\n%s\n",IDstring);
	exit(0);
	}

/*
 * parse command line
 */
void parse_args(int ac, char **av)
	{
	int i;
	args *ta;

	for(i=1;i<ac;++i)
		{
		for(ta=targs;ta->arg;++ta)
			{
			char *s=ta->arg;
			char *t=av[i];
			bool hasa;

			if(hasa=HAS_ARG(s)) ++s;
			while(*s && *s==*t) {++s;++t;}
			if(*s) continue;	/* not found */
			
			/* get arg if needed */
			if(hasa && !*t) 
				{
				t=av[++i];
				if(i>=ac)
					{
					printf("%s: Missing argument after %s !\n",
					ProgName,ta->arg+1); /* +1 because of '*' */
					usage();
					}
				}
		
			/* call function */
			if((*ta->proc)(t,ta->argptr,ta->argdesc)) wrong_arg(t);
			break;
			}
			
		ifn(ta->arg) break;
		}

	if(ac-i>1) duplicate_AC_AV(ac-i,av+i);
	}

/*
 * parse the environment variable
 */
void parse_env(char *envar)
	{
	char *arg,*s,*t;
	char **av;
	int ac,c;

	ifn(s = getenv(envar)) return;
	ifn(arg = malloc(strlen(s)+2)) return;
	strcpy(arg,s);

	/*
	 * build an (ac,av) couple from the envvar.
	 */

	ac = 1;
	for (t=s=arg; *s ; ++ac) 
		{char c;
		while ((c = *s) && isblank(c)) ++s;

		/*
		 * If in quote mode, just copy everything up to the ending quote.
		 * \" == " and \' == '.
		 */

		if ( c == '"' || c == '\'' ) 
			{char delim;
			delim = c;++s;
			while ( *s ) 
				{
				/* escape */
				while ( *s == '\\' && s[1] == delim ) 
					{
					*t++ = delim;
					s += 2;
					}
				if ( *s == delim ) 
					{
					++s;
					break;
					}
				while ((c=*s++) && c != delim) *t++ = c;
				}
			} 
		else	while ((c = *s++) && !isblank(c)) *t++ = c;
		*t++ = '\0';
		}

	*t = '\0';
	ifn ( av = malloc((ac+1)*sizeof(*av))) 
		{
		free(arg);
		return;
		}

	s = arg;
	for (c=1; c<ac; ++c) 
		{
		av[c] = s;
		s += strlen(s) + 1;
		}
	av[c] = NULL;

	parse_args(ac,av);
	}
