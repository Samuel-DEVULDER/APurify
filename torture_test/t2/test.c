char string[]="123456789012345678901234567890123456789012345678901234567890";

char *str[]={&string[0],&string[40],&string[10]};

toto()
	{
	char *s=string;
	char *a=&string[40];
	a=&s[40];
	}
