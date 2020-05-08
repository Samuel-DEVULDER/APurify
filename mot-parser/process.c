/*
 *	process.c
 *
 * (c) by Samuel DEVULDER, 1995
 */
#include "parse.h"

PRIVATE bool process_line(bool xref_done)
	{
	if(!iscomment())
		{
		tokenize();
		code_section_chk();
		xref_chk();
		if(code_section)
			{
			label_chk();
			if(jbsr_chk()) goto end;
			lstack_chk();
			inst_chk();
			}
		if(!strcasecmp(opcode,"end") && !xref_done)
			{
			put_label();
			put_xref();
			xref_done=TRUE;
			}
		}
	if(source_is && code_section) source_level();
	fputs(line,ofp);
end:	if(sas) sasbug6_first(line);
	return xref_done;
	}

bool process_file(char *source_name,char *dest_name)
	{
	bool xref_done;
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

	if(sas) sasbug6_first(NULL);
	
	xref_done = FALSE;
	while(fgets(line,LSIZE-1,ifp)) 
		{
		xref_done = process_line(xref_done);
		}
	if(!xref_done) 
		{
		put_label();
		put_xref();
		}
	fclose(ofp);
	fclose(ifp);ifp=NULL;
	return TRUE;
	}

