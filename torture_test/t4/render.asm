	section text,code

	debug	17
; void init_render(context *c)

	ds.l 0
	procstart
	xdef	_init_render
_init_render:
	movem.l	l7,-(sp)
	move.l	l6-4(A7),A3
	link	A5,#-l4
	debug	21
; 
; {
;     struct Rectangle r;
;     
;     c->rport=c->wnd->RPort;

	move.l	14(A3),A0
	move.l	50(A0),34(A3)
	debug	22
; 
;     c->curfont=(c->deffont=c->rport->Font);

	move.l	34(A3),A0
	move.l	52(A0),42(A3)
	move.l	42(A3),38(A3)
	debug	23
; 
;     c->newind=0;

	moveq.l	#0,D0
	move.l	D0,74(A3)
	debug	24
; 
;     c->x=c->wnd->BorderLeft; 

	move.l	14(A3),A0
	move.b	54(A0),D0
	ext.w	D0
	ext.l	D0
	move.l	D0,54(A3)
	debug	25
; 
;     c->y=c->wnd->BorderTop;

	move.l	14(A3),A0
	move.b	55(A0),D0
	ext.w	D0
	ext.l	D0
	move.l	D0,58(A3)
	debug	26
; 
;     c->newjust=JUST_CENTER;

	moveq.l	#2,D0
	move.l	D0,82(A3)
	debug	27
; 
;     SetAPen(c->rport, 0);

	clr.l	-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetAPen
	addq.l	#8,sp
	debug	29
; 
;     RectFill(c->rport, c->wnd->BorderLeft, c->wnd->BorderTop,
;             c->wnd->BorderLeft+c->wx, c->wnd->BorderTop+c->wy);

	move.l	14(A3),A0
	move.b	55(A0),D0
	ext.w	D0
	ext.l	D0
	add.l	30(A3),D0
	move.l	D0,-(sp)
	move.l	14(A3),A0
	move.b	54(A0),D0
	ext.w	D0
	ext.l	D0
	add.l	26(A3),D0
	move.l	D0,-(sp)
	move.l	14(A3),A0
	move.b	55(A0),D0
	ext.w	D0
	ext.l	D0
	move.l	D0,-(sp)
	move.l	14(A3),A0
	move.b	54(A0),D0
	ext.w	D0
	ext.l	D0
	move.l	D0,-(sp)
	move.l	34(A3),-(sp)
	jsr	_RectFill
	lea	20(sp),sp
	debug	30
; 
;     SetAPen(c->rport, (c->curfg=1));

	move.w	#$01,88(A3)
	moveq.l	#0,D0
	move.w	88(A3),D0
	move.l	D0,-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetAPen
	addq.l	#8,sp
	debug	31
; 
;     SetBPen(c->rport, (c->curbg=0));

	move.w	#$00,90(A3)
	moveq.l	#0,D0
	move.w	90(A3),D0
	move.l	D0,-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetBPen
	addq.l	#8,sp
	debug	32
; 
;     SetDrMd(c->rport, 1);

	pea	1.W
	move.l	34(A3),-(sp)
	jsr	_SetDrMd
	addq.l	#8,sp
	debug	33
; 
;     SetSoftStyle(c->rport, (c->curstyle=0), 7);

	pea	7.W
	move.w	#$00,86(A3)
	moveq.l	#0,D0
	move.w	86(A3),D0
	move.l	D0,-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetSoftStyle
	lea	12(sp),sp
	debug	34
; 
;     c->layer=c->rport->Layer;

	move.l	34(A3),A0
	move.l	(A0),46(A3)
	debug	35
; 
;     c->region=NewRegion();

	jsr	_NewRegion
	move.l	D0,50(A3)
	debug	36
; 
;     if (c->region)

	tst.l	50(A3)
	beq	l2
l1
	debug	38
; 
;     {
;         r.MinX=c->wnd->BorderLeft;

	move.l	14(A3),A0
	move.b	54(A0),D0
	ext.w	D0
	move.w	D0,-8(A5)
	debug	39
; 
;         r.MaxX=c->wnd->BorderLeft+c->wx-1;

	move.l	14(A3),A0
	move.b	54(A0),D0
	ext.w	D0
	move.w	28(A3),D1
	add.w	D0,D1
	subq.w	#$01,D1
	move.w	D1,-4(A5)
	debug	40
; 
;         r.MinY=c->wnd->BorderTop;

	move.l	14(A3),A0
	move.b	55(A0),D1
	ext.w	D1
	move.w	D1,-6(A5)
	debug	41
; 
;         r.MaxY=c->wnd->BorderTop+c->wy-1;

	move.l	14(A3),A0
	move.b	55(A0),D1
	ext.w	D1
	move.w	32(A3),D0
	add.w	D1,D0
	subq.w	#$01,D0
	move.w	D0,-2(A5)
	debug	42
; 
;         OrRectRegion(c->region, &r);

	pea.l	-8(A5)
	move.l	50(A3),-(sp)
	jsr	_OrRectRegion
	addq.l	#8,sp
	debug	43
; 
;         InstallClipRegion(c->layer, c->region);

	move.l	50(A3),-(sp)
	move.l	46(A3),-(sp)
	jsr	_InstallClipRegion
	addq.l	#8,sp
l2
l3
	debug	45
; 
;     }
;     NewList(&c->renderlist);

	pea.l	92(A3)
	jsr	_NewList
	addq.l	#4,sp
l5
	unlk	A5
	move.l	(sp)+,A3
	rts
l7	reg	A3
l6	equ	12
l4	equ	8
	procend
	debug	46
; 
; }

	debug	48
; 
; 
; void close_render(context *c)

	procstart
	xdef	_close_render
_close_render:
	movem.l	l15,-(sp)
	move.l	l14-4(A7),A3
	link	A5,#-l12
	debug	50
; 
; {
;     if (c->region)

	tst.l	50(A3)
	beq	l10
l9
	debug	52
; 
;     {   
;         InstallClipRegion(c->layer, 0L);

	clr.l	-(sp)
	move.l	46(A3),-(sp)
	jsr	_InstallClipRegion
	addq.l	#8,sp
	debug	53
; 
;         DisposeRegion(c->region);

	move.l	50(A3),-(sp)
	jsr	_DisposeRegion
	addq.l	#4,sp
l10
l11
	debug	55
; 
;     }
;     c->region=0L;

	moveq.l	#0,D0
	move.l	D0,50(A3)
l13
	unlk	A5
	move.l	(sp)+,A3
	rts
l15	reg	A3
l14	equ	12
l12	equ	0
	procend
	debug	56
; 
; }

	debug	58
; 
; 
; void line_reset(context *c)

	procstart
	xdef	_line_reset
_line_reset:
	movem.l	l23,-(sp)
	move.l	l22-4(A7),A3
	link	A5,#-l20
	debug	60
; 
; {
;     c->curline=(renline *)calloc(sizeof(renline), 1);

	pea	1.W
	pea	44.W
	jsr	_calloc
	addq.l	#8,sp
	move.l	D0,106(A3)
	debug	61
; 
;     if (c->curline)

	tst.l	106(A3)
	beq	l18
l17
	debug	65
; 
;     {
;         renline *r=c->curline;
; 
;         AddTail(&c->renderlist, (struct Node *)r);

	move.l	106(A3),A2
	move.l	A2,-(sp)
	pea.l	92(A3)
	jsr	_AddTail
	addq.l	#8,sp
	debug	66
; 
;         NewList(&r->linelist);

	pea.l	30(A2)
	jsr	_NewList
	addq.l	#4,sp
	debug	68
; 
;         
;         r->h=(c->h=c->curfont->tf_YSize);

	move.l	38(A3),A0
	moveq.l	#0,D0
	move.w	20(A0),D0
	move.l	D0,62(A3)
	move.w	64(A3),18(A2)
	debug	69
; 
;         c->hb=c->curfont->tf_Baseline;

	move.l	38(A3),A0
	moveq.l	#0,D0
	move.w	26(A0),D0
	move.l	D0,66(A3)
	debug	70
; 
;         SetFont(c->rport,c->curfont);

	move.l	38(A3),-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetFont
	addq.l	#8,sp
	debug	71
; 
;         SetSoftStyle(c->rport, c->curstyle, 7);

	pea	7.W
	moveq.l	#0,D0
	move.w	86(A3),D0
	move.l	D0,-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetSoftStyle
	lea	12(sp),sp
	debug	72
; 
;         SetAPen(c->rport, c->curfg);

	moveq.l	#0,D0
	move.w	88(A3),D0
	move.l	D0,-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetAPen
	addq.l	#8,sp
	debug	73
; 
;         SetBPen(c->rport, c->curbg);

	moveq.l	#0,D0
	move.w	90(A3),D0
	move.l	D0,-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetBPen
	addq.l	#8,sp
	debug	74
; 
;         r->justification=c->newjust;

	move.l	82(A3),22(A2)
	debug	75
; 
;         r->ind  =   c->newind;

	move.l	74(A3),26(A2)
	debug	76
; 
;         c->x    =   c->newind;  

	move.l	74(A3),54(A3)
	debug	77
; 
;         c->atstart=1;

	moveq.l	#1,D0
	move.l	D0,78(A3)
l18
l19
l21
	unlk	A5
	movem.l	(sp)+,A2/A3
	rts
l23	reg	A2/A3
l22	equ	16
l20	equ	0
	procend
	debug	79
; 
;     }
; }

	debug	81
; 
; 
; void calculate_coordinates(context *c, block *b, int pos, UWORD *x, UWORD *y, UWORD *h, UWORD *ay)

	procstart
	xdef	_calculate_coordinates
_calculate_coordinates:
	movem.l	l52,-(sp)
	move.l	l51-4(A7),A3
	move.l	l51+4(A7),D7
	link	A5,#-l49
	debug	87
; 
; 
; 
; 
; {
;     UWORD wx=0,wy=0,wh=0,way=0;
;     int done=0;   

	moveq.l	#0,D6
	move.w	#$00,-2(A5)
	moveq.l	#0,D5
	moveq.l	#0,D4
	debug	89
; 
; 
;     while(b->node.ln_Succ && b->type!=BLK_TEXT) b=(block *)b->node.ln_Succ;

	moveq.l	#0,D3
	bra	l26
l25
	move.l	l51+4(A5),A0
	move.l	(A0),l51+4(A5)
l26
	move.l	l51+4(A5),A0
	tst.l	(A0)
	beq	l108
	move.l	l51+4(A5),A0
	tst.l	14(A0)
	bne	l25
l109
l108
l27
	debug	90
; 
;     if (b->node.ln_Succ)

	move.l	l51+4(A5),A0
	tst.l	(A0)
	beq	l29
l28
	debug	92
; 
;     {
;         if (pos<b->length)

	move.l	l51+4(A5),A0
	cmp.l	22(A0),D7
	bcc	l32
l110
l31
	debug	96
; 
;         {
;             renline *r;
; 
;             r=(renline *)c->renderlist.lh_Head;

	move.l	92(A3),A2
	debug	97
; 
;             while(r->node.ln_Succ && !done)

	bra	l35
l34
	debug	101
; 
;             {
;                 renitem *ri;
;                 
;                 wy=r->y; way=r->cy;

	move.w	14(A2),-2(A5)
	move.w	20(A2),D4
	debug	102
; 
;                 wh=r->h;

	move.w	18(A2),D5
	debug	103
; 
;                 wx=r->x;

	move.w	16(A2),D6
	debug	104
; 
;                 ri=(renitem *)r->linelist.lh_Head;

	move.l	30(A2),A6
	debug	105
; 
;                 while(ri->node.ln_Succ)

	bra	l38
l37
	debug	107
; 
;                 {
;                     if (ri->textblock!=b)

	move.l	18(A6),D0
	cmp.l	l51+4(A5),D0
	beq	l41
l111
l40
	bra	l38
	bra	l42
l41
	debug	111
; 
;                         continue;
;                     else
;                     {
;                         if ((ri->pos<=pos) && (ri->pos+ri->len>pos))

	cmp.l	22(A6),D7
	blt	l44
l113
	move.l	22(A6),D0
	add.l	30(A6),D0
	cmp.l	D0,D7
	bge	l44
l114
l112
l43
	debug	115
; 
;                         {
;                             UWORD nl;
; 
;                             nl=pos-ri->pos;

	move.w	D7,D0
	move.w	24(A6),D1
	move.w	D0,D2
	sub.w	D1,D2
	debug	116
; 
;                             if (nl>0)

	tst.w	D2
	bls	l47
l115
l46
	debug	118
; 
;                             {
;                                 SetFont(c->rport, ri->font);

	move.l	14(A6),-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetFont
	addq.l	#8,sp
	debug	119
; 
;                                 SetSoftStyle(c->rport, ri->style, 7);

	pea	7.W
	moveq.l	#0,D1
	move.w	38(A6),D1
	move.l	D1,-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetSoftStyle
	lea	12(sp),sp
	debug	120
; 
;                                 wx=TextLength(c->rport, ri->text, nl);

	moveq.l	#0,D0
	move.w	D2,D0
	move.l	D0,-(sp)
	move.l	26(A6),-(sp)
	move.l	34(A3),-(sp)
	jsr	_TextLength
	lea	12(sp),sp
	move.w	D0,D6
	debug	121
; 
;                                 SetFont(c->rport, c->curfont);

	move.l	38(A3),-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetFont
	addq.l	#8,sp
	debug	122
; 
;                                 SetSoftStyle(c->rport, c->curstyle, 7);

	pea	7.W
	moveq.l	#0,D0
	move.w	86(A3),D0
	move.l	D0,-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetSoftStyle
	lea	12(sp),sp
l47
l48
	debug	124
; 
;                             } 
;                             done=1;

	moveq.l	#1,D3
	debug	126
; 
;                             break;
;                         }    

	bra	l39
l44
l45
l42
l38
	tst.l	(A6)
	bne	l37
l39
l35
	tst.l	(A2)
	beq	l116
	tst.l	D3
	beq	l34
l116
l36
l32
l33
l29
l30
	debug	132
; 
;                     }    
;                 }    
;             }    
;         }    
;     }    
;     *x=wx; *y=wy; *h=wh; *ay=way;

	move.l	l51+12(A5),A2
	move.w	D6,(A2)
	move.l	l51+16(A5),A2
	move.w	-2(A5),(A2)
	move.l	l51+20(A5),A2
	move.w	D5,(A2)
	move.l	l51+24(A5),A2
	move.w	D4,(A2)
l50
	unlk	A5
	movem.l	(sp)+,D2/D3/D4/D5/D6/D7/A2/A3/A6
	rts
l52	reg	D2/D3/D4/D5/D6/D7/A2/A3/A6
l51	equ	44
l49	equ	4
	procend
	debug	133
; 
; }

	debug	135
; 
; 
; void finish_line(context *c)

	procstart
	xdef	_finish_line
_finish_line:
	movem.l	l129,-(sp)
	move.l	l128-4(A7),A3
	link	A5,#-l126
	debug	139
; 
; {
;     renline *r;
; 
;     r=c->curline;

	move.l	106(A3),A2
	debug	140
; 
;     if (r->justification==JUST_LEFT)

	tst.l	22(A2)
	bne	l118
l117
	debug	141
; 
;         r->x=c->wnd->BorderLeft+r->ind;

	move.l	14(A3),A0
	move.b	54(A0),D0
	ext.w	D0
	move.w	28(A2),D1
	add.w	D1,D0
	move.w	D0,16(A2)
	bra	l119
l118
	debug	142
; 
;     else if (r->justification==JUST_RIGHT)

	moveq.l	#1,D1
	cmp.l	22(A2),D1
	bne	l121
l131
l120
	debug	143
; 
;         r->x=c->wnd->BorderLeft+r->ind+c->wx-c->x;

	move.l	14(A3),A0
	move.b	54(A0),D1
	ext.w	D1
	ext.l	D1
	add.l	26(A2),D1
	move.w	28(A3),D0
	add.w	D1,D0
	move.w	56(A3),D1
	sub.w	D1,D0
	move.w	D0,16(A2)
	bra	l122
l121
	debug	144
; 
;     else if (r->justification==JUST_CENTER)

	moveq.l	#2,D1
	cmp.l	22(A2),D1
	bne	l124
l132
l123
	debug	149
; 
;     {
;         
;         
;         
;         r->x=c->wnd->BorderLeft+r->ind+(c->wx-(c->x)-1)/2;    

	move.l	14(A3),A0
	move.b	54(A0),D1
	ext.w	D1
	ext.l	D1
	add.l	26(A2),D1
	move.l	D1,D2
	move.l	26(A3),D1
	sub.l	54(A3),D1
	subq.l	#$01,D1
	move.l	D1,D0
	moveq.l	#2,D1
	jsr	__divs
	move.l	D0,D1
	move.w	D2,D0
	add.w	D1,D0
	move.w	D0,16(A2)
l124
l125
l122
l119
	debug	151
; 
;     }
;     r->y=c->y+c->hb;

	move.w	60(A3),D1
	move.w	68(A3),D0
	add.w	D0,D1
	move.w	D1,14(A2)
	debug	152
; 
;     r->cy=c->y+c->h;

	move.w	60(A3),D0
	move.w	64(A3),D1
	add.w	D1,D0
	move.w	D0,20(A2)
	debug	153
; 
;     c->y+=c->h;

	move.l	62(A3),D1
	add.l	D1,58(A3)
	debug	154
; 
;     r->h=c->h;

	move.w	64(A3),18(A2)
	debug	155
; 
;     line_reset(c);

	move.l	A3,-(sp)
	jsr	_line_reset
	addq.l	#4,sp
l127
	unlk	A5
	movem.l	(sp)+,D2/A2/A3
	rts
l129	reg	D2/A2/A3
l128	equ	20
l126	equ	0
	procend
	debug	156
; 
; }

	debug	158
; 
; 
; void render_line(context *c, renline *rl)

	procstart
	xdef	_render_line
_render_line:
	movem.l	l139,-(sp)
	move.l	l138-4(A7),A3
	move.l	l138+0(A7),A2
	link	A5,#-l136
	debug	162
; 
; {
;     renitem *r;
;     
;     Move(c->rport, rl->x, rl->y);

	moveq.l	#0,D0
	move.w	14(A2),D0
	move.l	D0,-(sp)
	moveq.l	#0,D0
	move.w	16(A2),D0
	move.l	D0,-(sp)
	move.l	34(A3),-(sp)
	jsr	_Move
	lea	12(sp),sp
	debug	163
; 
;     r=(renitem *)rl->linelist.lh_Head;

	move.l	30(A2),A6
	debug	164
; 
;     while (r->node.ln_Succ)

	bra	l134
l133
	debug	166
; 
;     {
;         SetFont(c->rport, r->font);

	move.l	14(A6),-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetFont
	addq.l	#8,sp
	debug	167
; 
;         SetSoftStyle(c->rport, r->style, 7);

	pea	7.W
	moveq.l	#0,D0
	move.w	38(A6),D0
	move.l	D0,-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetSoftStyle
	lea	12(sp),sp
	debug	168
; 
;         SetAPen(c->rport, r->fg); SetBPen(c->rport, r->bg);

	moveq.l	#0,D0
	move.w	34(A6),D0
	move.l	D0,-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetAPen
	addq.l	#8,sp
	moveq.l	#0,D0
	move.w	36(A6),D0
	move.l	D0,-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetBPen
	addq.l	#8,sp
	debug	169
; 
;         Text(c->rport, r->text, r->len);

	move.l	30(A6),-(sp)
	move.l	26(A6),-(sp)
	move.l	34(A3),-(sp)
	jsr	_Text
	lea	12(sp),sp
	debug	170
; 
;         r=(renitem *)r->node.ln_Succ;

	move.l	(A6),A6
l134
	tst.l	(A6)
	bne	l133
l135
l137
	unlk	A5
	movem.l	(sp)+,A2/A3/A6
	rts
l139	reg	A2/A3/A6
l138	equ	20
l136	equ	0
	procend
	debug	172
; 
;     }
; }

	debug	174
; 
; 
; void render_caret(context *c)

	procstart
	xdef	_render_caret
_render_caret:
	movem.l	l144,-(sp)
	move.l	l143-4(A7),A3
	link	A5,#-l141
	debug	176
; 
; {
;     c->cursoractive=!c->cursoractive;

	moveq.l	#0,D0
	tst.w	146(A3)
	seq.b	D0
	neg.b	D0
	move.w	D0,146(A3)
	debug	177
; 
;     SetDrMd(c->rport, 2);

	pea	2.W
	move.l	34(A3),-(sp)
	jsr	_SetDrMd
	addq.l	#8,sp
	debug	178
; 
;     Move(c->rport, c->cx, c->cy);

	moveq.l	#0,D0
	move.w	142(A3),D0
	move.l	D0,-(sp)
	moveq.l	#0,D0
	move.w	140(A3),D0
	move.l	D0,-(sp)
	move.l	34(A3),-(sp)
	jsr	_Move
	lea	12(sp),sp
	debug	179
; 
;     Draw(c->rport, c->cx, c->cy-c->ch);

	moveq.l	#0,D0
	move.w	142(A3),D0
	moveq.l	#0,D1
	move.w	144(A3),D1
	neg.l	D1
	add.l	D0,D1
	move.l	D1,-(sp)
	moveq.l	#0,D1
	move.w	140(A3),D1
	move.l	D1,-(sp)
	move.l	34(A3),-(sp)
	jsr	_Draw
	lea	12(sp),sp
	debug	180
; 
;     Move(c->rport, c->cx+1, c->cy);

	moveq.l	#0,D1
	move.w	142(A3),D1
	move.l	D1,-(sp)
	moveq.l	#0,D1
	move.w	140(A3),D1
	addq.l	#$01,D1
	move.l	D1,-(sp)
	move.l	34(A3),-(sp)
	jsr	_Move
	lea	12(sp),sp
	debug	181
; 
;     Draw(c->rport, c->cx+1, c->cy-c->ch);

	moveq.l	#0,D1
	move.w	142(A3),D1
	moveq.l	#0,D0
	move.w	144(A3),D0
	neg.l	D0
	add.l	D1,D0
	move.l	D0,-(sp)
	moveq.l	#0,D0
	move.w	140(A3),D0
	addq.l	#$01,D0
	move.l	D0,-(sp)
	move.l	34(A3),-(sp)
	jsr	_Draw
	lea	12(sp),sp
	debug	182
; 
;     SetDrMd(c->rport, 1);

	pea	1.W
	move.l	34(A3),-(sp)
	jsr	_SetDrMd
	addq.l	#8,sp
l142
	unlk	A5
	move.l	(sp)+,A3
	rts
l144	reg	A3
l143	equ	12
l141	equ	0
	procend
	debug	183
; 
; }

	debug	185
; 
; 
; void move_caret(context *c, block *b, int pos)

	procstart
	xdef	_move_caret
_move_caret:
	movem.l	l152,-(sp)
	move.l	l151-4(A7),A3
	move.l	l151+0(A7),A2
	move.l	l151+4(A7),D7
	link	A5,#-l149
	debug	188
; 
; {
;     UWORD dummy;
;     if (c->cursoractive) render_caret(c);

	tst.w	146(A3)
	beq	l147
l146
	move.l	A3,-(sp)
	jsr	_render_caret
	addq.l	#4,sp
l147
l148
	debug	189
; 
;     calculate_coordinates(c, b, pos, &c->cx, &dummy, &c->ch, &c->cy);

	pea.l	142(A3)
	pea.l	144(A3)
	pea.l	-2(A5)
	pea.l	140(A3)
	move.l	D7,-(sp)
	move.l	A2,-(sp)
	move.l	A3,-(sp)
	jsr	_calculate_coordinates
	lea	28(sp),sp
	debug	190
; 
;     render_caret(c);

	move.l	A3,-(sp)
	jsr	_render_caret
	addq.l	#4,sp
l150
	unlk	A5
	movem.l	(sp)+,D7/A2/A3
	rts
l152	reg	D7/A2/A3
l151	equ	20
l149	equ	4
	procend
	debug	191
; 
; }

	debug	193
; 
; 
; void remove_caret(context *c)

	procstart
	xdef	_remove_caret
_remove_caret:
	movem.l	l160,-(sp)
	move.l	l159-4(A7),A3
	link	A5,#-l157
	debug	195
; 
; { 
;     if (c->cursoractive) render_caret(c);

	tst.w	146(A3)
	beq	l155
l154
	move.l	A3,-(sp)
	jsr	_render_caret
	addq.l	#4,sp
l155
l156
	debug	196
; 
;     c->cursoractive=0;

	move.w	#$00,146(A3)
l158
	unlk	A5
	move.l	(sp)+,A3
	rts
l160	reg	A3
l159	equ	12
l157	equ	0
	procend
	debug	197
; 
; }

	debug	199
; 
; 
; void start_caret(context *c)

	procstart
	xdef	_start_caret
_start_caret:
	movem.l	l165,-(sp)
	move.l	l164-4(A7),A3
	link	A5,#-l162
	debug	201
; 
; {
;     c->cursorcount=6;

	move.w	#$06,148(A3)
	debug	202
; 
;     c->cursoractive=1;

	move.w	#$01,146(A3)
	debug	203
; 
;      render_caret(c);

	move.l	A3,-(sp)
	jsr	_render_caret
	addq.l	#4,sp
l163
	unlk	A5
	move.l	(sp)+,A3
	rts
l165	reg	A3
l164	equ	12
l162	equ	0
	procend
	debug	204
; 
; }

	debug	206
; 
;     
; void blink_caret(context *c)

	procstart
	xdef	_blink_caret
_blink_caret:
	movem.l	l173,-(sp)
	move.l	l172-4(A7),A3
	link	A5,#-l170
	debug	208
; 
; {
;     if (!c->cursorcount)

	tst.w	148(A3)
	bne	l168
l167
	debug	210
; 
;     {
;         render_caret(c);

	move.l	A3,-(sp)
	jsr	_render_caret
	addq.l	#4,sp
	debug	211
; 
;         c->cursorcount=6;

	move.w	#$06,148(A3)
l168
l169
	debug	213
; 
;     }
;     c->cursorcount--;

	subq.w	#$01,148(A3)
l171
	unlk	A5
	move.l	(sp)+,A3
	rts
l173	reg	A3
l172	equ	12
l170	equ	0
	procend
	debug	214
; 
; }

	debug	216
; 
; 
; void render_context(context *c)

	procstart
	xdef	_render_context
_render_context:
	movem.l	l181,-(sp)
	move.l	l180-4(A7),A3
	link	A5,#-l178
	debug	220
; 
; {
;     renline *r;
; 
;     r=(renline *)c->renderlist.lh_Head;

	move.l	92(A3),A2
	debug	221
; 
;     while (r->node.ln_Succ)

	bra	l176
l175
	debug	223
; 
;     {
;         render_line(c,r);

	move.l	A2,-(sp)
	move.l	A3,-(sp)
	jsr	_render_line
	addq.l	#8,sp
	debug	224
; 
;         r=(renline *)r->node.ln_Succ;

	move.l	(A2),A2
l176
	tst.l	(A2)
	bne	l175
l177
	debug	226
; 
;     }
;     c->curline=0L;

	moveq.l	#0,D0
	move.l	D0,106(A3)
l179
	unlk	A5
	movem.l	(sp)+,A2/A3
	rts
l181	reg	A2/A3
l180	equ	16
l178	equ	0
	procend
	debug	227
; 
; }

	debug	229
; 
; 
; void free_linelist(renline *r)

	procstart
	xdef	_free_linelist
_free_linelist:
	movem.l	l189,-(sp)
	move.l	l188-4(A7),A3
	link	A5,#-l186
	debug	231
; 
; {
;     while (!(( ((&r->linelist)->lh_TailPred) == (struct Node *)(&r->linelist) )))

	bra	l184
l183
	debug	234
; 
;     {
;         renitem *ri=(renitem *)r->linelist.lh_Head;
;         Remove(ri);

	move.l	30(A3),A2
	move.l	A2,-(sp)
	jsr	_Remove
	addq.l	#4,sp
	debug	235
; 
;         free(ri);

	move.l	A2,-(sp)
	jsr	_free
	addq.l	#4,sp
l184
	lea.l	30(A3),A2
	cmp.l	38(A3),A2
	bne	l183
l191
l185
l187
	unlk	A5
	movem.l	(sp)+,A2/A3
	rts
l189	reg	A2/A3
l188	equ	16
l186	equ	0
	procend
	debug	237
; 
;     }
; }

	debug	239
; 
; 
; void free_renderlist(context *c)

	procstart
	xdef	_free_renderlist
_free_renderlist:
	movem.l	l198,-(sp)
	move.l	l197-4(A7),A3
	link	A5,#-l195
	debug	243
; 
; {
;     renline *r;
; 
;     while (!(( ((&c->renderlist)->lh_TailPred) == (struct Node *)(&c->renderlist) )))

	bra	l193
l192
	debug	245
; 
;     {
;         r=(renline *)c->renderlist.lh_Head;

	move.l	92(A3),A2
	debug	246
; 
;         Remove(r);

	move.l	A2,-(sp)
	jsr	_Remove
	addq.l	#4,sp
	debug	247
; 
;         free_linelist(r);

	move.l	A2,-(sp)
	jsr	_free_linelist
	addq.l	#4,sp
	debug	248
; 
;         free(r);

	move.l	A2,-(sp)
	jsr	_free
	addq.l	#4,sp
l193
	lea.l	92(A3),A0
	cmp.l	100(A3),A0
	bne	l192
l200
l194
l196
	unlk	A5
	movem.l	(sp)+,A2/A3
	rts
l198	reg	A2/A3
l197	equ	16
l195	equ	0
	procend
	debug	250
; 
;     }
; }

	debug	252
; 
; 
; void addtorenlist(context *c, block *b, int pos, char *text, int len, int iswrapped)

	procstart
	xdef	_addtorenlist
_addtorenlist:
	movem.l	l213,-(sp)
	move.l	l212-4(A7),A3
	move.l	l212+4(A7),D7
	move.l	l212+12(A7),D6
	move.l	l212+16(A7),D5
	link	A5,#-l210
	debug	257
; 
; {
;     renitem *r;
;     renline *rl;
; 
;     c->atstart=0;

	moveq.l	#0,D0
	move.l	D0,78(A3)
	debug	258
; 
;     rl=c->curline;

	move.l	106(A3),A6
	debug	259
; 
;     if (c->h<c->curfont->tf_YSize) c->h=c->curfont->tf_YSize; 

	move.l	38(A3),A0
	moveq.l	#0,D0
	move.w	20(A0),D0
	cmp.l	62(A3),D0
	bls	l202
l217
l201
	move.l	38(A3),A0
	moveq.l	#0,D0
	move.w	20(A0),D0
	move.l	D0,62(A3)
l202
l203
	debug	260
; 
;     if (c->hb<c->curfont->tf_Baseline) c->hb=c->curfont->tf_Baseline;

	move.l	38(A3),A0
	moveq.l	#0,D0
	move.w	26(A0),D0
	cmp.l	66(A3),D0
	bls	l205
l218
l204
	move.l	38(A3),A0
	moveq.l	#0,D0
	move.w	26(A0),D0
	move.l	D0,66(A3)
l205
l206
	debug	261
; 
;     r=(renitem *)calloc(sizeof(renitem), 1);

	pea	1.W
	pea	40.W
	jsr	_calloc
	addq.l	#8,sp
	move.l	D0,A2
	debug	262
; 
;     AddTail(&rl->linelist, r);

	move.l	A2,-(sp)
	pea.l	30(A6)
	jsr	_AddTail
	addq.l	#8,sp
	debug	263
; 
;     r->textblock=b;

	move.l	l212+4(A5),18(A2)
	debug	264
; 
;     r->pos=pos;

	move.l	D7,22(A2)
	debug	265
; 
;     r->text=text;

	move.l	l212+12(A5),26(A2)
	debug	266
; 
;     r->len=len;

	move.l	D6,30(A2)
	debug	267
; 
;     r->font=c->curfont;

	move.l	38(A3),14(A2)
	debug	268
; 
;     r->style=c->curstyle;

	move.w	86(A3),38(A2)
	debug	269
; 
;     r->bg=c->curbg; r->fg=c->curfg;

	move.w	90(A3),36(A2)
	move.w	88(A3),34(A2)
	debug	270
; 
;     c->x+=TextLength(c->rport, text, len);

	move.l	D6,-(sp)
	move.l	l212+12(A5),-(sp)
	move.l	34(A3),-(sp)
	jsr	_TextLength
	lea	12(sp),sp
	ext.l	D0
	add.l	D0,54(A3)
	debug	271
; 
;     if (iswrapped)

	tst.l	D5
	beq	l208
l207
	debug	273
; 
;     {    
;         finish_line(c);

	move.l	A3,-(sp)
	jsr	_finish_line
	addq.l	#4,sp
l208
l209
l211
	unlk	A5
	movem.l	(sp)+,D5/D6/D7/A2/A3/A6
	rts
l213	reg	D5/D6/D7/A2/A3/A6
l212	equ	32
l210	equ	0
	procend
	debug	275
; 
;     } 
; }

	debug	277
; 
; 
; void render_text(context *c, char *text, int len, int pos, block *b)

	procstart
	xdef	_render_text
_render_text:
	movem.l	l264,-(sp)
	move.l	l263-4(A7),A3
	move.l	l263+0(A7),A2
	move.l	l263+4(A7),D7
	move.l	l263+8(A7),D6
	move.l	l263+12(A7),A6
	link	A5,#-l261
	debug	281
; 
; {
;     int skip=0;
; 
;     if (!text || !len)

	moveq.l	#0,D5
	move.l	A2,D0
	beq	l266
	tst.l	D7
	bne	l220
l266
l219
	bra	l262
l220
l221
	debug	283
; 
;         return;
;     while(pos<len && c->y<c->wy+c->wnd->BorderTop-1)

	bra	l223
l222
	debug	288
; 
;     {
;         int tf;
;         struct TextExtent te1;
; 
;         tf=TextFit(c->rport,   &text[pos], (UWORD)(len-pos), &te1, 

	move.l	38(A3),A0
	moveq.l	#0,D0
	move.w	20(A0),D0
	move.l	D0,-(sp)
	move.w	28(A3),D0
	move.w	56(A3),D1
	neg.w	D1
	add.w	D0,D1
	subq.w	#$01,D1
	andi.l	#$FFFF,D1
	move.l	D1,-(sp)
	pea	1.W
	clr.l	-(sp)
	pea.l	-12(A5)
	move.w	D7,D0
	move.w	D6,D1
	neg.w	D1
	add.w	D0,D1
	andi.l	#$FFFF,D1
	move.l	D1,-(sp)
	pea.l	0(A2,D6.L)
	move.l	34(A3),-(sp)
	jsr	_TextFit
	lea	32(sp),sp
	move.l	D0,D4
	debug	291
; 
;                             0L, (UWORD)1, (UWORD)(c->wx-c->x-1),
;                             (UWORD)c->curfont->tf_YSize);
;         if (tf+pos==len)                        

	move.l	D4,D0
	add.l	D6,D0
	cmp.l	D0,D7
	bne	l226
l267
l225
	debug	293
; 
;         {
;             addtorenlist(c, b, pos, &text[pos], tf, 0);

	clr.l	-(sp)
	move.l	D4,-(sp)
	pea.l	0(A2,D6.L)
	move.l	D6,-(sp)
	move.l	A6,-(sp)
	move.l	A3,-(sp)
	jsr	_addtorenlist
	lea	24(sp),sp
	debug	294
; 
;             pos+=tf;                            

	add.l	D4,D6
	bra	l227
l226
	debug	295
; 
;         } else if (tf)

	tst.l	D4
	beq	l229
l228
	debug	299
; 
;         {
;             int tp=tf, tl=tf;
; 
;             if (text[pos+tp-1]==' ')    

	move.l	D4,D3
	move.l	D4,D2
	move.l	D6,D0
	add.l	D3,D0
	subq.l	#$01,D0
	move.b	0(A2,D0.L),D1
	ext.w	D1
	move.w	D1,D0
	cmp.w	#$A0,D0
	bne	l232
l268
l231
	debug	301
; 
;             {
;                 tl=tp;

	move.l	D3,D2
	debug	302
; 
;                 while(tl+pos<len-1)

	bra	l235
l234
	debug	304
; 
;                 {
;                     if(text[pos+tl]==0x20)

	move.l	D6,D0
	add.l	D2,D0
	cmp.b	#$20,0(A2,D0.L)
	bne	l238
l269
l237
	debug	306
; 
;                     {
;                         tl++;           

	addq.l	#$01,D2
	bra	l239
l238
	bra	l236
l239
l235
	move.l	D2,D0
	add.l	D6,D0
	move.l	D7,D1
	subq.l	#$01,D1
	cmp.l	D0,D1
	bgt	l234
l270
l236
	debug	311
; 
;                     }
;                     else
;                         break;
;                 }
;                 while(tp>1)

	bra	l241
l240
	debug	313
; 
;                 {
;                     if (text[pos+tp-1]!=0x20)

	move.l	D6,D0
	add.l	D3,D0
	subq.l	#$01,D0
	cmp.b	#$20,0(A2,D0.L)
	beq	l244
l271
l243
	bra	l242
	bra	l245
l244
	debug	316
; 
;                         break;      
;                     else
;                         tp--;               

	subq.l	#$01,D3
l245
l241
	moveq.l	#1,D0
	cmp.l	D3,D0
	blt	l240
l272
l242
	bra	l233
l232
	debug	322
; 
;                 }
;             }  else 
;             {
;                 int sf=0;
; 
;                 while(tp>1)

	moveq.l	#0,D0
	bra	l247
l246
	debug	324
; 
;                 {
;                     tp--;

	subq.l	#$01,D3
	debug	325
; 
;                     if (text[pos+tp-1]==0x20)

	move.l	D6,D1
	add.l	D3,D1
	subq.l	#$01,D1
	cmp.b	#$20,0(A2,D1.L)
	bne	l250
l273
l249
	debug	327
; 
;                     {
;                         tl=tp;

	move.l	D3,D2
	debug	328
; 
;                         sf=1;

	moveq.l	#1,D0
	debug	330
; 
;                         break;
;                     }

	bra	l248
l250
l251
l247
	moveq.l	#1,D1
	cmp.l	D3,D1
	blt	l246
l274
l248
	debug	332
; 
;                 }
;                 if (!sf) {

	tst.l	D0
	bne	l253
l252
	debug	339
; 
;                     
; 
; 
; 
; 
;  
;                     if (c->atstart)  

	tst.l	78(A3)
	beq	l256
l255
	debug	340
; 
;                         tp=(tl=tf); 

	move.l	D4,D2
	move.l	D2,D3
	bra	l257
l256
	debug	341
; 
;                     else skip=1; 

	moveq.l	#1,D5
l257
l253
l254
l233
	debug	344
; 
;                 }
;             }
;             if (skip)

	tst.l	D5
	beq	l259
l258
	debug	346
; 
;             {
;                 finish_line(c);

	move.l	A3,-(sp)
	jsr	_finish_line
	addq.l	#4,sp
	debug	347
; 
;                 skip=0;

	moveq.l	#0,D5
	bra	l260
l259
	debug	350
; 
;             } else
;             {
;                 addtorenlist(c, b, pos, &text[pos], tp, 1);

	pea	1.W
	move.l	D3,-(sp)
	pea.l	0(A2,D6.L)
	move.l	D6,-(sp)
	move.l	A6,-(sp)
	move.l	A3,-(sp)
	jsr	_addtorenlist
	lea	24(sp),sp
	debug	351
; 
;                 pos+=tl;

	add.l	D2,D6
l260
	bra	l230
l229
	debug	355
; 
;             }
;         } else
;         {    
;             finish_line(c);

	move.l	A3,-(sp)
	jsr	_finish_line
	addq.l	#4,sp
l230
l227
l223
	cmp.l	D6,D7
	ble	l275
l276
	move.l	14(A3),A0
	move.b	55(A0),D4
	ext.w	D4
	ext.l	D4
	add.l	30(A3),D4
	subq.l	#$01,D4
	cmp.l	58(A3),D4
	bgt	l222
l277
l275
l224
l262
	unlk	A5
	movem.l	(sp)+,D2/D3/D4/D5/D6/D7/A2/A3/A6
	rts
l264	reg	D2/D3/D4/D5/D6/D7/A2/A3/A6
l263	equ	44
l261	equ	12
	procend
	debug	358
; 
;         }
;     }
; }

	debug	360
; 
; 
; void read_format(context *c, format *f)

	procstart
	xdef	_read_format
_read_format:
	movem.l	l287,-(sp)
	move.l	l286-4(A7),A3
	move.l	l286+0(A7),A2
	link	A5,#-l284
	debug	362
; 
; {
;     c->newind=f->indent*8;

	move.l	4(A2),D0
	asl.l	#$03,D0
	move.l	D0,74(A3)
	debug	363
; 
;     if (c->newind>c->wx/2) c->newind=c->wx/2;  

	move.l	26(A3),D0
	moveq.l	#2,D1
	jsr	__divs
	cmp.l	74(A3),D0
	bge	l279
l289
l278
	move.l	26(A3),D0
	moveq.l	#2,D1
	jsr	__divs
	move.l	D0,74(A3)
l279
l280
	debug	364
; 
;     c->newjust=f->justification;

	move.l	(A2),82(A3)
	debug	365
; 
;     if (c->atstart)

	tst.l	78(A3)
	beq	l282
l281
	debug	367
; 
;     {
;         c->curline->justification=c->newjust;

	move.l	106(A3),A0
	move.l	82(A3),22(A0)
	debug	368
; 
;         c->curline->ind=c->newind;

	move.l	106(A3),A0
	move.l	74(A3),26(A0)
	debug	369
; 
;         c->x=c->newind;

	move.l	74(A3),54(A3)
l282
l283
l285
	unlk	A5
	movem.l	(sp)+,A2/A3
	rts
l287	reg	A2/A3
l286	equ	16
l284	equ	0
	procend
	debug	371
; 
;     }
; }

	debug	373
; 
; 
; void read_style(context *c, style *s)

	procstart
	xdef	_read_style
_read_style:
	movem.l	l305,-(sp)
	move.l	l304-4(A7),A3
	move.l	l304+0(A7),A2
	link	A5,#-l302
	debug	375
; 
; {
;     if (s->ta.ta_Name)

	tst.l	(A2)
	beq	l291
l290
	debug	377
; 
;     {
;         c->curfont=get_font(&s->ta);

	pea.l	(A2)
	jsr	_get_font
	addq.l	#4,sp
	move.l	D0,38(A3)
	debug	378
; 
;         SetFont(c->rport, c->curfont);

	move.l	38(A3),-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetFont
	addq.l	#8,sp
l291
l292
	debug	380
; 
;     }
;     if (s->style!=((UWORD)~0))

	cmp.w	#$FFFF,8(A2)
	beq	l294
l307
l293
	debug	382
; 
;     {
;         SetSoftStyle(c->rport, (c->curstyle=s->style), 7);

	pea	7.W
	move.w	8(A2),86(A3)
	moveq.l	#0,D0
	move.w	86(A3),D0
	move.l	D0,-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetSoftStyle
	lea	12(sp),sp
l294
l295
	debug	384
; 
;     }
;     if (s->fg!=0xFFFF)

	cmp.w	#$FFFF,10(A2)
	beq	l297
l308
l296
	debug	386
; 
;     {
;         SetAPen(c->rport, (c->curfg=s->fg));

	move.w	10(A2),88(A3)
	moveq.l	#0,D0
	move.w	88(A3),D0
	move.l	D0,-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetAPen
	addq.l	#8,sp
l297
l298
	debug	388
; 
;     }
;     if (s->bg!=0xFFFF)

	cmp.w	#$FFFF,12(A2)
	beq	l300
l309
l299
	debug	390
; 
;     {
;         SetAPen(c->rport, (c->curbg=s->bg));

	move.w	12(A2),90(A3)
	moveq.l	#0,D0
	move.w	90(A3),D0
	move.l	D0,-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetAPen
	addq.l	#8,sp
l300
l301
l303
	unlk	A5
	movem.l	(sp)+,A2/A3
	rts
l305	reg	A2/A3
l304	equ	16
l302	equ	0
	procend
	debug	392
; 
;     }
; }

	debug	394
; 
; 
; void render_block(context *c, block *b, int pos)

	procstart
	xdef	_render_block
_render_block:
	movem.l	l325,-(sp)
	move.l	l324-4(A7),A3
	move.l	l324+0(A7),A2
	move.l	l324+4(A7),D7
	link	A5,#-l322
	debug	396
; 
; {
;     if (b->flags&0x0001)

	btst.b	#$00,21(A2)
	beq	l311
l310
l311
l312
	debug	399
; 
;     {
;     }
;     if (b->flags&0x0002)

	btst.b	#$01,21(A2)
	beq	l314
l313
l314
l315
	debug	402
; 
;     {
;     }
;     switch(b->type)

	move.l	14(A2),D0
	tst.l	D0
	beq	l318
	subq.l	#$01,D0
	beq	l320
	subq.l	#$01,D0
	beq	l319
	bra	l321
l318
	debug	405
; 
;     {
;         case BLK_TEXT:
;             render_text(c, ((char *)( b )->data), b->length, pos, b);

	move.l	A2,-(sp)
	move.l	D7,-(sp)
	move.l	22(A2),-(sp)
	move.l	30(A2),-(sp)
	move.l	A3,-(sp)
	jsr	_render_text
	lea	20(sp),sp
	debug	407
; 
;             break;
;         case BLK_FORMATTING:

	bra	l317
l319
	debug	408
; 
;             read_format(c, ((format *)( b )->data));

	move.l	30(A2),-(sp)
	move.l	A3,-(sp)
	jsr	_read_format
	addq.l	#8,sp
	debug	410
; 
;             break;
;         case BLK_STYLE:

	bra	l317
l320
	debug	411
; 
;             read_style(c, ((style *)( b )->data));

	move.l	30(A2),-(sp)
	move.l	A3,-(sp)
	jsr	_read_style
	addq.l	#8,sp
	debug	413
; 
;             break;
;         default:

	bra	l317
l321
	debug	415
; 
;             break;
;     }

	bra	l317
l317
l323
	unlk	A5
	movem.l	(sp)+,D7/A2/A3
	rts
l325	reg	D7/A2/A3
l324	equ	20
l322	equ	0
	procend
	debug	416
; 
; }

	debug	418
; 
; 
; void render_list(context *c)

	procstart
	xdef	_render_list
_render_list:
	movem.l	l333,-(sp)
	move.l	l332-4(A7),A3
	link	A5,#-l330
	debug	423
; 
; {
;     block *first;
;     int pos;
; 
;     first=c->curblock;

	move.l	124(A3),A2
	debug	424
; 
;     pos=c->curpos;

	move.l	128(A3),D7
	debug	425
; 
;     line_reset(c);

	move.l	A3,-(sp)
	jsr	_line_reset
	addq.l	#4,sp
	debug	426
; 
;     while(c->y<c->wy+c->wnd->BorderTop && (first->node.ln_Succ))

	bra	l328
l327
	debug	428
; 
;     {
;         render_block(c, first, pos);

	move.l	D7,-(sp)
	move.l	A2,-(sp)
	move.l	A3,-(sp)
	jsr	_render_block
	lea	12(sp),sp
	debug	429
; 
;         first=first->node.ln_Succ; pos=0;

	move.l	(A2),A2
	moveq.l	#0,D7
l328
	move.l	14(A3),A0
	move.b	55(A0),D0
	ext.w	D0
	ext.l	D0
	add.l	30(A3),D0
	cmp.l	58(A3),D0
	ble	l335
l336
	tst.l	(A2)
	bne	l327
l335
l329
	debug	431
; 
;     }
;     finish_line(c);

	move.l	A3,-(sp)
	jsr	_finish_line
	addq.l	#4,sp
	debug	432
; 
;     render_context(c);

	move.l	A3,-(sp)
	jsr	_render_context
	addq.l	#4,sp
	debug	433
; 
;     SetFont(c->rport, c->deffont); 

	move.l	42(A3),-(sp)
	move.l	34(A3),-(sp)
	jsr	_SetFont
	addq.l	#8,sp
l331
	unlk	A5
	movem.l	(sp)+,D7/A2/A3
	rts
l333	reg	D7/A2/A3
l332	equ	20
l330	equ	0
	procend
	debug	434
; 
; }

	debug	436
; 
; 
; void contextresized(context *c)

	procstart
	xdef	_contextresized
_contextresized:
	movem.l	l340,-(sp)
	move.l	l339-4(A7),A3
	link	A5,#-l337
	debug	438
; 
; {
;     c->wx=c->wnd->Width-c->wnd->BorderLeft-c->wnd->BorderRight-1;

	move.l	14(A3),A0
	move.w	8(A0),D0
	ext.l	D0
	move.l	14(A3),A0
	move.b	54(A0),D1
	ext.w	D1
	ext.l	D1
	neg.l	D1
	add.l	D0,D1
	move.l	14(A3),A0
	move.b	56(A0),D0
	ext.w	D0
	ext.l	D0
	neg.l	D0
	add.l	D1,D0
	subq.l	#$01,D0
	move.l	D0,26(A3)
	debug	439
; 
;     c->wy=c->wnd->Height-c->wnd->BorderTop-c->wnd->BorderBottom-1;

	move.l	14(A3),A0
	move.w	10(A0),D0
	ext.l	D0
	move.l	14(A3),A0
	move.b	55(A0),D1
	ext.w	D1
	ext.l	D1
	neg.l	D1
	add.l	D0,D1
	move.l	14(A3),A0
	move.b	57(A0),D0
	ext.w	D0
	ext.l	D0
	neg.l	D0
	add.l	D1,D0
	subq.l	#$01,D0
	move.l	D0,30(A3)
	debug	440
; 
;     init_render(c);

	move.l	A3,-(sp)
	jsr	_init_render
	addq.l	#4,sp
	debug	441
; 
;     c->curblock=(block *)c->blocklist.lh_Head;

	move.l	110(A3),124(A3)
	debug	442
; 
;     render_list(c);

	move.l	A3,-(sp)
	jsr	_render_list
	addq.l	#4,sp
	debug	443
; 
;     move_caret(c, c->curblock, 0);

	clr.l	-(sp)
	move.l	124(A3),-(sp)
	move.l	A3,-(sp)
	jsr	_move_caret
	lea	12(sp),sp
	debug	444
; 
;     free_renderlist(c);

	move.l	A3,-(sp)
	jsr	_free_renderlist
	addq.l	#4,sp
	debug	445
; 
;     close_render(c);

	move.l	A3,-(sp)
	jsr	_close_render
	addq.l	#4,sp
l338
	unlk	A5
	move.l	(sp)+,A3
	rts
l340	reg	A3
l339	equ	12
l337	equ	0
	procend
	debug	446
; 
; }

	debug	448
; 
; 
; void initcontext_contents(context *c)

	procstart
	xdef	_initcontext_contents
_initcontext_contents:
	movem.l	l345,-(sp)
	move.l	l344-4(A7),A3
	link	A5,#-l342
	debug	450
; 
; {
;     setupdemo(&c->blocklist);

	pea.l	110(A3)
	jsr	_setupdemo
	addq.l	#4,sp
l343
	unlk	A5
	move.l	(sp)+,A3
	rts
l345	reg	A3
l344	equ	12
l342	equ	0
	procend
	debug	451
; 
; }

	debug	453
; 
; 
; void freecontext_contents(context *c)

	procstart
	xdef	_freecontext_contents
_freecontext_contents:
	movem.l	l350,-(sp)
	move.l	l349-4(A7),A3
	link	A5,#-l347
	debug	455
; 
; {
;     freedemo(&c->blocklist);

	pea.l	110(A3)
	jsr	_freedemo
	addq.l	#4,sp
l348
	unlk	A5
	move.l	(sp)+,A3
	rts
l350	reg	A3
l349	equ	12
l347	equ	0
	procend
	debug	456
; 
; }

	debug	458
; 
; 
; void useless_routine()

	procstart
	xdef	_useless_routine
_useless_routine:
	movem.l	l355,-(sp)
	link	A5,#-l352
	debug	460
; 
; {
;    return;

	bra	l353
l353
	unlk	A5
	rts
l355	reg	
l354	equ	8
l352	equ	0
	procend
	debug	461
; 
; }

	xref	__divs
	xref	_freedemo
	xref	_setupdemo
	xref	_get_font
	xref	_TextFit
	xref	_free
	xref	_Remove
	xref	_Draw
	xref	_Text
	xref	_Move
	xref	_TextLength
	xref	_SetFont
	xref	_AddTail
	xref	_calloc
	xref	_DisposeRegion
	xref	_NewList
	xref	_InstallClipRegion
	xref	_OrRectRegion
	xref	_NewRegion
	xref	_SetSoftStyle
	xref	_SetDrMd
	xref	_SetBPen
	xref	_RectFill
	xref	_SetAPen
	END
