/*
 *	lstack.c
 *
 * (c) by Samuel DEVULDER, 1995.
 */

#include "parse.h"

/*
 * Detects which register is used for local stack frames and updates
 * lstackreg[] accordingly.
 */
void lstack_chk()
	{
	char *s=lstackreg,*t=arg1;
	/*
	 * set on link
	 */
	if(!strncasecmp(opcode,"link",4))
		{
		*s++=*t++;
		*s++=*t;
		*s='\0';
		return;
		}
	/*
	 * reset on new func-label
	 */
	if(isfunclbl(label)) *s=0;
	}
