/*
 *	tokenize.c
 *
 * (c) by Samuel DEVULDER, 1995.
 */

#include "parse.h"

/*
 * Cut line into tokens.
 */
void tokenize()
	{
	char *s=line,*t;
	int p;
	
	/*
	 * grab label
	 */
	t=label;   
	while(*s && !isblank(*s)) *t++=*s++;
	*t='\0';
	while(isblank(*s)) ++s;

	/*
	 * grab opcode
	 */
	t=opcode;
	while(*s && !isblank(*s)) *t++=*s++;
	*t='\0';
	while(isblank(*s)) ++s;

	/*
	 * grab arg1
	 */
	if(sas) {sasbug4(s);sasbug5(s);sasbug8(s);sasbug9(s);}
	t=arg1;p=0;
	while(*s && !(separator(*s) && !p)) 
		{
		switch(*s) 
			{
			case '[': case '(': ++p; break;
			case ']': case ')': --p; break;
			default: break;
			}
		*t++=*s++;
		
		if(sas && (sasbug1(s) || 
			   sasbug3(s))) break;
		}
	while(t>arg1 && isblank(t[-1])) --t;
	*t='\0';
	while(isblank(*s)) ++s;

	/*
	 * grab arg2
	 */
	if(sas) {sasbug5(s);sasbug9(s);}
	t=arg2;p=0;
	if(*s==',')
		{
		if(sas && sasbug2(s)) goto cont1; /* is sas really a good
						     compiler ?? */
		++s;
		if(sas) sasbug5(s);
		while(*s && !(separator(*s) && !p)) 
			{
			switch(*s) 
				{
				case '[': case '(': ++p; break;
				case ']': case ')': --p; break;
				default: break;
				}
			*t++=*s++;

			if(sas && sasbug1(s)) break;
			}
		}
cont1:	while(t>arg2 && isblank(t[-1])) --t;
	*t='\0';
	if(sas) sasbug7();
	}
