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
	if(!strcasecmp(opcode,"cseg"))
		{
		use_cseg     = TRUE;
		code_section = TRUE;
		return;
		}

	if(!strcasecmp(opcode,"dseg"))
		{
		use_cseg     = TRUE;
		code_section = FALSE;
		return;
		}

	if(!strcasecmp(opcode,"section"))
		{
		use_section  = TRUE;
	    	code_section = !strcasecmp(arg2,"code");
		return;
		}
	}
