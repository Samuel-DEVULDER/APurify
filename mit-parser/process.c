/*
 *	process.c
 *
 * (c) by Samuel DEVULDER, 1995
 */
#include "parse.h"

PRIVATE void process_line(void)
	{
	if(!iscomment())
		{
		tokenize();
		code_section_chk();
		if(code_section)
			{
			label_chk();
			if(jbsr_chk()) return;
			if(stabd_chk()) return;
			lstack_chk();
			inst_chk();
			}
		}
	fputs(line,ofp);
	}

bool process_file(char *source_name,char *dest_name)
	{
	ifn(ifp=fopen(source_name,"r")) 
		{
		my_perror(source_name);
		return FALSE;
		}
	ifn(ofp=fopen(dest_name,"w"))
		{
		fclose(ifp);ifp=NULL;
		my_perror(dest_name);
		return FALSE;
		}
	while(fgets(line,LSIZE-1,ifp)) process_line();
	fclose(ofp);
	fclose(ifp);ifp=NULL;
	return TRUE;
	}

