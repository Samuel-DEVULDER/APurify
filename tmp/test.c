#include <stdio.h>

main()
	{
	FILE *f;
	if(f=fopen("dummy","a"))
		{
		fprintf(f,"ok\n");
		fclose(f);
		}
	else perror("dummy");
	}
