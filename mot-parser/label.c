/*
 *	label.c
 *
 * (c) by Samuel DEVULDER, 1995.
 */

#include "parse.h"

struct stack 
	{
	struct stack *next;
	char *name;
	};

PRIVATE struct stack *stk = NULL;

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
		if(s-2>opcode && s[-2]=='.') s-=2;
		*s='\0';
		if(!strcasecmp(opcode,"dc") ||
		   !strcasecmp(opcode,"ds")) {ret=FALSE;break;}
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
 * Detects if argument is a funclabel. returns the label or NULL.
 * (We assume that such a label begins with a '_' and 
 * can ends with a ':').
 */
char *isfunclbl(char *lbl)
	{
   	static char labstr[80];
	char *s=lbl,*t;
	if(!*s || *s!='_') return NULL;
	/*
	 * for sas, we assume that ___XXXX__1: is the func label
	 */
	if(sas)
		{
	   	char *u=labstr;
		t=s+strlen(s)-1;
		ifn(s[0]=='_' && s[1]=='_' && s[2]=='_') return NULL;

		if(t[0]!=':') return NULL;
		do --t; while(*t>='0' && *t<='9');
		if(*t--!='_') return NULL;
		if(*t!='_') return NULL;
		if(strcmp(t,"__1:")) return NULL;
	   	s+=2;
	   	while(s<t) *u++=*s++;
	   	*u='\0';
	   	return labstr;
	   	}
	t=labstr;
	while(*t++=*s++);t-=2;
	if(*t==':') *t='\0'; /* strip off the ending colon */
	return next_inst_code()?labstr:NULL;
	}

/*
 * push a new func name onto the stack.
 */
PRIVATE void push(char *name)
	{
	struct stack *s;
	ifn(s = malloc(sizeof(struct stack)))
		{
		char s[32];
		sprintf(s,"Out of mem (%d) !",sizeof(struct stack));
		my_perror(s);
		exit(10);
		}
	s->next = stk;
	ifn(s->name = strdup(name))
		{
		my_perror("strdup");
		exit(10);
		}
	stk = s;
	}

/*
 * pop a func name.
 */
PRIVATE char *pop(void)
	{
	char *n;
	struct stack *s;
	
	ifn(stk) return NULL;
	n=stk->name;
	s = stk->next;
	free((void*)stk);
	stk = s;
	return n;
	}

/*
 * Output code for the labels
 */
void put_label(void)
	{
	char *label,*s;
	if(use_cseg)	fprintf(ofp,"\tdseg\n");
	else		fprintf(ofp,"\tsection\tAPurify,data\n");
	if(source_is)
		{
		fprintf(ofp,"AP");safeprint(ofp,source_is);fprintf(ofp,":\n");
		fprintf(ofp,"\tdc.b\t");
		s=source_is;
		while(*s) fprintf(ofp,"%d,",*s++);
		fprintf(ofp,"0\n");
		}
	while(label = pop())
		{
		fprintf(ofp,"AP%s:\n",label);
		fprintf(ofp,"\tdc.b\t");
		s=label;
		while(*s) fprintf(ofp,"%d,",*s++);
		fprintf(ofp,"0\n");
		free((void*)label);
		}
	}

/*
 * Put special code in case where the line bears a label that
 * is a start of sub-routine. 
 */
void label_chk()
	{
	char *s=label;
	char *labstr;
	
	/* 
	 * check if the label is good
	 */
	ifn(*s) return;
	ifn(labstr=isfunclbl(s)) return;

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
/*	reset_lineno(); pas utile... */
	push(labstr);
	ifn(source_is)
		{
		ifn(use_A4) fprintf(ofp,"%s\tpea\tAP%s\n",label,labstr);
		else fprintf(ofp,"%s\tpea\tAP%s(%s)\n",label,labstr,basereg);
		put_jsr("AP_FuncStartup");
		}
	else	{
		ifn(use_A4)
			{
			fprintf(ofp,"%s\n\tpea\tAP",label);
			safeprint(ofp,source_is);fprintf(ofp,"\n");
			fprintf(ofp,"\tpea\tAP%s\n",labstr);
			}
		else	{
			fprintf(ofp,"%s\n\tpea\tAP",label);
			safeprint(ofp,source_is);fprintf(ofp,"(%s)\n",basereg);
			fprintf(ofp,"\tpea\tAP%s(%s)\n",labstr,basereg);
			}
		dump_lineno();
		put_jsr("AP_FuncStartup2");
		}
	/*
	 * if the label is _main, put AP_Init automatically
	 */
	ifn(strcmp(labstr,proc_main)) 
		{
		put_jsr(sas?"AP_Init":"_AP_Init");
		xref_AP_Init = TRUE;
		}
	}
