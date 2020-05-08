/*
 *	xref.c
 *
 * (c) by Samuel DEVULDER, 1995
 */

#include "parse.h"

/*
 * check if we are using xref or public
 */
void xref_chk()
	{
	if(use_xref || use_public) return;
	if(!strcasecmp(opcode,"xref"))   use_xref   = TRUE;
	if(!strcasecmp(opcode,"public")) use_public = TRUE;
	}

PRIVATE void aux(char *s)
	{
	fprintf(ofp,"\t%s\t%s\n",(use_public?"public":"xref"),s);
	}

/*
 * put xref statements
 */
void put_xref()
	{
	aux(source_is?"AP_FuncStartup2":"AP_FuncStartup");
	if(source_is) aux(sas?"AP_lineno":"_AP_lineno");
	aux("AP_FuncCall");
	aux("AP_Read");
	aux("AP_ReadB");
	aux("AP_ReadW");
	aux("AP_ReadL");
	aux("AP_Write");
	aux("AP_WriteB");
	aux("AP_WriteW");
	aux("AP_WriteL");
	if(xref_AP_Init) aux(sas?"AP_Init":"_AP_Init");
	}
