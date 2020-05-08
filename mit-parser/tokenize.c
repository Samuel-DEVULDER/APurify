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
	t=arg1;p=0;
	while(*s && !(separator(*s) && !p)) 
		{
		switch(*s) 
			{
			case '(': ++p; break;
			case ')': --p; break;
			default: break;
			}
		*t++=*s++;
		}
	while(t>arg1 && isblank(t[-1])) --t;
	*t='\0';
	while(isblank(*s)) ++s;

	/*
	 * grab arg2
	 */
	t=arg2;p=0;
	if(*s==',')
		{
		++s;
		while(*s && !(separator(*s) && !p)) 
			{
			switch(*s) 
				{
				case '(': ++p; break;
				case ')': --p; break;
				default: break;
				}
			*t++=*s++;
			}
		}
	while(t>arg2 && isblank(t[-1])) --t;
	*t='\0';
	while(isblank(*s)) ++s;
	
	/*
	 * grab arg3
	 */
	t=arg3;p=0;
	if(*s==',')
		{
		++s;
		while(*s && !(separator(*s) && !p)) 
			{
			switch(*s) 
				{
				case '(': ++p; break;
				case ')': --p; break;
				default: break;
				}
			*t++=*s++;
			}
		}
	while(t>arg3 && isblank(t[-1])) --t;
	*t='\0';
/*	while(isblank(*s)) ++s;	*/
	}
