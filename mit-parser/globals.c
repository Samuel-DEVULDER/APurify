/*
 *	globals.c
 *
 * (c) by Samuel DEVULDER, 1995
 */

#include "parse.h"

FILE	*ifp,*ofp;                 /* sourcefile, destfile */

#ifndef pdc
char	basereg[3]="A4";           /* Our default bas reg is A4 */
#else
char	basereg[3]={'A','4',0};
#endif
char	lstackreg[3];
char	*proc_main     = "_main";
bool	test_basereg   = TRUE;
bool	test_stack     = FALSE;
bool	test_lstack    = FALSE;
bool    test_pea       = FALSE;

bool	code_section   = FALSE;    /* We say that by default we work */
                                   /* in data section. */ 

char	line[LSIZE];               /* current line */
char	label[LSIZE];              /* current label */
char	opcode[LSIZE];             /* current opcode */
char	arg1[LSIZE];               /* current first arg */
char	arg2[LSIZE];               /* current second arg */
char	arg3[LSIZE];               /* current third arg */

char	*ProgName;                 /* a copy of argv[0] */
char	*source_name;		   /* inputname */
char	*dest_name="%s";	   /* outputname */
char	*source_is=NULL;	   /* name of original source */
