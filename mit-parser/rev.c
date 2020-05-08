#define cctimes 415
#define VNAME "MIT-APurify"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef VNUM
#define VNUM "0.0"
#endif

#ifdef __PDC__
static char _IDstring[1]={0};
static char IDstring1[]="$VER: MIT-APurify "VNUM" (04.07.99) [c: 415] © by Samuel Devulder.";
#else
static char _IDstring[]="\0$VER: MIT-APurify "VNUM" (04.07.99) [c: 415] © by Samuel Devulder.";
#endif
char *IDstring=&_IDstring[7];

static void _revinfo(char *name)
	{
	printf("%s\n\n",(char*)IDstring);
	printf("revinfo(\"%s\"): 415 compilations\n",name);
	puts("    mit-parser/gcc-objs/args.o\t\t   10103  Sun Jul 04 19:24:46 1999");
	puts("    mit-parser/gcc-objs/code_section.o\t    3461  Sun Jul 04 19:20:04 1999");
	puts("    mit-parser/gcc-objs/globals.o\t    3926  Sun Jul 04 19:23:03 1999");
	puts("    mit-parser/gcc-objs/inst.o\t\t   10037  Sun Jul 04 19:20:32 1999");
	puts("    mit-parser/gcc-objs/iscomment.o\t    3168  Sun Jul 04 19:23:27 1999");
	puts("    mit-parser/gcc-objs/jbsr.o\t\t    3679  Sun Jul 04 19:20:56 1999");
	puts("    mit-parser/gcc-objs/label.o\t\t    6085  Sun Jul 04 19:22:13 1999");
	puts("    mit-parser/gcc-objs/lstack.o\t    3513  Sun Jul 04 19:23:50 1999");
	puts("    mit-parser/gcc-objs/main.o\t\t    4086  Sun Jul 04 19:21:20 1999");
	puts("    mit-parser/gcc-objs/movem.o\t\t    5531  Sun Jul 04 19:22:39 1999");
	puts("    mit-parser/gcc-objs/process.o\t    4316  Sun Jul 04 19:25:11 1999");
	puts("    mit-parser/gcc-objs/stabd.o\t\t    3741  Sun Jul 04 19:25:34 1999");
	puts("    mit-parser/gcc-objs/tokenize.o\t    4334  Sun Jul 04 19:21:46 1999");
	puts("    mit-parser/gcc-objs/utils.o\t\t    5594  Sun Jul 04 19:24:17 1999");
	}

void revinfo(int argc, char **argv)
	{
	register int i;
	for(i=1;i<argc;++i)
		if(!strcmp(argv[i],"-revinfo"))
			{_revinfo(argv[0]);exit(0);}
	}
