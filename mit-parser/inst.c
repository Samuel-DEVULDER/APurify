/*
 * inst.c
 *
 * (c) by Samuel DEVULDER, 1995
 */

#include "parse.h"

#define LENL 4        /* size of adressed memory */
#define LENW 2
#define LENB 1
                      /* ways of accessing memory: */
#define NONE 0        /* no access         */
#define READ 1        /* acces in reading  */
#define WRIT 2        /* access in writing */

typedef struct inst 
	{ /* list of instructions that might references memory */
	char *name;
	char len,arg1,arg2;
	} INST;

/*
 * This array lists all possible instructions that might reference
 * the memory and the way it does (does it read or write; does it
 * acces a byte, a word or a long)..
 *
 * If you notice any errors, please tell me about it.
 */
PRIVATE INST tab_inst[]= {

{"abcd",  LENB,READ,WRIT},

{"addb",  LENB,READ,WRIT},{"addw",  LENW,READ,WRIT},{"addl",  LENL,READ,WRIT},
{"addaw", LENW,READ,WRIT},{"addal", LENL,READ,WRIT},
{"addib", LENB,READ,WRIT},{"addiw", LENW,READ,WRIT},{"addil", LENL,READ,WRIT},
{"addqb", LENB,READ,WRIT},{"addqw", LENW,READ,WRIT},{"addql", LENL,READ,WRIT},
{"addxb", LENB,READ,WRIT},{"addxw", LENW,READ,WRIT},{"addxl", LENL,READ,WRIT},

{"andb",  LENB,READ,WRIT},{"andw",  LENW,READ,WRIT},{"andl",  LENL,READ,WRIT},
{"andib", LENB,READ,WRIT},{"andiw", LENW,READ,WRIT},{"andil", LENL,READ,WRIT},

{"aslb",  LENB,READ,WRIT},{"aslw",  LENW,READ,WRIT},{"asll",  LENL,READ,WRIT},

{"bchg",  LENB,READ,WRIT},
{"bchgb", LENB,READ,WRIT},{"bchgw", LENW,READ,WRIT},{"bchgl", LENL,READ,WRIT},
{"bclr",  LENB,READ,WRIT},
{"bclrb", LENB,READ,WRIT},{"bclrw", LENW,READ,WRIT},{"bclrl", LENL,READ,WRIT},
{"bset",  LENB,READ,WRIT},
{"bsetb", LENB,READ,WRIT},{"bsetw", LENW,READ,WRIT},{"bsetl", LENL,READ,WRIT},
{"btst",  LENB,READ,READ},
{"btstb", LENB,READ,READ},{"btstw", LENW,READ,READ},{"btstl", LENL,READ,READ},

{"clrb",  LENB,WRIT,NONE},{"clrw",  LENW,WRIT,NONE},{"clrl",  LENL,WRIT,NONE},

{"cmpb",  LENB,READ,READ},{"cmpw",  LENW,READ,READ},{"cmpl",  LENL,READ,READ},
{"cmpab", LENB,READ,READ},{"cmpaw", LENW,READ,READ},{"cmpal", LENL,READ,READ},
{"cmpib", LENB,READ,READ},{"cmpiw", LENW,READ,READ},{"cmpil", LENL,READ,READ},
{"cmpmb", LENB,READ,READ},{"cmpmw", LENW,READ,READ},{"cmpml", LENL,READ,READ},

{"divs",  LENW,READ,WRIT},                          {"divsl", LENL,READ,WRIT},
{"divu",  LENW,READ,WRIT},                          {"divul", LENL,READ,WRIT},

{"eorb",  LENB,READ,WRIT},{"eorw",  LENW,READ,WRIT},{"eorl",  LENL,READ,WRIT},
{"eorib", LENB,READ,WRIT},{"eoriw", LENW,READ,WRIT},{"eoril", LENL,READ,WRIT},

{"lslb",  LENB,READ,WRIT},{"lslw",  LENW,READ,WRIT},{"lsll",  LENL,READ,WRIT},
{"lsrb",  LENB,READ,WRIT},{"lsrw",  LENW,READ,WRIT},{"lsrl",  LENL,READ,WRIT},

{"movb",  LENB,READ,WRIT},{"movw",  LENW,READ,WRIT},{"movl",  LENL,READ,WRIT},
{"movmb", LENB,READ,WRIT},{"movmw", LENW,READ,WRIT},{"movml", LENL,READ,WRIT},
{"movpb", LENB,READ,WRIT},{"movpw", LENW,READ,WRIT},{"movpl", LENL,READ,WRIT},
{"movq",  LENL,READ,WRIT},

{"moveb", LENB,READ,WRIT},{"movew", LENW,READ,WRIT},{"movel", LENL,READ,WRIT},
{"movemb",LENB,READ,WRIT},{"movemw",LENW,READ,WRIT},{"moveml",LENL,READ,WRIT},
{"movepb",LENB,READ,WRIT},{"movepw",LENW,READ,WRIT},{"movepl",LENL,READ,WRIT},
{"moveq", LENL,READ,WRIT},

{"muls",  LENW,READ,WRIT},                          {"mulsl", LENL,READ,WRIT},
{"mulu",  LENW,READ,WRIT},                          {"mulul", LENW,READ,WRIT},

{"nbcd",  LENB,WRIT,NONE},

{"negb",  LENB,WRIT,NONE},{"negw",  LENW,WRIT,NONE},{"negl",  LENL,WRIT,NONE},
{"negxb", LENB,WRIT,NONE},{"negxw", LENW,WRIT,NONE},{"negxl", LENL,WRIT,NONE},

{"notb",  LENB,WRIT,NONE},{"notw",  LENW,WRIT,NONE},{"notl",  LENL,WRIT,NONE},

{"orb",   LENB,READ,WRIT},{"orw",   LENW,READ,WRIT},{"orl",   LENL,READ,WRIT},
{"orib",  LENB,READ,WRIT},{"oriw",  LENW,READ,WRIT},{"oril",  LENL,READ,WRIT},

{"rolb",  LENB,READ,WRIT},{"rolw",  LENW,READ,WRIT},{"roll",  LENL,READ,WRIT},
{"roxlb", LENB,READ,WRIT},{"roxlw", LENW,READ,WRIT},{"roxll", LENL,READ,WRIT},

{"rorb",  LENB,READ,WRIT},{"rorw",  LENW,READ,WRIT},{"rorl",  LENL,READ,WRIT},
{"roxrb", LENB,READ,WRIT},{"roxrw", LENW,READ,WRIT},{"roxrl", LENL,READ,WRIT},

{"sbcd",  LENB,READ,WRIT},

{"shi",   LENB,WRIT,NONE},
{"sls",   LENB,WRIT,NONE},
{"scc",   LENB,WRIT,NONE},  
{"scs",   LENB,WRIT,NONE},  
{"sne",   LENB,WRIT,NONE},  
{"seq",   LENB,WRIT,NONE},  
{"svc",   LENB,WRIT,NONE},  
{"svs",   LENB,WRIT,NONE},  
{"spl",   LENB,WRIT,NONE},  
{"smi",   LENB,WRIT,NONE},  
{"sge",   LENB,WRIT,NONE},
{"slt",   LENB,WRIT,NONE},
{"sgt",   LENB,WRIT,NONE},
{"sle",   LENB,WRIT,NONE},

{"subb",  LENB,READ,WRIT},{"subw",  LENW,READ,WRIT},{"subl",  LENL,READ,WRIT},
{"subaw", LENW,READ,WRIT},{"subal", LENL,READ,WRIT},
{"subib", LENB,READ,WRIT},{"subiw", LENW,READ,WRIT},{"subil", LENL,READ,WRIT},
{"subqb", LENB,READ,WRIT},{"subqw", LENW,READ,WRIT},{"subql", LENL,READ,WRIT},
{"subxb", LENB,READ,WRIT},{"subxw", LENW,READ,WRIT},{"subxl", LENL,READ,WRIT},

{"tas",   LENB,WRIT,READ},

{"tstb",  LENB,READ,NONE},{"tstw",  LENW,READ,NONE},{"tstl",  LENL,READ,NONE},

{"pea", LENW,WRIT,NONE}, /* we assume that an address is pushed for writing */

{NULL,0,0,0}  /* sentinel */
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
void inst_init(void)
	{
	int len;
	INST *inst=tab_inst;
	
	for(len = 0; inst->name; ++inst,++len);
	qsort(tab_inst, tab_inst_len=len, sizeof(INST), (void*)inst_cmp);
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
		mid = (first+last) >> 1;
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
	static char ea[32];

	/* 
	 * check if arg is indirect mode 
	 */
	for(s=arg;*s && *s!='@';++s);
	ifn(*s) return NULL;

	/*
	 * grap base register
	 */
	for(t=reg,s=arg;*s!='@' && s<arg+8;*t++=*s++);
	*t='\0';

	if(!strcasecmp(reg,"pc")) return NULL;
        if(!strcasecmp(reg,basereg) && !test_basereg)  return NULL;
        if(!strcasecmp(reg,lstackreg) && !test_lstack) return NULL;
        if((!strcasecmp(reg,"sp") || !strcasecmp(reg,"a7")) && !test_stack)
              return NULL;

	/*
	 * grab ea
	 */
	for(t=ea,s=arg;*s;*t++=*s++);
	
	/* 
	 * clean dec/increment
	 */
	if(t[-1]=='-') --t;
	if(t[-1]=='+') --t;
	*t='\0';
	return ea;
	}

/*
 * Put code to check memory accesses.
 */
PRIVATE void do_put_acces(int len,int rw,char *arg)
	{
	char *ea;

	ifn(ea=get_ea(arg)) return;

	if(source_is) dump_lineno();

	/* patch for pre-decrementation */
	if(arg[strlen(arg)-1]=='-')
		fprintf(ofp,"\tpea %s(%d)\n",ea,-len);
	else	fprintf(ofp,"\tpea %s\n",ea);
	
	if(rw==READ)	fprintf(ofp,"\tjbsr AP_Read");
	else		fprintf(ofp,"\tjbsr AP_Write");

	fprintf(ofp,"%c\n",len==LENB?'B':len==LENW?'W':'L');
	}


/*
 * Test if current instruction references memory through indirection
 * and put special code in that case.
 */
void inst_chk(void)
	{
	INST *ip;
 
	ifn(ip = chk_opcode(opcode)) return;
	if(!strncasecmp(opcode,"movem",5)||!strncasecmp(opcode,"movm",4)) 
		{
		movem_chk(ip->len,arg1,arg2);
		return;
		}
        if(!test_pea && !strcasecmp(opcode,"pea")) return;
	if(ip->arg1!=NONE) do_put_acces(ip->len,ip->arg1,arg1);
	if(ip->arg2!=NONE) do_put_acces(ip->len,ip->arg2,arg2);
	}
