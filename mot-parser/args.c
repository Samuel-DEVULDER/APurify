/*
 *	args.c
 *
 * (c) by Samuel DEVULDER, 1995
 */

#include "parse.h"

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

/* if prefixed with '*', option needs an arg */
#define	HAS_ARG(s)	(*s=='*' && s[1])

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
	ifn(strcasecmp(n,"1"))		(*val)=TRUE;
	else ifn(strcasecmp(n,"ON"))	(*val)=TRUE;
	else ifn(strcasecmp(n,"0"))  	(*val)=FALSE;
	else ifn(strcasecmp(n,"OFF"))	(*val)=FALSE;
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
FTYPE set_basereg(char *t)	
	{
	if(t==GIVE_ARG)     return "An";
	if(t==GIVE_DEFAULT) return basereg;
	if(strlen(t)>2 || tolower(t[0])!='a' || t[1]<'0' || t[1]>'6')
		{
		printf("%s: bad base register : %s !\n",ProgName,t);
		usage();
		}
	basereg[0]='A';
	basereg[1]=t[1];
	basereg[2]=t[2];
	return NULL;
	}
PRIVATE	bool def_action(char *n)
	{
	if(source_name)
		{
		printf("%s: source file already set to \"%s\". What is \"%s\" ?\n",
			ProgName,source_name,n);
		return FALSE;
		}
	else	source_name = n;
	return TRUE;
	}

PRIVATE args targs[]=
{
{"?",	"To display this usage",		(void*)usage},
{"*-o", "specifies output file",		set_STR, &dest_name, "file"},
{"-tb",	"enable base register testing",		set_ON,  &test_basereg},
{"-ts",	"enable stack register testing",	set_ON,  &test_stack},
{"-tl",	"enable local stack frame testing",	set_ON,  &test_lstack},
{"-tp", "to test pea instruction",		set_ON,  &test_pea},
{"-sd", "use small data model",			set_ON,  &use_A4},
{"-sc", "use small code model",			set_ON,  &use_PC},
{"*-br","sets the base register",		set_basereg},
{"*-mp","sets the main label",			set_STR, &proc_main, "main label"},
{"*-sl","to use source level informations",     set_STR, &source_is,"source name"},
{"-sas","use sas/c compatibility mode",		set_ON,  &sas},

{NULL}	/* sentinel */
};

/*
 * generate automatically the usage according to the array
 */
void usage()
	{
	int maxa,hasa;
	args *ta;
	printf("\nUsage: %s [-revinfo] [\033[3moptions\033[23m] <inputfile>\n",ProgName);
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
	printf("\n%s\n",&IDstring[0]);
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
			
		if(!ta->arg && (av[i][0]=='-' || !def_action(av[i])))
			{
			printf("%s: Illegal argument: %s !\n",ProgName,av[i]);
			usage();
			}
		}

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
				while ((c=*s) && c != delim) {++s;*t++ = c;}
				}
			} 
		else if(c) 
			{
			while ((c = *s) && !isblank(c)) {++s;*t++ = c;}
			}
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
