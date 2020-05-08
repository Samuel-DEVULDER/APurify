/*
 *     movem.c
 *
 * (c) by Samuel DEVULDER, 1995
 */

#include "parse.h"

/*
 * Rn-Rm | Rn ==> nb regs
 */
PRIVATE int dorspec(char *s)
	{
	register int	m, n;
	
	if(s[0]!='a' && s[0]!='A' && s[0]!='d' && s[0]!='D') return -1;

	if(s[1]>'7' || s[1]<'0') return -1;
	m=s[1]-'0';

	if(s[2] != '-') return 1;

	if((s[3]!='d') && (s[3]!='D') && (s[3]!='a') && (s[3]!='A'))
		return -1;

	if(s[4]>'7' || s[4]<'0') return 0;
	n = s[4] - '0';

	return n-m+1;
	}

/*
 * stonr() - return number of used regs in string
 */
PRIVATE int stonr(char *s)
	{
	register int	nb,t;
	register char	*p;

	ifn(*s) return 0;

	nb=dorspec(s);
	if(nb<0) return -1;

	p = s;
	do
		{
		while(*p && *p!= '/') ++p;
		if(*p == '/') 
		        {
			++p;
			t=dorspec(p);
			if(t<0) return -1;
			nb+=t;
			}
		}
	while(*p);

	return nb;
	}

PRIVATE int get_len(char *mask)
        {
	int nb,i;
	char *endm;
	/*
         * try An-Ap...
         */
	nb=stonr(mask);
	if(nb>=0) return nb;
	/*
         * try immediate
	 */
	if(*mask!='#') return -1;
	++mask;
	if(*mask=='<')
	       {
	       char *s=++mask;
	       while(*s && *s!='>') ++s;
	       if(*s!='>') return -1;
	       *s='\0';
	       return stonr(mask);
	       }
	nb=strtol(mask,&endm,0);
	if(*endm) return -1; /* error */
	nb &= 65535;
	i=0;
	while(nb) {if(nb&1) ++i;nb>>=1;}
	return i;
	}

/*
 * Puts code for movem instructions. I don't know if it is used by the
 * -O option, but we could imagine that
 *
 *       move.l 4(a0),d0
 *       move.l 8(a0),d2
 *       move.l 12(a0),a1
 *
 * is optimized in something like
 *
 *       move.l 4(a0),d0/d2/a1
 *
 * So we can't assume that movem is just used to push things on the stack.
 * Thus we have to check its memory access.
 */
void movem_chk(int len,char *arg1,char *arg2)
        {
	char *ea;
	char *arg;
	char *mask;
	if(ea=get_ea(arg1)) {arg=arg1;mask=arg2;}
	else if(ea=get_ea(arg2)) {arg=arg2;mask=arg1;}
	else   return;
	len *= get_len(mask);
	if(len<0)
	         {
		 printf("%s: strange movem found: %s,%s !\n",
			ProgName,arg1,arg2);
		 return;
		 }

	if(source_is) dump_lineno();

	fprintf(ofp,"\tpea\t%d\n",len);

	/* patch for pre-decrementation */
	if(arg[0]=='-' && len) 
		fprintf(ofp,"\tpea\t%d%s\n",-len,ea);
	else 	fprintf(ofp,"\tpea\t%s\n",ea);

	if(mask==arg2)	put_jsr("AP_Read");
	else		put_jsr("AP_Write");
        }
