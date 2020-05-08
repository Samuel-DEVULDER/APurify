/*
 *	sasbugs.c
 *
 * (c) by Samuel DEVULDER, 1995.
 */

#include "parse.h"

/* sas has a bug. Sometimes it generates lines
 * like:
 *        BEQ.W __long_name; 4456 78AB
 *
 * which he is unable to re-assemble.
 * Then we just add a space before the ';'.
 */
bool sasbug1(char *s)
	{
	if(*s==';')
		{
		char *q=s;
		while(*q++);
		while(s<q) {q[0]=q[-1];--q;}
		*q=' ';
		return TRUE;
		}
	return FALSE;
	}

/* sas has an other bug. Sometimes it generates lines
 * like:
 *        SCC	D0,#$700
 *
 * which he is unable to re-assemble.
 * Then we just clean the second argument
 */
bool sasbug2(char *s)
	{
	if(!strcasecmp(opcode,"SCC") ||
	   !strcasecmp(opcode,"SCS") ||
	   !strcasecmp(opcode,"SEQ") ||
	   !strcasecmp(opcode,"SGE") ||
	   !strcasecmp(opcode,"SGT") ||
	   !strcasecmp(opcode,"SHI") ||
	   !strcasecmp(opcode,"SLE") ||
	   !strcasecmp(opcode,"SLS") ||
	   !strcasecmp(opcode,"SLT") ||
	   !strcasecmp(opcode,"SMI") ||
	   !strcasecmp(opcode,"SNE") ||
	   !strcasecmp(opcode,"SPL") ||
	   !strcasecmp(opcode,"SVC") ||
	   !strcasecmp(opcode,"SVS") ||
	   !strcasecmp(opcode,"ST"))
		{
		while(*s && !isblank(*s)) *s++=' ';
		return TRUE;
		}
	return FALSE;
	}

/* sas has an other bug. Sometimes it generates lines
 * like:
 *        DC.L	__LBL+1d
 *
 * which he is unable to re-assemble (one $ is missing).
 * then we add the missing $.
 */
bool sasbug3(char *s)
	{
	if(!strcasecmp(opcode,"DC.L") &&
	   s[0]=='+' && s[1]!='$')
		{
		char *q=++s;
		while(*q++);
		while(s<q) {q[0]=q[-1];--q;}
		*q='$';
		return TRUE;
		}
	return FALSE;
	}

/* sas has an other bug. Sometimes it generates lines
 * like:
 *        MOVEQ.l #$fe,D0
 *
 * which he is unable to re-assemble.
 * Then we just expand the first argument to $fffffffe
 */
void sasbug4(char *arg1)
	{
	char *s,c;
	if(strcasecmp(opcode,"MOVEQ.L")) return;
	ifn(arg1[0]=='#' && arg1[1]=='$' && 
	    arg1[2]>='8' && arg1[2]<='f' &&
	    arg1[4]==',') return;
	arg1+=2;	
	s=arg1;while(*s) ++s;
	while(s>=arg1) {s[6]=s[0];--s;} ++s;
	c='f';*s++=c;*s++=c;*s++=c;*s++=c;*s++=c;*s=c;
	}

/* sas has an other bug. Sometimes it generates lines
 * like:
 *        MOVEM.L	A1/A2/A3/A4/A5/A6/D1/D2/D3/D4/D5,-(SP)
 *
 * which he is unable to re-assemble (too long argument).
 * then we pack the argument
 */
#define isreg(s)	((tolower(*s)=='a' || tolower(*s)=='d') && \
			 s[1]>='0' && s[1]<='7' && (!s[2] || s[2]=='/' || s[2]=='-' || s[2]==','))
bool sasbug5(char *arg)
	{
	if(!strncasecmp(opcode,"MOVEM",5) && isreg(arg) && arg[2])
		{
		char *s;
		for(s=arg+3;*s && isreg(s);s+=3)
			{
			if(tolower(s[-3])==tolower(s[0]) && (s[-2]+1)==s[1]) s[-1]='-';
			}
		for(s=arg+3;*s && isreg(s);)
			{
			if(s[-1]=='-' && s[2]=='-')
				{
				char *t;
				for(t=s;t[0]=t[3];++t);
				}
			else s+=3;
			}
		return TRUE;
		}
	return FALSE;
	}

/* sas has an other bug. Sometimes it generates lines
 * like:
 *        DBR  D1,#$fffffffc             ;51c9 fffc 
 *
 * which he is unable to re-assemble. Then we try to
 * work around using pseudo-labels.
 */
#define ishex(a)	(((a)>='0' && (a)<='9') || \
			 ((a)>='a' && (a)<='f') || ((a)>='A' && (a)<='F'))
void sasbug6_first(char *line)
	{
	char *s;
	int cpt;

	ifn(s=line)
		{
		offset = 0;
		fprintf(ofp,"APsas%d:\n",offset);
		return;
		}
	while(*s) ++s; if(s[-1]=='\n') --s;
	cpt = 0;
	while(s>line && ishex(s[-5]) && ishex(s[-4]) && ishex(s[-3]) && ishex(s[-2]) &&
	                s[-1]==' ') {s -= 5;cpt += 2;}
	if(s>line && s[-1]==';' && cpt)
		{
		offset += cpt;
		fprintf(ofp,"APsas%d:\n",offset);
		return;
		}
	}
void sasbug6_last(void)
	{
	int delta;
	char *s=arg2,*t;

	if(!strcasecmp(opcode,"dbr") || !strcasecmp(opcode,"dbf"))
		{
		t=line;
		while(strncasecmp(t,opcode,3)) ++t;
		t[3]=t[0]=='d'?'a':'A';
		strcpy(opcode,"dbra");
		}

	ifn(s[0]=='#' || s[1]=='$') return;
	
	s+=2;
	delta=0;
	while(ishex(*s)) 
		{
		char c=tolower(*s);
		delta<<=4;
		if(c>='0' && c<='9') delta += c-'0';
		else delta += c-'a'+10;
		++s;
		}
	sprintf(arg2,"APsas%d",offset+delta+2);
	s=line;t=arg2;
	while(s[0]!='#' && s[1]!='$') ++s;
	while(*t) *s++=*t++;
	while(*s!=';') *s++=' ';
	}

/* 
 * fix x(PC) addressing using pseudo-labels
 */
void sasbug7(void)
	{
	int delta;
	char *s;
	static char buff[LSIZE];

	s = arg1;
	if(s[0]=='$')
		{
		++s;
		delta=0;
		while(ishex(*s)) 
			{
			char c=tolower(*s);
			delta<<=4;
			if(c>='0' && c<='9') delta += c-'0';
			else delta += c-'a'+10;
			++s;
			}
		if(!strcasecmp(s,"(PC)")) 
			{
			int n = strlen(arg1);		
			s = line;while(strncasecmp(s,arg1,n)) ++s;
			strcpy(buff,s+n);
			sprintf(arg1,"APsas%d",offset+delta+2);
			strcpy(s,arg1);
			strcat(s,buff);
			}
		}
	s = arg2;
	if(s[0]=='$')
		{
		++s;
		delta=0;
		while(ishex(*s)) 
			{
			char c=tolower(*s);
			delta<<=4;
			if(c>='0' && c<='9') delta += c-'0';
			else delta += c-'a'+10;
			++s;
			}
		if(!strcasecmp(s,"(PC)")) 
			{
			int n = strlen(arg1);		
			s = line;while(strncasecmp(s,arg2,n)) ++s;
			strcpy(buff,s+n);
			sprintf(arg2,"APsas%d",offset+delta+2);
			strcpy(s,arg2);
			strcat(s,buff);
			}
		}
	}
/* 
 * fix single quotes occuring in dc.b statements
 */
void sasbug8(char *s)
	{
	char *t;
	if(strcasecmp(opcode,"dc.b")) return;
	if(*s!='\'') return;
	++s;
	t=s;while(t[1]) ++t;while(t[0]!='\'') --t;
	while(s<t)
		{
		if(*s++=='\'')
			{
			int num = 5; /* ,39,' */
			char *u;

			/* shift */
			for(u=t;*u;++u);
			while(s<=u) {u[num]=u[0];--u;} t+=num;

			*s++ = ',';
			*s++ = '3';
			*s++ = '9';
			*s++ = ',';
			*s++ = '\'';
			}
		}
	}
/*
 * sometimes sas does this:
 *	PEA	(+$xxxx).l
 * or 	MOVE.L	#+$xxxx,yyyyy
 * instead of
 *	PEA	(__fardata+$xxxx).l
 */ 
void sasbug9(char *s)
	{
	if((s[0]=='(' || s[0]=='#') 
 	 && s[1]=='+' && s[2]=='$')
		{
		char *z="__fardata";
		int num = strlen(z);
		char *t;

		++s;
		/* shift */
		for(t=s;*t;++t);
		while(s<=t) {t[num]=t[0];--t;} 
		while(*z) *s++ = *z++;
		}
	}
