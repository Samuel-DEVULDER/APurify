/*
 * mklink.c - incremantal update of linkfiles. Very simple brute force
 * algorithm.
 *
 * (c) by Samuel DEVULDER, 1996.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

int delim_found;

void Usage(char *n)
	{
	printf("\nUsage: %s <linkfile> <delimitor char> <new entry>\n",n);
	printf("\n(c) by Samuel Devulder, 1996.\n");
	exit(0);
	}

int get_delim(char *s)
	{
	if(!*s) return -1;
	if(*s=='\'' || *s=='\"') ++s;
	if(*s=='\\' || *s=='*')
		{
		char c;
		if(!*++s) return -1;
		c=tolower(*s);
		if(c=='n') return '\n';
		if(c=='r') return '\r';
		if(c=='\\') return '\\';
		if(c=='*') return '*';
		return -1;
		}
	return *s;
	}

char *get_arg(FILE *f,char delim)
	{
	static char buff[1024];
	char *s;
	int c;
	while((c=fgetc(f))!=EOF && c==delim) delim_found = 1;
	if(c==EOF) return NULL;
	ungetc(c,f);
	for(s=buff;s<(buff+1023) && (c=fgetc(f))!=EOF && (c!=delim);*s++=c);
	*s='\0';
	delim_found = 0;
	if(c!=EOF) 
		{
		while(((c=fgetc(f))!=EOF) && c==delim) delim_found = 1;
		if(c!=EOF) ungetc(c,f);
		}
	return buff;
	}

int main(int ac,char **av)
	{
	int delimitor;
	FILE *f;
	char *s;
	int flag = 1;
	
	if(ac!=4) Usage(av[0]);
	delimitor=get_delim(av[2]);
	if(delimitor==-1) Usage(av[0]);
	if(!(f=fopen(av[1],"r+"))) 
		{
		if(errno == ENOENT && !(f=fopen(av[1],"w")))
			{
			perror(av[1]);
			return 5;
			}
		}
	delim_found = 0;
	while(s=get_arg(f,delimitor))
		{
		flag = 0;
		if(!strcmp(s,av[3])) return 0;
		}
	fseek(f,0,2);
	if(!delim_found && !flag) fputc(delimitor,f);
	fputs(av[3],f);fputc(delimitor,f);
	fclose(f);
	return 0;
	}
