#define cctimes 642
#define VNAME "MOT-APurify"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef VNUM
#define VNUM "0.0"
#endif

#ifdef __PDC__
static char _IDstring[1]={0};
static char IDstring1[]="$VER: MOT-APurify "VNUM" (04.07.99) [c: 642] © by Samuel Devulder.";
#else
static char _IDstring[]="\0$VER: MOT-APurify "VNUM" (04.07.99) [c: 642] © by Samuel Devulder.";
#endif
char *IDstring=&_IDstring[7];

static void _revinfo(char *name)
	{
	printf("%s\n\n",(char*)IDstring);
	printf("revinfo(\"%s\"): 642 compilations\n",name);
	puts("    mot-parser/gcc-objs/args.o\t\t   10396  Sun Jul 04 19:16:39 1999");
	puts("    mot-parser/gcc-objs/branch.o\t    5012  Sun Jul 04 19:17:06 1999");
	puts("    mot-parser/gcc-objs/code_section.o\t    3579  Sun Jul 04 19:11:04 1999");
	puts("    mot-parser/gcc-objs/globals.o\t    4329  Sun Jul 04 19:14:07 1999");
	puts("    mot-parser/gcc-objs/inst.o\t\t   10771  Sun Jul 04 19:11:34 1999");
	puts("    mot-parser/gcc-objs/iscomment.o\t    3168  Sun Jul 04 19:14:30 1999");
	puts("    mot-parser/gcc-objs/jbsr.o\t\t    3850  Sun Jul 04 19:11:59 1999");
	puts("    mot-parser/gcc-objs/label.o\t\t    8705  Sun Jul 04 19:13:17 1999");
	puts("    mot-parser/gcc-objs/lstack.o\t    3460  Sun Jul 04 19:14:54 1999");
	puts("    mot-parser/gcc-objs/main.o\t\t    4086  Sun Jul 04 19:12:23 1999");
	puts("    mot-parser/gcc-objs/movem.o\t\t    5484  Sun Jul 04 19:13:43 1999");
	puts("    mot-parser/gcc-objs/process.o\t    5037  Sun Jul 04 19:15:43 1999");
	puts("    mot-parser/gcc-objs/sasbugs.o\t   10922  Sun Jul 04 19:17:41 1999");
	puts("    mot-parser/gcc-objs/sourcelevel.o\t    5620  Sun Jul 04 19:18:06 1999");
	puts("    mot-parser/gcc-objs/tokenize.o\t    4629  Sun Jul 04 19:12:48 1999");
	puts("    mot-parser/gcc-objs/utils.o\t\t    5594  Sun Jul 04 19:16:09 1999");
	puts("    mot-parser/gcc-objs/xref.o\t\t    4336  Sun Jul 04 19:15:18 1999");
	}

void revinfo(int argc, char **argv)
	{
	register int i;
	for(i=1;i<argc;++i)
		if(!strcmp(argv[i],"-revinfo"))
			{_revinfo(argv[0]);exit(0);}
	}
