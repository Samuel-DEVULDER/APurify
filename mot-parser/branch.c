/*
 * branch.c
 *
 * (c) by Samuel DEVULDER, 1995
 */
#include "parse.h"

/*
 * We must transform bcc.b as bcc
 */
bool chk_branch()
	{
	char bcc[5];
	char *s,*t;
	int l;

	s=bcc;
	t=opcode;
	*s++=tolower(*t);++t;
	*s++=tolower(*t);++t;
	*s++=tolower(*t);++t;
	if(*t!='.') {*s++=tolower(*t);++t;}
	*s='\0';
	l=s-bcc;
	if(*t++!='.') return FALSE;
	if(tolower(*t)!='b' && tolower(*t)!='s') return FALSE;
	if(strcmp(bcc,"bcc") &&
	   strcmp(bcc,"bcs") &&
	   strcmp(bcc,"beq") &&
	   strcmp(bcc,"bge") &&
	   strcmp(bcc,"bgt") &&
	   strcmp(bcc,"bhi") &&
	   strcmp(bcc,"ble") &&
	   strcmp(bcc,"bls") &&
	   strcmp(bcc,"blt") &&
	   strcmp(bcc,"bmi") &&
	   strcmp(bcc,"bne") &&
	   strcmp(bcc,"bpl") &&
	   strcmp(bcc,"bra") &&
	   strcmp(bcc,"bvc") &&
	   strcmp(bcc,"bvs") &&
	   strcmp(bcc,"dbr") &&
	   strcmp(bcc,"dbcc") &&
	   strcmp(bcc,"dbcs") &&
	   strcmp(bcc,"dbeq") &&
	   strcmp(bcc,"dbge") &&
	   strcmp(bcc,"dbgt") &&
	   strcmp(bcc,"dbhi") &&
	   strcmp(bcc,"dble") &&
	   strcmp(bcc,"dbls") &&
	   strcmp(bcc,"dblt") &&
	   strcmp(bcc,"dbmi") &&
	   strcmp(bcc,"dbne") &&
	   strcmp(bcc,"dbpl") &&
	   strcmp(bcc,"dbra") &&
	   strcmp(bcc,"dbvc") &&
	   strcmp(bcc,"dbvs")) return FALSE;

	/* cleanup .b for opcode */
	opcode[l]='\0';

	/* and for the line */
	for(s=line;strncasecmp(s,bcc,l);++s);
	s+=l;
	s[0]=s[1]=' ';

	/* evenutally correct second arg for sas */
	if(sas && bcc[0]=='d' && bcc[1]=='b') sasbug6_last();

	return TRUE;
	}
