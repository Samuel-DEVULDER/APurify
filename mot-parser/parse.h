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
extern	bool	use_cseg;
extern	bool	use_section;
extern	bool	use_public;
extern	bool	use_xref;
extern	bool	use_A4;
extern	bool	use_PC;
extern	bool	xref_AP_Init;
extern	bool	sas;

extern	int	offset; /* sasbug #6 */

extern	char	*ProgName,*IDstring;
extern	char	*source_name;
extern	char	*dest_name;
extern	char	*source_is;

extern	char	line[];
extern	char	label[];
extern	char	opcode[];
extern	char	arg1[];
extern	char	arg2[];

/*
 *	functions
 */
extern	void	copy			_((char *, char *));
extern	void	tokenize		_((void));
extern	void	code_section_chk	_((void));
extern	void	label_chk		_((void));
extern	bool	jbsr_chk		_((void));
extern	void	lstack_chk		_((void));
extern  void    inst_chk                _((void));
extern	bool	iscomment		_((void));
extern  void    movem_chk               _((int, char *, char *));
extern  void    put_xref                _((void));
extern  void    put_label               _((void));
extern  char    *get_ea                 _((char *));
extern  void    revinfo                 _((int, char**));
extern	void	inst_init		_((void));
extern	void	xref_chk		_((void));
extern	void	my_perror		_((char *name));
extern	bool	process_file		_((char *,char *));
extern	void	put_jsr			_((char *));
extern	void	usage			_((void));
extern	void	parse_args		_((int,char **));
extern	void	parse_env		_((char *));
extern	char	*build_name		_((char *,char *));
extern	char	*isfunclbl		_((char *));
extern	bool	chk_branch		_((void));
extern	bool	sasbug1			_((char *));
extern	bool	sasbug2			_((char *));
extern	bool	sasbug3			_((char *));
extern	void	sasbug4			_((char *));
extern	bool	sasbug5			_((char *));
extern	void	sasbug6_first		_((char *));
extern	void	sasbug6_last		_((void));
extern	void	sasbug7			_((void));
extern	void	sasbug8			_((char *));
extern	void	sasbug9			_((char *));
extern	void	source_level		_((void));
extern	void	dump_lineno		_((void));
extern	void	reset_lineno		_((void));
