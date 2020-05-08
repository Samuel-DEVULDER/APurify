#define cctimes 422
#define VNAME "APF"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef VNUM
#define VNUM "0.0"
#endif

#ifdef __PDC__
static char _IDstring[1]={0};
static char IDstring1[]="$VER: APF "VNUM" (04.07.99) [c: 422] © by Samuel Devulder.";
#else
static char _IDstring[]="\0$VER: APF "VNUM" (04.07.99) [c: 422] © by Samuel Devulder.";
#endif
char *IDstring=&_IDstring[7];

static void _revinfo(char *name)
	{
	printf("%s\n\n",(char*)IDstring);
	printf("revinfo(\"%s\"): 422 compilations\n",name);
	puts("    frontend/gcc-objs/args.o\t\t    9342  Sun Jul 04 19:30:45 1999");
	puts("    frontend/gcc-objs/dcc.o\t\t    8551  Sun Jul 04 19:32:31 1999");
	puts("    frontend/gcc-objs/gcc.o\t\t    8655  Sun Jul 04 19:33:27 1999");
	puts("    frontend/gcc-objs/globals.o\t\t    4083  Sun Jul 04 19:31:33 1999");
	puts("    frontend/gcc-objs/main.o\t\t    5299  Sun Jul 04 19:31:10 1999");
	puts("    frontend/gcc-objs/pdc.o\t\t    8755  Sun Jul 04 19:32:59 1999");
	puts("    frontend/gcc-objs/sas.o\t\t   11647  Sun Jul 04 19:32:03 1999");
	puts("    frontend/gcc-objs/utils.o\t\t   25006  Sun Jul 04 19:34:16 1999");
	}

void revinfo(int argc, char **argv)
	{
	register int i;
	for(i=1;i<argc;++i)
		if(!strcmp(argv[i],"-revinfo"))
			{_revinfo(argv[0]);exit(0);}
	}
