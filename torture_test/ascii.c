#include <proto/dos.h>

void main(void)
{
	int i;

	PutStr("[32m  ");

	for (i = 0; i < 16; i++)
		Printf("  %lx ", i);

	PutStr("[31m\n");

	for (i = 0; i < 256; i++)
	{
		if (0 == (i % 16))
			Printf("[32m%lx [31m", i / 16);

		if (((i >= 32) && i < 128) || (i >= 160))
			Printf("  %lc ", i);
		else
			Printf("    ");

		if (0 == ((i + 1) % 16))
			Printf("\n");
	}
}
