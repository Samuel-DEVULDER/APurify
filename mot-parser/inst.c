/*
 * inst.c
 *
 * (c) by Samuel DEVULDER, 1995
 */

#include "parse.h"

#define LL 4        /* size of adressed memory */
#define LW 2
#define LB 1
                    /* ways of accessing memory: */
#define NO 0        /* no access         */
#define RD 1        /* acces in reading  */
#define WR 2        /* access in writing */

typedef struct inst 
	{ /* list of instructions that might references memory */
	char *name;
	char len,arg1,arg2;
	} INST;

/*
 * This array lists all possible instructions that might reference
 * the memory and the way it does (does it RD or WRe; does it
 * acces a byte, a word or a long)..
 *
 * If you notice any errors, please tell me about it.
 */
PRIVATE INST tab_inst[]= 
	{

	{"abcd",   LB,RD,WR},
	
	{"add.b",  LB,RD,WR},{"add.w",  LW,RD,WR},{"add.l",  LL,RD,WR},
	{"adda.w", LW,RD,WR},{"adda.l", LL,RD,WR},
	{"addi.b", LB,RD,WR},{"addi.w", LW,RD,WR},{"addi.l", LL,RD,WR},
	{"addq.b", LB,RD,WR},{"addq.w", LW,RD,WR},{"addq.l", LL,RD,WR},
	{"addxb.", LB,RD,WR},{"addx.w", LW,RD,WR},{"addx.l", LL,RD,WR},
	
	{"and.b",  LB,RD,WR},{"and.w",  LW,RD,WR},{"and.l",  LL,RD,WR},
	{"andi.b", LB,RD,WR},{"andi.w", LW,RD,WR},{"andi.l", LL,RD,WR},
	
	{"asl.b",  LB,RD,WR},{"asl.w",  LW,RD,WR},{"asl.l",  LL,RD,WR},
	
	{"bchg",   LB,RD,WR},
	{"bchg.b", LB,RD,WR},{"bchg.w", LW,RD,WR},{"bchg.l", LL,RD,WR},
	{"bclr",   LB,RD,WR},
	{"bclr.b", LB,RD,WR},{"bclr.w", LW,RD,WR},{"bclr.l", LL,RD,WR},
	{"bset",   LB,RD,WR},
	{"bset.b", LB,RD,WR},{"bset.w", LW,RD,WR},{"bset.l", LL,RD,WR},
	{"btst",   LB,RD,RD},
	{"btst.b", LB,RD,RD},{"btst.w", LW,RD,RD},{"btst.l", LL,RD,RD},
	
	{"clr.b",  LB,WR,NO},{"clr.w",  LW,WR,NO},{"clr.l",  LL,WR,NO},
	
	{"cmp.b",  LB,RD,RD},{"cmp.w",  LW,RD,RD},{"cmp.l",  LL,RD,RD},
	{"cmpa.b", LB,RD,RD},{"cmpa.w", LW,RD,RD},{"cmpa.l", LL,RD,RD},
	{"cmpi.b", LB,RD,RD},{"cmpi.w", LW,RD,RD},{"cmpi.l", LL,RD,RD},
	{"cmpm.b", LB,RD,RD},{"cmpm.w", LW,RD,RD},{"cmpm.l", LL,RD,RD},
	
	{"divs",   LW,RD,WR},{"divs.l", LL,RD,WR},
	{"divu",   LW,RD,WR},{"divu.l", LL,RD,WR},
	
	{"eor.b",  LB,RD,WR},{"eor.w",  LW,RD,WR},{"eor.l",  LL,RD,WR},
	{"eori.b", LB,RD,WR},{"eori.w", LW,RD,WR},{"eori.l", LL,RD,WR},
	
	{"lsl.b",  LB,RD,WR},{"lsl.w",  LW,RD,WR},{"lsl.l",  LL,RD,WR},
	{"lsr.b",  LB,RD,WR},{"lsr.w",  LW,RD,WR},{"lsr.l",  LL,RD,WR},
	
	{"move.b", LB,RD,WR},{"move.w", LW,RD,WR},{"move.l", LL,RD,WR},
	{"movem.b",LB,RD,WR},{"movem.w",LW,RD,WR},{"movem.l",LL,RD,WR},
	{"movep.b",LB,RD,WR},{"movep.w",LW,RD,WR},{"movep.l",LL,RD,WR},
	{"moveq",  LL,RD,WR},
	{"moveq.l",LL,RD,WR},
	
	{"muls",   LW,RD,WR},{"muls.l", LL,RD,WR},
	{"mulu",   LW,RD,WR},{"mulu.l", LW,RD,WR},
	
	{"nbcd",   LB,WR,NO},
	
	{"neg.b",  LB,WR,NO},{"neg.w",  LW,WR,NO},{"neg.l",  LL,WR,NO},
	{"negx.b", LB,WR,NO},{"negx.w", LW,WR,NO},{"negx.l", LL,WR,NO},

	{"not.b",  LB,WR,NO},{"not.w",  LW,WR,NO},{"not.l",  LL,WR,NO},
	
	{"or.b",   LB,RD,WR},{"or.w",   LW,RD,WR},{"or.l",   LL,RD,WR},
	{"ori.b",  LB,RD,WR},{"ori.w",  LW,RD,WR},{"ori.l",  LL,RD,WR},
	
	{"rol.b",  LB,RD,WR},{"rol.w",  LW,RD,WR},{"rol.l",  LL,RD,WR},
	{"roxl.b", LB,RD,WR},{"roxl.w", LW,RD,WR},{"roxl.l", LL,RD,WR},
	
	{"ror.b",  LB,RD,WR},{"ror.w",  LW,RD,WR},{"ror.l",  LL,RD,WR},
	{"roxr.b", LB,RD,WR},{"roxr.w", LW,RD,WR},{"roxr.l", LL,RD,WR},
	
	{"sbcd",   LB,RD,WR},
	
	{"shi",    LB,WR,NO},
	{"sls",    LB,WR,NO},
	{"scc",    LB,WR,NO},  
	{"scs",    LB,WR,NO},  
	{"sne",    LB,WR,NO},  
	{"seq",    LB,WR,NO},  
	{"svc",    LB,WR,NO},  
	{"svs",    LB,WR,NO},  
	{"spl",    LB,WR,NO},  
	{"smi",    LB,WR,NO},  
	{"sge",    LB,WR,NO},
	{"slt",    LB,WR,NO},
	{"sgt",    LB,WR,NO},
	{"sle",    LB,WR,NO},

	{"sub.b",  LB,RD,WR},{"sub.w",  LW,RD,WR},{"sub.l",  LL,RD,WR},
	{"suba.w", LW,RD,WR},{"suba.l", LL,RD,WR},
	{"subi.b", LB,RD,WR},{"subi.w", LW,RD,WR},{"subi.l", LL,RD,WR},
	{"subq.b", LB,RD,WR},{"subq.w", LW,RD,WR},{"subq.l", LL,RD,WR},
	{"subx.b", LB,RD,WR},{"subx.w", LW,RD,WR},{"subx.l", LL,RD,WR},
	
	{"tas",    LB,WR,RD},
	
	{"tst.b",  LB,RD,NO},{"tst.w",  LW,RD,NO},{"tst.l",  LL,RD,NO},

	{"pea",    LW,WR,NO}, 
	        /* we assume that an address is pushed for writing */

	{NULL,0,0,0}  
	        /* sentinel */
	};

PRIVATE int tab_inst_len;

/*
 * Compare two INST 
 */
PRIVATE int inst_cmp(INST *i1,INST *i2)
	{
	return strcasecmp(i1->name,i2->name);
	}

/*
 * Sort the tab_inst array for the binary search.
 */
void inst_init()
	{
	int len;
	INST *inst=tab_inst;
	
	for(len = 0; inst->name; ++inst,++len);
	qsort((void *)tab_inst, (unsigned)(tab_inst_len=len), (int)sizeof(INST), (void *)inst_cmp);
	}

/*
 * Return the entry in the array above for the opcode or NULL if
 * it is not in that array. Binary search.
 */
PRIVATE INST *chk_opcode(char *opcode)
	{
	register INST *array = tab_inst;
	int first = 0;
	int last = tab_inst_len - 1;
	int mid;
	int diff;

	if(!*opcode) return NULL;

	/* binary search */
	while( first <= last ) {
		mid  = (first+last) >> 1;
		diff = strcasecmp(opcode, array[mid].name);
		if( !diff )
			return( &array[ mid ] ); /* found */
		if( first == last )
			break; /* not found */
		if( diff < 0 )
			last = mid - 1;
		else
			first = mid + 1;
		}

	return NULL;
	}

/*
 * Parse the argument and return the effective adress or NULL if arg
 * does not reference memory through indirection.
 */
char *get_ea(char *arg)
	{
	char reg[8],*s,*t;
	static char ea[80];

	/*
	 * grap base register.. we assume the first pattern as
         * (An [An ,An gives the base register. If no such pattern
	 * exists, we claim the arg is not indirect mode.
	 */
	reg[0]='\0';
        for(s=arg;*s;++s)
		{
		if((s[0]=='(' || s[0]=='[' || s[0]==',') &&
		   (
		    (tolower(s[1])=='a' && s[2]>='0' && s[2]<='7') ||
		    (tolower(s[1])=='p' && tolower(s[2])=='c') ||
		    (tolower(s[1])=='s' && tolower(s[2])=='p')))
			{
			reg[0]=s[1];
			reg[1]=s[2];
			reg[2]='\0';
			break;
			}
		
		}
	ifn(*reg) return NULL; /* not indirect mode */


	if(!strcasecmp(reg,"pc")) return NULL;
        if(!strcasecmp(reg,basereg) && !test_basereg)  return NULL;
        if(!strcasecmp(reg,lstackreg) && !test_lstack) return NULL;
        if((!strcasecmp(reg,"sp") || !strcasecmp(reg,"a7")) && !test_stack)
              return NULL;

	/*
	 * clean decrement -(reg)
	 */
	s=arg;
	if(s[0]=='-' && s[1]=='(' && !strncmp(s+2,reg,2) && s[4]==')') ++s;

	for(t=ea;*s;*t++=*s++);
	
	/* 
	 * clean increment
	 */
	if(t[-1]=='+') --t;
	*t='\0';
	return ea;
	}

/*
 * Put code to check memory accesses.
 */
PRIVATE void do_put_acces(char len,char rw,char *arg)
	{
	char *ea;
	char func[16];

	ifn(ea=get_ea(arg)) return;
	
	if(source_is) dump_lineno();

	/* patch for pre-decrementation */
	if(arg[0]=='-' && arg[1]=='(')
		fprintf(ofp,"\tpea -%d%s\n",len,ea);
	else	fprintf(ofp,"\tpea\t%s\n",ea);
	
	if(rw==RD)	strcpy(func,"AP_Read");
	else		strcpy(func,"AP_Write");

	strcat(func,len==LB?"B":len==LW?"W":"L");
	put_jsr(func);
	}

/*
 * Test if current instruction references memory through indirection
 * and put special code in that case.
 */
void inst_chk()
	{
	INST *ip;
 
	if(chk_branch()) return;

	ifn(ip = chk_opcode(opcode)) return;

	if(!strncasecmp(opcode,"movem",5))
	        {
	        movem_chk(ip->len,arg1,arg2);
	        return;
	        }
	
        if(!test_pea && !strcasecmp(opcode,"pea")) return;
	if(ip->arg1!=NO) do_put_acces(ip->len,ip->arg1,arg1);
	if(ip->arg2!=NO) do_put_acces(ip->len,ip->arg2,arg2);
	}
