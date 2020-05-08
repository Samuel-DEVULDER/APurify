/*
 *	copy.c
 *
 * (c) by Samuel DEVULDER, 1995
 */

#include "parse.h"

/*
 * a kind of cp...
 */
void copy(src,dst)
	char *src;
	char *dst;
	{
	FILE *f,*g;
	if(f=fopen(src,"r"))
		{
		if(g=fopen(dst,"w"))
			{
			while(fgets(line,LSIZE-1,f)) fputs(line,g);
			fclose(g);
			}
		else	{fprintf(stderr,"%s - ",ProgName);perror(dst);}
		fclose(f);
		}
	else	{fprintf(stderr,"%s - ",ProgName);perror(src);}
	}
