/*
 *	sourcelevel.c
 *
 * (c) by Samuel DEVULDER, 1996.
 */

#include "parse.h"

PRIVATE int lineno;
PRIVATE bool dice_code = FALSE; /* pdc may interfere with dice */

/*
 * outputs linenumbering code when needed
 */
void dump_lineno(void)
	{
	ifn(lineno) return;
	if(sas)	{
		if(use_A4) fprintf(ofp,"\tmove.w\t#%d,AP_lineno(%s)\n",lineno,basereg);
		else	   fprintf(ofp,"\tmove.w\t#%d,(AP_lineno).l\n",lineno);
		}
	else	{
		if(use_A4) fprintf(ofp,"\tmove.w\t#%d,_AP_lineno(%s)\n",lineno,basereg);
		else	   fprintf(ofp,"\tmove.w\t#%d,_AP_lineno\n",lineno);
		}
	lineno = 0;
	}

/*
 * reset line numbering
 */
void reset_lineno(void)
 	{
 	lineno = 0;
 	}

/*
 * source level for sas. returns TRUE if sas source level info
 * are found
 */
PRIVATE bool sas_sl(void)
	{
	char *s=line;
	int num;

	if(*s!=';') return FALSE;
	++s;
	while(*s==' ') ++s; 
	num = 0;while(*s>='0' && *s<='9') {num*=10;num+=*s-'0';++s;}
	if(*s!=':') return FALSE;

	lineno = num;
	
	return TRUE;
	}

/*
 * source level for dcc. returns TRUE if dice source level info
 * are found
 */
PRIVATE bool dcc_sl(void)
	{
	if(strcasecmp(opcode,"debug")) return FALSE;
	lineno = atoi(arg1);
	dice_code = TRUE;
	return TRUE;
	}


/*
 * source level for pdc. returns TRUE if PDC source level info
 * are found
 */
PRIVATE int pdc_find_line(char *line)
	{
	static FILE *source;
	static int num = 0;
	static char buff[LSIZE];

	if(!source && !(source=fopen(source_is,"r")))
		{/* side effect: source will be closed at program exit() */
		my_perror(source_is);
		return 0;
		}
	while(fgets(buff,LSIZE-1,source))
		{
		++num;
		ifn(strcmp(buff,line)) return num;
		}

	return 0;
	}
PRIVATE bool pdc_sl(void)
	{
	ifn(line[0]==';' && line[1]==';' && line[2]==';' && line[3]=='\t') return FALSE;
	ifn(lineno=pdc_find_line(line+4)) return FALSE;
	return TRUE;
	}

/*
 * use source level infos
 */
void source_level()
	{
	if(sas) {sas_sl();return;}
	if(dcc_sl()) return;
	if(!dice_code && pdc_sl()) return;
	}
