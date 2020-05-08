/*
 *	code_section.c
 *
 * (c) by Samuel DEVULDER, 1995
 */

#include "parse.h"

/* 
 * detects if the next lines of asm will go in the code.
 */
void code_section_chk()
	{
	if(!strcasecmp(label,".text") ||
	   !strcasecmp(opcode,".text")) code_section=1;

	if(!strcasecmp(label,".data") ||
	   !strcasecmp(label,".data1") || 
	   !strcasecmp(opcode,".data") ||
	   !strcasecmp(opcode,".data1")) code_section=0;
	}
