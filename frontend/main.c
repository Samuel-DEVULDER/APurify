/*
 * main.c - apurify frontend.
 *
 * (c) by Samuel DEVULDER, 1996
 */
#include "apf.h"

/*
 * determine which compiler is used
 */
void set_compiler(void)
	{
	char *bn=basename(AV[0]);

	ifn(stricmp(bn,"gcc") && stricmp(bn,"g++") && stricmp(bn,"gcc20"))
		{
		Compiler = COMPILER_GCC;
		return;
		}
	ifn(stricmp(bn,"ccx"))
		{
		Compiler = COMPILER_PDC;
		return;
		}
	ifn(stricmp(bn,"dcc"))
		{
		Compiler = COMPILER_DCC;
		return;
		}
	ifn(stricmp(bn,"sc") && stricmp(bn,"slink"))
		{
		Compiler = COMPILER_SAS;
		return;
		}
	printf("%s: Unknown compiler: \"%s\"\n",ProgName,AV[0]);
	exit(5);
	}

/*
 * get the source & the dest of the process
 */
void set_phases(void)
	{
	switch(Compiler)
		{
		case COMPILER_SAS: sas_parse(); return;
		case COMPILER_DCC: dcc_parse(); return;
		case COMPILER_PDC: pdc_parse(); return;
		case COMPILER_GCC: gcc_parse(); return;
		}
	}

/*
 * compile according to the compiler
 */
void compile(void)
	{
	switch(Compiler)
		{
		case COMPILER_SAS: sas_compile(); return;
		case COMPILER_DCC: dcc_compile(); return;
		case COMPILER_PDC: pdc_compile(); return;
		case COMPILER_GCC: gcc_compile(); return;
		}
	}

int main(int argc, char **argv)
	{
	revinfo(argc,argv);
	
	ProgName = argv[0];
	
	parse_env("APF_OPT");
	parse_args(argc,argv);
	
	if(AC<2) usage();

	set_compiler();
	set_phases();
	compile();
	
	return 0;
	}
