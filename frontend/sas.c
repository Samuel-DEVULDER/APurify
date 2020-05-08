/*
 * sas.c - sas/c part of apurify frontend.
 *
 * (c) by Samuel DEVULDER, 1996
 */
#include "apf.h"

PRIVATE char *objdir;

/*
 * determine if sas arg is equal. Eventually split it when needed
 *
 * if pattern ends with '=' that means a second argument is needed.
 */
PRIVATE int sas_arg(int ac,char *pattern)
	{
	char *a=AV[ac];
	while(*a && *pattern && *pattern!='=')
		{
		if(tolower(*a)==tolower(*pattern)) 
			{++a;++pattern;continue;}
		else if(islower(*pattern)) 
			{++pattern;continue;}
		return 0;
		}
	while(*pattern && *pattern!='=' && islower(*pattern)) ++pattern;
	ifn(*a)
		{
		if(!*pattern || *pattern=='=') return 1;
		return 0;
		}
	if(*a!='=') return 0;
	/* split */
	split_arg(ac,a+1);
	*a='\0'; /* cleanup the '=' */
	return 1;
	}

/*
 * check if argument is a sourcefile
 */
PRIVATE void sas_file(int ac)
	{
	char *av  = AV[ac];
	char *ext = extension(av);

	if     (!stricmp(ext,"c") || 
		!stricmp(ext,"h") || 
	   	!stricmp(ext,"p"))	{PhaseFrom = PHASE_C;set_source(ac);}
	else if(!stricmp(ext,"a") || 
		!stricmp(ext,"asm") || 
		!stricmp(ext,"s"))	{PhaseFrom = PHASE_A;set_source(ac);}
	else if(!stricmp(ext,"o"))
		{if(PhaseFrom==PHASE_UNK ||
		    PhaseFrom>PHASE_O) 	{PhaseFrom = PHASE_O;set_source(ac);}}
	else if(access_c(av))	
		{ /* add .c to the source */
		char *t;
		PhaseFrom = PHASE_C;
		t = s_malloc(strlen(av)+3);
		strcpy(t,av);
		strcat(t,".c");
		free(av);
		AV[ac]=t;
		set_source(ac);
		}
	/* else it is an unknow option */
	}

/*
 * parse command line. Determine the 'from' & the 'to'
 */
void sas_parse(void)
	{
	int ac;
	bool seen_dbg = FALSE;

	PhaseFrom = (!stricmp(basename(AV[0]),"slink"))?PHASE_O:PHASE_UNK;
	SourceIdx = 0;

	PhaseTo = PHASE_O; /* default dest will be object file */
	DestIdx = 0;
	LinkIdx = 0;
	objdir  = NULL;

	for(ac=1;ac<AC;++ac)
		{
		if     (sas_arg(ac,"ASseMbler=")) 
			{	
			PhaseFrom = PHASE_A;
			set_source(++ac);
			}		
		else if(sas_arg(ac,"CSouRCe="))
			{
			PhaseFrom = PHASE_C;
			set_source(++ac);
			}
		else if(sas_arg(ac,"DISASseMble=")) 
			{
			PhaseTo = PHASE_A;
			set_dest(++ac);
			}
/*		else if(sas_arg(ac,"FROM"))	{}	*//* ignored */
		else if(sas_arg(ac,"LINK"))
			{
			PhaseTo = PHASE_E;
			LinkIdx = ac;
			}
		else if(sas_arg(ac,"NOLINK"))
			{
			PhaseTo = PHASE_O;
			LinkIdx = ac;
			}
		else if(sas_arg(ac,"OBJect="))
			{
			if(PhaseFrom>PHASE_O)
				{
				PhaseFrom = PHASE_O; 
				set_source(++ac);
				}
			}
		else if(sas_arg(ac,"OBJectNAME=")) 
			{
			char *s,*t;
			s = AV[++ac];t=s+strlen(s)-1;
			if(t[0]==':' || t[0]=='/')
				{
				objdir = s_strdup(s);
				}
			else	set_dest(ac);
			}
		else if(sas_arg(ac,"ProgramNAME="))  
			{
			DestIdx = 0; /* reset dest */
			PhaseTo = PHASE_E;
			set_dest(++ac);
			}
/* sourceis ?*/
		else if(sas_arg(ac,"TO="))
			{
			set_dest(++ac);
			}
		else if(sas_arg(ac,"WITH="))
			{
			PhaseFrom = PHASE_O;
			set_source(++ac); /* indeed a fake source :-) */
			}
		else if(sas_arg(ac,"DeBuG="))
			{
			seen_dbg = TRUE;
			}
		else sas_file(ac);
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
		
		t    = noextension(AV[SourceIdx]);
		dest = s_malloc(strlen(t)+(PhaseTo==PHASE_O?3:1));
		strcpy(dest,t);free(t);
		if(PhaseTo==PHASE_O) strcat(dest,".o");

		add_arg(PhaseTo==PHASE_O?"OBJNAME":
			PhaseTo==PHASE_E?"TO":
			"INTERNAL_ERROR");
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
	if(!seen_dbg && source_level) add_arg("DBG=FF");
	}

/*
 * create the right object name
 */
PRIVATE char *make_obj(char *name)
	{
	char *s,*t;
	if(objdir)
		{
		t = noextension(basename(name));
		s = s_malloc(3+strlen(objdir)+strlen(t));
		strcpy(s,objdir);strcat(s,t);strcat(s,".o");
		free(t);
		return s;
		}
	else	{
		t = noextension(name);
		s = s_malloc(3+strlen(t));
		strcpy(s,t);strcat(s,".o");
		free(t);
		return s;
		}
	}

/*
 * generate the name of the asm file
 */
PRIVATE char *tmp_asm_name(char *source)
	{
	char *s,*t;

/*	t = noextension(basename(source));
	s = s_malloc(strlen(t)+9); 
	strcpy(s,"t:");
	strcat(s,t); free(t);
	strcat(s,"-AP.a");
*/	
	t = noextension(source);
	s = s_malloc(strlen(t)+9); 
	strcpy(s,t); free(t);
	strcat(s,"-AP.a");

	return s;
	}

/*
 * compile with sas
 */
void sas_compile(void)
	{
	char *source,*dest;
	enum phase Phase;
	bool tmp_source;
	
	tmp_source = 0;
	Phase      = PhaseFrom;
	source     = AV[SourceIdx];
	
	if(Phase == PhaseTo) return;

	/* .c -> .a */
	if(Phase == PHASE_C)
		{
#if 0 /* broken */
		/* generating the .a file looses the .c file for debugging */
		/* but here is a hack :-) */
		add_arg("SRCIS");add_arg(source);
#endif
		add_arg("DEF=APURIFY");
		/* sasbug #5: */
		add_arg("DEF=__regargs");add_arg("PARM=STACK");
		/* sasbug #9: */
		add_arg("NOSTRMER");
		
		if(PhaseTo == PHASE_A) 	dest = AV[DestIdx];
		else	{
			tmp_source = 1;
			dest = tmp_asm_name(source); 
			}
			
		process(source,"DISASM",dest,"NOLINK");
		
		sub_arg();
		sub_arg();sub_arg();
		sub_arg();
		
		{ /* sas/c is leaving a .o file, we delete it */
		char *s=make_obj(source);
		Unlink(s);
		free(s);
		}

		source = dest;
		Phase  = PHASE_A;
		}

	if(Phase == PhaseTo) return;

	/* .s -> .o */
	if(Phase == PHASE_A)
		{
		if(tmp_source) apurify(mot_parser,"-sas",source,source);
		else	{
			dest = tmp_asm_name(source); 
			apurify(mot_parser,"-sas",source,dest);
			source = dest;
			}

		if(PhaseTo == PHASE_O)
			{
			char *t,*s;

			add_arg("USCORE");
			process(source,"","","NOLINK");
			sub_arg();

			s = make_obj(source);
			t = make_obj(AV[DestIdx]);
			if(strcmp(s,t))
				{
				copy(s,t);
				Unlink(s);
				}
			free(s);
			free(t);
			}
		else /* PhaseTo == PHASE_E */ 	
			{
			char *s;

			add_arg("USCORE");add_arg(saslib);
			process(source,NULL,NULL,"LINK");
			sub_arg();sub_arg();

			/* 
			 * I decide to delete the temporary obj file
			 */
			s = make_obj(source);
			Unlink(s);free(s);
			}
		/*
		 * the source is always temporary: we delete it
		 */
		Unlink(source);
		return;
		}
	/* .o -> .exe */
	if(Phase == PHASE_O)
		{
		add_arg(saslib);
		process(source,NULL,NULL,"LINK");
		sub_arg();
		}
	}
