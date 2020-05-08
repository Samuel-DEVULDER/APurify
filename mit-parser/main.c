/*
 *	main.c
 *
 * (c) by Samuel DEVULDER, 1995
 */
#include "parse.h"

int main(argc,argv)
	int	argc;
	char	*argv[];
	{
	char	*tmp_name;

	revinfo(argc,argv);
	
	ProgName = argv[0];
	
	parse_env("AP_MITP_OPT");
	parse_args(argc,argv);
	
	ifn(source_name)
		{
		printf("%s: no source file !\n",ProgName);
		usage();
		}
		
	dest_name = build_name(dest_name,source_name);
	
	tmp_name  = tmpnam(NULL);
	
	inst_init();
	
	/*
	 * real work starts here
	 */
	if(process_file(source_name,tmp_name))
		{
		copy(tmp_name,dest_name);
		unlink(tmp_name);
		}

	return 0;
	}
