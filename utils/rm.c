#include <stdio.h>

main(ac,av)
	int ac;char **av;
	{
	int i;
	for(i=1;i<ac;++i) if(remove(av[i])) perror(av[i]);
	exit(0);
	}
