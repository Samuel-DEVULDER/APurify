/*
 * globals.c
 *
 * (c) by Samuel DEVULDER, 1996
 */
#include "apf.h"

char 		*ProgName;

enum compiler	Compiler	= COMPILER_UNK;

enum phase	PhaseFrom	= PHASE_UNK,
		PhaseTo		= PHASE_UNK;

int		SourceIdx	= 0;
int		DestIdx		= 0;
int		LinkIdx		= 0; 
/* pointer to the 'LINK' or '-c', '-a', '-S' keyword */


int 		AC;
char 		**AV;

char		*mot_parser	= "Mot-APurify";
char		*mit_parser	= "Mit-APurify";

char		*gcclib		= "-lAPur-gcc";
char		*dcclib		= "-lAPur-dcc";
char		*pdclib		= "-lAPur-pdc";
char		*saslib		= "LIB=LIB:APur-sas.lib";

bool		verbose		= FALSE;
bool		source_level	= FALSE;
