# APurify
Enforcer-like tool for Amigas without MMU.
http://aminet.net/package/dev/debug/apurify_v1.5

Short|	Enforcer-like tool without MMU. V1.5
Author|	devulder at info.unicaen.fr (Samuel DEVULDER)
Uploader|	devulder info unicaen fr (Samuel DEVULDER)
Type|	dev/debug
Architecture|	m68k-amigaos
Date|	1996-12-18


A program that detects illegal memory access of your programs. It needs asm sourcefile (or reverse-engineering generated sourcefile) and insert code to check memory access.

It consists  of  a  source  parser  (bin/M?T-APurify),  a  linking  library (lib/#?APur-#?.#?) and a frontend (bin/APF) to  call  the  parser  and  the linker automatically.

This archive contains files for APurify v1.5. There are different  versions of APurify for GCC, PDC, DICE and SAS/C. To install it, you  just  have  to execute the APurify-Install script. It will install everything for you	and will even generate a remover script if you want to remove APurify from your system. If you want to install it by hand, please read the documentation.

For the documentation, consult doc/MIT-APurify.doc for the GCC compiler and doc/MOT-APurify.doc for other compilers. If you want to use  the  frontend, read doc/APF.doc. I strongly encourage you to use it. It is simpler to	use it than making everything by hand.

The source is available ~~~from author~~~ in github and ~~~is not included in that archive. It~~~  requires around 5.5Mb on my harddisk when compiled :-).

Happy debugging !

		Sam.
```