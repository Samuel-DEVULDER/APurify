/*
 *	jbsr.c
 *
 * (c) by Samuel DEVULDER, 1995.
 */

#include "parse.h"

/*
 * Check if current instruction is a call to a sub-routine. If
 * it is one, it outputs special code for that.
 */
bool jbsr_chk()
	{
	if(!strcasecmp(opcode,"jsr")  ||
	   !strcasecmp(opcode,"bsr")  ||
	   !strcasecmp(opcode,"bsr.s") ||
	   !strcasecmp(opcode,"bsr.b") ||
	   !strcasecmp(opcode,"bsr.w") ||
	   !strcasecmp(opcode,"bsr.l")
	  )	{
	  	if(source_is) dump_lineno();
		fprintf(ofp,"\tpea\t%s\n",arg1);
		put_jsr("AP_FuncCall");
		return TRUE;
		}
	return FALSE;
	}

/*
 * put jsr _LABEL or bsr _LABEL if we can use PC Relative mode
 */
void put_jsr(char *label)
	{
	fprintf(ofp,"\t%s\t%s\n",(use_PC?"bsr":"jsr"),label);
	}
