/*
 *	label.c
 *
 * (c) by Samuel DEVULDER, 1995.
 */

#include "parse.h"

PRIVATE bool source_is_done = FALSE;

/*
 * Check if next instruction is something defining a data or a real
 * function. (very bad code indeed :-( )
 */
PRIVATE bool next_inst_code(void)
	{
	int currpos = ftell(ifp);
	bool ret = TRUE;
	char line[256],*s,*opcode;

	while(fgets(line,255,ifp))
		{
		s=line;while(isblank(*s)) ++s; ifn(*s) continue;
		if(*s=='*' || *s=='#' || *s==';') continue; /* comment */
		opcode = s;
		while(*s && !isblank(*s)) ++s;
		*s='\0';
		if(!strcasecmp(opcode,".stabs")) continue; /* dummy thing */
		if(!strcasecmp(opcode,".ascii") ||
		   !strcasecmp(opcode,".double") ||
		   !strcasecmp(opcode,".long") ||
		   !strcasecmp(opcode,".word") ||
		   !strcasecmp(opcode,".byte") ||
		   !strcasecmp(opcode,".skip") ||
		   !strcasecmp(opcode,".comm")
		   ) {ret=FALSE;break;}
		else break;
		}	
	fseek(ifp,currpos,0);
	return ret;
	}

/*
 * replace strange chars by '_'
 */
PRIVATE void safeprint(FILE *f,char *s)
	{
	char c;
	while(c=*s++)
		{
		ifn(isalpha(c)) c='_';
		fputc(c,f);
		}
	}

/*
 * Put special code in case where the line bears a label that
 * is a start of sub-routine. (We assume that such labels begins
 * with a '_' and ends with a ':').
 */
void label_chk()
	{
	char *s=label;
	
	/* 
	 * check if the label is good
	 */
	ifn(*s) return;
	reset_stabd();
	if(*s!='_') return;
	while(s[1]) ++s;
	if(*s!=':') return;
	*s='\0';
	
	ifn(next_inst_code()) return;

	/*
	 * remove it from the current line
	 */
	for(s=line;*s && !isblank(*s);*s++=' ');

	/*
	 * delete the line if it just contains the label
	 */
	if(*s=='\n') *line='\0';
	else ifn(*s) *line='\0';

	/*
	 * put AP_stuff
	 */
	fprintf(ofp,"AP%s:\n",label);
	fprintf(ofp,"\t.ascii \"%s\\0\"\n",label);
	if(!source_is_done && source_is)
		{
		fprintf(ofp,"AP");safeprint(ofp,source_is);fprintf(ofp,":\n");
		fprintf(ofp,"\t.ascii \"%s\\0\"\n",source_is);
		source_is_done = TRUE;
		}
	fprintf(ofp,"\t.even\n");
	ifn(source_is)
		{
		fprintf(ofp,"%s:\n\tpea AP%s\n",label,label);
		fprintf(ofp,"\tjbsr AP_FuncStartup\n");
		}
	else	{
		fprintf(ofp,
		"%s:\n\tpea AP",label);safeprint(ofp,source_is);fprintf(ofp,"\n");
		fprintf(ofp,"\tpea AP%s\n",label);
		dump_lineno();
		fprintf(ofp,"\tjbsr AP_FuncStartup2\n");
		}
	/*
	 * if the label is _main, put AP_Init automatically
	 */
	ifn(strcmp(label,proc_main))
		{
		fprintf(ofp,"\tjbsr _AP_Init\n");
		}
	}
