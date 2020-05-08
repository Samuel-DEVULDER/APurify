/*
 * makehistory.c
 *
 * Add last history note to the readme file
 */
#include <stdio.h>

#ifdef __GNUC__
#include <sys/time.h>		/* dosextens.h needs it ! */
#endif
#include <exec/exec.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#ifdef __SASC
#include <proto/dos.h>
#include <proto/exec.h>
#endif

#define ifn(x)	if(!(x))

#define LSIZE 512

char line[LSIZE];
char *version,*readme_name,*history_name,*tmp_name;

FILE *readme_file,*history_file,*tmp_file;

void usage(char *n)
	{
	printf("\nUsage: %s version readmefile historyfile\n\n",n);
	printf("(c) by Samuel Devulder. ("__DATE__")\n");
	exit(5);
	}

void read_till_lasthist(void)
	{
	int lastpos=0,histpos=0;
	while(fgets(line,LSIZE-1,history_file)) 
		{
		if(*line == '-') histpos=lastpos;
		lastpos=ftell(history_file);
		}
	fseek(history_file,histpos,0);
	}

void read_readme1(void)
	{
	int i;
	if(i=fgets(line,LSIZE-1,readme_file)) 
		{char *s,*t;
		s=line;while(*s) ++s;
		while(s[-1]!='V') --s;
		t=version;
		while(*t) *s++=*t++;
		*s++='\n';*s='\0';
		fputs(line,tmp_file);
		}
	if(i) while(i=fgets(line,LSIZE-1,readme_file)) 
		{
		if(*line == '-') break;
		fputs(line,tmp_file);
		}
	if(i) while(fgets(line,LSIZE-1,readme_file)) 
		{
		char *s;
		for(s=line;*s && (*s==' ' || *s=='\t' || *s=='\n');++s);
		ifn(*s) break;
		}
	}

void read_history(void)
	{
	while(fgets(line,LSIZE-1,history_file)) 
		{
		fputs(line,tmp_file);
		}
	}

void read_readme2(void)
	{
	fputs("\n",tmp_file);
	while(fgets(line,LSIZE-1,readme_file)) 
		{
		fputs(line,tmp_file);
		}
	}

main(int ac,char **av)
	{
	if(ac!=4) usage(av[0]);

	version      = av[1];
	readme_name  = av[2];
	history_name = av[3];
	tmp_name     = tmpnam(NULL);

	ifn(readme_file  = fopen(readme_name,"r")) {perror(readme_name);exit(5);}
	ifn(history_file = fopen(history_name,"r")) {perror(history_name);exit(5);}
	ifn(tmp_file = fopen(tmp_name,"w")) {perror(readme_name);exit(5);}
	
	read_till_lasthist();
	read_readme1();
	read_history();
	read_readme2();

	fclose(readme_file);
	fclose(history_file);
	fclose(tmp_file);

	ifn(tmp_file = fopen(tmp_name,"r")) {perror(readme_name);exit(5);}
	ifn(readme_file  = fopen(readme_name,"w")) {perror(readme_name);exit(5);}
	while(fgets(line,LSIZE-1,tmp_file)) 
		{
		fputs(line,readme_file);
		}
	fclose(readme_file);
	fclose(tmp_file);	
	unlink(tmp_name);
	return(0);
	}
