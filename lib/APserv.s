#
#	Interface C/ASM pour APur.lib - essai de re-ecriture a la syntaxe mit
#
# (c) by Samuel DEVULDER, 1995
#
	.text
	.even

	.globl	_StdAlloc
_StdAlloc:
	moveml	sp@(4),D0-D1
	movel	A6,sp@-
	movel	4:w,A6
	jbsr	0x0fffff0
	movel	sp@+,A6
	rts

	.globl	_AsmAllocMem
_AsmAllocMem:
	moveml	D1-D7/A0-A6,sp@-
	moveml	D0-D1,sp@-
	jbsr	_PubAllocMem
	addql	#8,sp
	moveml	sp@+,D1-D7/A0-A6
	tstl	d0
	rts

	.globl	_StdFree
_StdFree:
	movel	sp@(4),A1
	movel	sp@(8),D0
	movel	A6,sp@-
	movel	4:w,A6
	jbsr	0x0fffff0
	movel	sp@+,A6
	rts

	.globl	_AsmFreeMem
_AsmFreeMem:
	moveml	D1-D7/A0-A6,sp@-
	movel	D0,sp@-
	movel	A1,sp@-
	jbsr	_PubFreeMem
	addql	#8,sp
	moveml	sp@+,D1-D7/A0-A6
	rts

	.globl	AP_FuncStartup
AP_FuncStartup:
	movel	sp@(4),_AP_procname
	movel	sp@,sp@(4)
	addql	#4,sp
	rts

	.globl	AP_FuncStartup2
AP_FuncStartup2:
	movel	sp@(8),_AP_source
	movel	sp@(4),_AP_procname
	movel	sp@,sp@(8)
	addql	#8,sp
	rts

	.globl	AP_FuncCall
AP_FuncCall:
	movel	sp@,_AP_ret
	movel	sp@,_AP_pc
	moveml	D0-D7/A0-A6,sp@-
	jbsr	_AP_protect
	tstb	d0
	beq	err_funcall
	moveml	sp@+,D0-D7/A0-A6
	movel	sp@(4),sp@
	movel	#cont,sp@(4)
	rts
cont:	moveml  D0-D7/A0-A6,sp@-
	jbsr	_AP_unprotect
	moveml  sp@+,D0-D7/A0-A6
	movel	_AP_ret,sp@-
	rts
err_funcall:
	moveml  sp@+,D0-D7/A0-A6
	subqw	#4,sp
	movel	sp@(4),sp@
	movel	sp@(8),sp@(4)
	movel	sp@,sp@(8)
	addqw	#4,sp
	rts

	.globl	AP_Read
AP_Read:
	movel	(sp),_AP_pc
	moveml  D0-D7/A0-A6,sp@-
	moveml  sp@(64),D0/D1
	moveml  D0/D1,sp@-
	jbsr	_AP_rd
	addql	#8,sp
	moveml  sp@+,D0-D7/A0-A6
	movel	sp@,sp@(8)
	addql	#8,sp
	rts

	.globl	AP_ReadB
AP_ReadB:
	movel	(sp),_AP_pc
	moveml  D0-D7/A0-A6,sp@-
	movel	sp@(64),D0
	pea	1:w
	movel	D0,sp@-
	jbsr	_AP_rd
	addql	#8,sp
	moveml  sp@+,D0-D7/A0-A6
	movel	sp@,sp@(4)
	addql	#4,sp
	rts

	.globl	AP_ReadW
AP_ReadW:
	movel	(sp),_AP_pc
	moveml  D0-D7/A0-A6,sp@-
	movel	sp@(64),D0
	pea	2:w
	movel	D0,sp@-
	jbsr	_AP_rd
	addql	#8,sp
	moveml  sp@+,D0-D7/A0-A6
	movel	sp@,sp@(4)
	addql	#4,sp
	rts

	.globl	AP_ReadL
AP_ReadL:
	movel	(sp),_AP_pc
	moveml  D0-D7/A0-A6,sp@-
	movel	sp@(64),D0
	pea	4:w
	movel	D0,sp@-
	jbsr	_AP_rd
	addql	#8,sp
	moveml  sp@+,D0-D7/A0-A6
	movel	sp@,sp@(4)
	addql	#4,sp
	rts

	.globl	AP_Write
AP_Write:
	movel	(sp),_AP_pc
	moveml  D0-D7/A0-A6,sp@-
	moveml	sp@(64),D0/D1
	moveml  D0/D1,sp@-
	jbsr	_AP_wr
	addql	#8,sp
	moveml  sp@+,D0-D7/A0-A6
	movel	sp@,sp@(8)
	addql	#8,sp
	rts

	.globl	AP_WriteB
AP_WriteB:
	movel	(sp),_AP_pc
	moveml  D0-D7/A0-A6,sp@-
	movel	sp@(64),D0
	pea	1:w
	movel	D0,sp@-
	jbsr	_AP_wr
	addql	#8,sp
	moveml  sp@+,D0-D7/A0-A6
	movel	sp@,sp@(4)
	addql	#4,sp
	rts

	.globl	AP_WriteW
AP_WriteW:
	movel	(sp),_AP_pc
	moveml  D0-D7/A0-A6,sp@-
	movel	sp@(64),D0
	pea	2:w
	movel	D0,sp@-
	jbsr	_AP_wr
	addql	#8,sp
	moveml  sp@+,D0-D7/A0-A6
	movel	sp@,sp@(4)
	addql	#4,sp
	rts

	.globl	AP_WriteL
AP_WriteL:
	movel	(sp),_AP_pc
	moveml  D0-D7/A0-A6,sp@-
	moveml  sp@(64),D0
	pea	4:w
	movel	D0,sp@-
	jbsr	_AP_wr
	addql	#8,sp
	moveml  sp@+,D0-D7/A0-A6
	movel	sp@,sp@(4)
	addql	#4,sp
	rts
