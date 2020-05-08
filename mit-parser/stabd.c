/*
 * stabd.c 
 *
 * (c) by Samuel DEVULDER, 1995
 */

#include "parse.h"

PRIVATE unsigned short lineno;

/*
 * reset lineno
 */
void reset_stabd(void)
	{
	lineno = 0;
	}

/*
 * dump the line no if needed
 */
void dump_lineno(void)
	{
	if(lineno)
		{
		fprintf(ofp,"\tmovew #%d,_AP_lineno\n",lineno);
		lineno = 0;
		}
	}

/*
 * setup line number according to .stabd directive
 */
bool stabd_chk(void)
	{
	if(strcasecmp(opcode,".stabd")) return FALSE;
	fputs(line,ofp);
	lineno = atoi(arg3);
	return TRUE;
	}
