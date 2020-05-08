/*
 *	iscomment.c
 *
 * (c) by Samuel DEVULDER, 1995.
 */
#include "parse.h"

/*
 * We say that a line that begins with '#' '*' or ';' is a 
 * comment line.
 */
bool iscomment()
	{
	char *s=line;

	while(isblank(*s)) ++s;
	return *s=='*' || *s=='#' || *s==';';
	}
