/*
 *	apf.h 
 *
 *	(c) by Samuel DEVULDER
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#ifdef	__PDC__
#define	stricmp	strcasecmp
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

/*
 * types
 */
typedef int bool;
#define	TRUE	1
#define	FALSE	0

/*
 * global variables
 */
extern	char *ProgName,*IDstring;

extern enum compiler
	{
	COMPILER_UNK,
	COMPILER_GCC,
	COMPILER_PDC,
	COMPILER_SAS,
	COMPILER_DCC
	} Compiler;

extern enum phase
	{
	PHASE_UNK,
	PHASE_C,	/* C file */
	PHASE_A,	/* Asm file */
	PHASE_O,	/* Object file */
	PHASE_E		/* Exec file */
	} PhaseFrom, PhaseTo;

extern	int	SourceIdx;
extern	int	DestIdx;
extern	int	LinkIdx;

extern	int	AC;
extern	char	**AV;

extern	bool	verbose;
extern	bool	source_level;

extern	char	*mot_parser;
extern	char	*mit_parser;

extern	char	*gcclib;
extern	char	*dcclib;
extern	char	*pdclib;
extern	char	*saslib;

/*
 *	functions
 */
extern	void	revinfo			_((int, char **));
extern	void	usage			_((void));
extern	void	parse_args		_((int,char **));
extern	void	parse_env		_((char *));
extern	void	*s_malloc		_((unsigned long));
extern	void	*s_calloc		_((long, long));
extern	char	*s_strdup		_((char *));
extern	char	*basename		_((char *));
extern	char	*extension		_((char *));
extern	char	*noextension		_((char *));
extern	void	duplicate_AC_AV		_((int, char **));
extern	void	add_arg			_((char *));
extern	void	sub_arg			_((void));
extern	void	sas_compile		_((void));
extern	void	sas_parse		_((void));
extern	void	dcc_compile		_((void));
extern	void	dcc_parse		_((void));
extern	void	pdc_compile		_((void));
extern	void	pdc_parse		_((void));
extern	void	gcc_compile		_((void));
extern	void	gcc_parse		_((void));
extern	void	split_arg		_((int, char *));
extern	bool	access_c		_((char *));
extern	void	execute			_((void));
extern	void	copy			_((char *, char *));
extern	void	apurify			_((char *, char *, char *, char *));
extern	void	set_source		_((int));
extern	void	set_dest		_((int));
extern	void	process			_((char *,char *,char *,char *));
extern	void	Unlink			_((char *));
