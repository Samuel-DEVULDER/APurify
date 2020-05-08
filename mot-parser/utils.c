/*
 *	utils.c
 *
 * (c) by Samuel DEVULDER, 1995
 */

#include "parse.h"

/*
 * a kind of cp...
 */
void copy(src,dst)
	char *src;
	char *dst;
	{
	FILE *f,*g;
	if(f=fopen(src,"r"))
		{
		if(g=fopen(dst,"w"))
			{
			while(fgets(line,LSIZE-1,f)) fputs(line,g);
			fclose(g);
			}
		else	my_perror(dst);
		fclose(f);
		}
	else	my_perror(src);
	}

/*
 *	a more verbose perror()
 */
void my_perror(char *name)
	{
	fprintf(stderr,"%s: ",ProgName);
	perror(name);
	}

#if	0
/* 
 *	DCC doesn't have this
 */
#if defined(_DCC) || defined(__SASC)
int strcasecmp(char *s, char *d)
	{
	while(*s && tolower(*s)==tolower(*d)) {++s;++d;}
	return tolower(*s)-tolower(*d);
	}

int strncasecmp(char *s,char *d,int n)
	{
	++n;
	while(n-- && *s && tolower(*s)==tolower(*d)) {++s;++d;}
	return tolower(*s)-tolower(*d);
	}
#endif
#endif

/*
 * %s stands for full name. %S for fullname without extension.
 * %b = basename. %B = basename without extens.
 * %p = path. %e = extens
 */
char *build_name(char *pat,char *name)
	{
	char *S,*b="",*B=b,*p;
	char *e=b;
	char *s,*t,*r;
	int len;

	ifn(S=strdup(name)) return NULL;
	s=S+strlen(S);
	while(s>S) 
		{
		if(*s=='.') {*s='\0';e=s+1;break;} 
		--s;
		}

	s=name+strlen(name);
	while(s>name) 
		{
		if(*s=='/' || *s==':') {b=s+1;break;}
		--s;
		}
	s=S+strlen(S);
	while(s>S) 
		{
		if(*s=='/' || *s==':') {B=s+1;break;}
		--s;
		}

	ifn(p=strdup(S)) return NULL;
	s=p+strlen(p);
	while(s>p) 
		{
		if(*s=='/' || *s==':') {s[1]='\0';break;}
		--s;
		}

	len = 0;
	for(s=pat;*s;++s)
		{
		if(*s=='%')
			{
			if(s[1]=='s')		len+=strlen(name);
			else if(s[1]=='S')	len+=strlen(S);
			else if(s[1]=='b')	len+=strlen(b);
			else if(s[1]=='B')	len+=strlen(B);
			else if(s[1]=='p')	len+=strlen(p);
			else if(s[1]=='e')	len+=strlen(e);
			else {++len;continue;}
			++s;
			}
		else ++len;
		}
	ifn(r=t=malloc(len+1)) return NULL;
	for(s=pat;*s;++s)
		{
		if(*s=='%')
			{
			if(s[1]=='s')		strcpy(t,name);
			else if(s[1]=='S')	strcpy(t,S);
			else if(s[1]=='b')	strcpy(t,b);
			else if(s[1]=='B')	strcpy(t,B);
			else if(s[1]=='p')	strcpy(t,p);
			else if(s[1]=='e')	strcpy(t,e);
			else {*t++=*s;continue;}
			t+=strlen(t);
			++s;
			}
		else *t++=*s;
		}
	*t='\0';
	free(p);free(S);
	return r;
	}
