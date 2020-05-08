/*
 *	parse.h 
 *
 *	(c) by Samuel DEVULDER
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#if	defined(_DCC) || defined(__SASC)
#define	strcasecmp	stricmp
#define	strncasecmp	strnicmp
#endif

/* 
 * usefull defines
 */
#define	ifn(x) if(!(x))

#ifdef __STDC__
#define _(x)	x
#else
#define	_(x)	()
#endif

#define	PRIVATE static

#define isblank(c)	(c==' ' || c=='\t' || c=='\n' || c=='\r')
#define	separator(c)	(c==',' || c==';') /* ';' is for comments */
/*
 * types
 */
typedef int bool;
#define	TRUE	1
#define	FALSE	0

#define	LSIZE	1024

/*
 * global variables;
 */
extern	FILE	*ifp,	/* input file */
		*ofp;	/* output file */

extern	char	basereg[];
extern	char	lstackreg[];
extern	char	*proc_main;
extern	bool	test_basereg;
extern	bool	test_stack;
extern	bool	test_lstack;
extern  bool    test_pea;
extern	bool	code_section;
extern	bool	xref_AP_Init;


extern	char	*ProgName,*IDstring;
extern	char	*source_name;
extern	char	*dest_name;
extern	char	*source_is;

extern	char	line[];
extern	char	label[];
extern	char	opcode[];
extern	char	arg1[];
extern	char	arg2[];
extern	char	arg3[];

/*
 *	functions
 */
extern	void	copy			_((char *,char *));
extern	void	tokenize		_((void));
extern	void	code_section_chk	_((void));
extern	void	label_chk		_((void));
extern	bool	jbsr_chk		_((void));
extern	void	lstack_chk		_((void));
extern  void    inst_chk                _((void));
extern  void    inst_init               _((void));
extern	bool	iscomment		_((void));
extern  void    movem_chk               _((int, char *,char *));
extern  char    *get_ea                 _((char *));
extern  void    revinfo                 _((int,char**));
extern	void	my_perror		_((char *name));
extern	bool	process_file		_((char *,char *));
extern	void	usage			_((void));
extern	void	parse_args		_((int,char **));
extern	void	parse_env		_((char *));
extern	char	*build_name		_((char *,char *));
extern	bool	stabd_chk		_((void));
extern	void	reset_stabd		_((void));
extern	void	dump_lineno		_((void));
