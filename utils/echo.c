/*
 * echo.c
 *
 * c:echo can't handle arguments taht are too big.. this one is better
 */
#include <stdio.h>

main(int ac,char **av)
	{
	int i;
	char d,c;
	char *s;
	
	for(i=1;i<ac;++i)
		{
		s=av[i];
		d=0;
		while(c=*s++)
			{
			if(c=='*')
				{
				if(tolower(*s)=='n') {c='\n';++s;}
				else if(tolower(*s)=='e') {c='\033';++s;}
				else if(tolower(*s)=='*') {c='*';++s;}
				}
			fputc(c,stdout);
			d=c;
			}
		if(d && d!='\n' && i<ac-1) fputc(' ',stdout);
		}
	putc('\n',stdout);
	return 0;
	}
