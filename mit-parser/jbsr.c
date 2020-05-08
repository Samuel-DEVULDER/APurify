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
	if(!strcasecmp(opcode,"jbsr") ||
	   !strcasecmp(opcode,"jsr")  ||
	   !strcasecmp(opcode,"bsr")  ||
	   !strcasecmp(opcode,"bsrs") ||
	   !strcasecmp(opcode,"bsrb") ||
	   !strcasecmp(opcode,"bsrw") ||
	   !strcasecmp(opcode,"bsrl")
	  )	{
		if(source_is) dump_lineno();
		fprintf(ofp,"\tpea %s\n",arg1);
		fprintf(ofp,"\tjbsr AP_FuncCall\n");
		return TRUE;
		}
	return FALSE;
	}
