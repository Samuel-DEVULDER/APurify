/*
 * gcc.c - gcc part of apurify frontend.
 *
 * (c) by Samuel DEVULDER, 1996
 */
#include "apf.h"

/*
 * determine if arg is equal to pattern. Eventually split it when needed
 *
 * if pattern ends with ' ' that means a second argument is needed.
 * but may be concatenated.
 */
PRIVATE int gcc_arg(int ac,char *pattern)
	{
	char *a=AV[ac];
	while(*a && *pattern!=' '  && *a==*pattern) {++a;++pattern;}
	if(*pattern==' ') /* extra arg needed */
		{
		ifn(*a) return 1;
		split_arg(ac,a);
		return 1;
		}
	ifn(*pattern) return !*a;
	return 0;
	}

/*
 * check if argument is a sourcefile
 */
PRIVATE void gcc_file(int ac)
	{
	char *ext = extension(AV[ac]);

	if     (!stricmp(ext,"c") || 
		!stricmp(ext,"h") || 
	   	!stricmp(ext,"p"))	{PhaseFrom = PHASE_C;set_source(ac);}
	else if(!stricmp(ext,"a") ||
		!stricmp(ext,"s") ||
		!stricmp(ext,"asm"))	{PhaseFrom = PHASE_A;set_source(ac);}
	else if(!stricmp(ext,"o"))
		{if(PhaseFrom==PHASE_UNK ||
		    PhaseFrom>PHASE_O) 	{PhaseFrom = PHASE_O;set_source(ac);}}
		/* the '>' is in case of mixing .o and .c: 
		   cc toto.c titi.o -o exec */
	/* else it is an unknow option */
	}

/*
 * parse command line. Determine the 'from' & the 'to'
 */
void gcc_parse(void)
	{
	int ac;
	bool seen_g = FALSE;

	PhaseFrom = PHASE_UNK;
	SourceIdx = 0;

	PhaseTo = PHASE_E; /* default dest will be object file */
	DestIdx = 0;
	LinkIdx = 0;

	for(ac=1;ac<AC;++ac)
		{
		if     (gcc_arg(ac,"-o ")) 
			{
			set_dest(++ac);
			}		
		else if(gcc_arg(ac,"-c"))
			{
			if(LinkIdx) goto errlink;
			LinkIdx = ac;
			PhaseTo = PHASE_O;
			}
		else if(gcc_arg(ac,"-g"))
			{
			seen_g = TRUE;
			}
		else if(gcc_arg(ac,"-S"))
			{
			if(LinkIdx)
				{
errlink:			printf("%d: Arg \"%s\" illegal (\"%s\" is used)\n",
					ProgName,AV[ac],AV[LinkIdx]);
				exit(5);
				}
			LinkIdx = ac;
			PhaseTo = PHASE_A;
			}
		else gcc_file(ac);
		}
	ifn(SourceIdx)
		{
		printf("%s: No source !\n",ProgName);
		exit(5);
		}
	ifn(DestIdx) /* create a dest entry */
		{
		char *dest;
		char *t;
		
		
		if(PhaseTo == PHASE_E) 
			{
			dest = s_strdup(t=noextension(AV[SourceIdx]));
			free(t);
			}
		else if(PhaseTo == PHASE_A)
			{
			t    = noextension(AV[SourceIdx]);
			dest = s_malloc(strlen(t)+3);
			strcpy(dest,t);free(t);
			strcat(dest,".a");
			}
		else if(PhaseTo == PHASE_O)
			{
			t    = noextension(AV[SourceIdx]);
			dest = s_malloc(strlen(t)+3);
			strcpy(dest,t);free(t);
			strcat(dest,".o");
			}
		add_arg("-o");
		add_arg(dest);
		free(dest);
		DestIdx = AC-1;
		}
	/* sanity check */
	if(PhaseFrom >= PhaseTo)
		{
		printf("%s: INTERNAL ERROR: 'From' is after 'To' (%d,%d) !\n",
			ProgName,PhaseFrom,PhaseTo);
		exit(20);
		}
	if(!seen_g && source_level) add_arg("-g");
	}

/*
 * generate the name of the asm file
 */
PRIVATE char *tmp_asm_name(char *source)
	{
	char *s,*t;

	t = noextension(basename(source));
	s = s_malloc(strlen(t)+9); 
	strcpy(s,"t:");
	strcat(s,t); free(t);
	strcat(s,"-AP.s");
	
	return s;
	}

/*
 * compile with gcc
 */
void gcc_compile(void)
	{
	char *source,*dest;
	enum phase Phase;
	bool tmp_source;
	
	tmp_source = 0;
	Phase      = PhaseFrom;
	source     = AV[SourceIdx];
	
	if(Phase == PhaseTo) return;

	/* .c -> .s */
	if(Phase == PHASE_C)
		{
		add_arg("-DAPURIFY");
		
		if(PhaseTo == PHASE_A) 	dest = AV[DestIdx];
		else	{
			tmp_source = 1;
			dest = tmp_asm_name(source);
			}
			
		process(source,"-o",dest,"-S");
		
		sub_arg();
		source = dest;
		Phase  = PHASE_A;
		}

	if(Phase == PhaseTo) return;

	/* .s -> .o or .exe */
	if(Phase == PHASE_A)
		{
		if(tmp_source) apurify(mit_parser,NULL,source,source);
		else	{
			dest = tmp_asm_name(source);
			apurify(mit_parser,NULL,source,dest);
			source     = dest;
			}
#if 1
		if(PhaseTo == PHASE_O) 	
			{
			process(source,NULL,NULL,"-c");
			Unlink(source);
			return;
			}
		else	{tmp_source = 1; Phase = PHASE_O;}
		}
#else /* old stuff */
		if(PhaseTo == PHASE_O) 	dest = AV[DestIdx];
		else	{
			char *t;
			t    = noextension(source);
			dest = s_malloc(strlen(t)+3); 
			strcpy(dest,t); free(t);
			strcat(dest,".o");
			tmp_source = 1;
			}

		process(source,"-o",dest,"-c");
		
		Unlink(source);
		source = dest;
		Phase  = PHASE_O;
		}

	if(Phase == PhaseTo) return;
#endif
	/* .o -> .exe */
	if(Phase == PHASE_O)
		{
		add_arg(gcclib);
		process(source,NULL,NULL,"");
		sub_arg();
		if(tmp_source) Unlink(source);
		}
	}
