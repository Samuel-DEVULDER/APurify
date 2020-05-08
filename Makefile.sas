#
# Makefile: how to make apurify on amiga with SAS/C
#

.INCLUDE	globals

#
# system stuff
#
CFLAGS		= OPT DBG FF NOICONS
CC		= sc
LHA		= lha 
LHAARG		= -r -e r
AR              = oml
CP		= copy
# don't use c:ECHO since it can't have long arguments:
ECHO		= bin/echo 

#
# apurify stuff
#
OBJSDIR		= sas-objs/
MITAPURIFY	= $(BDIR)Mit-APurify.sas
MOTAPURIFY	= $(BDIR)Mot-APurify.sas
APF		= $(BDIR)APF.sas
APLIB		= $(LDIR)APur-sas.lib
TEST		= $(TDIR)test.sas
MKREV		= $(BDIR)mkrev.sas
RM		= $(BDIR)rm

#
# make all execs
#
all: $(MOTAPURIFY) $(MITAPURIFY) $(APF) $(TEST)

#
# clear temporary objects (.o, .a)
#
clean: mot-parse_clean mit-parse_clean lib_clean apf_clean

#
# utils: a rm that accepts more args than amigados delete and a revision
# manager
#
$(RM): $(UDIR)rm.c
	$(CC) $(CFLAGS) $< TO $@ LINK OBJNAME=ram:t/

$(MKREV): $(UDIR)mkrev.c
	$(CC) $(CFLAGS) $< TO $@ LINK OBJNAME=ram:t/

#
# mit-parser
#
MITPSRCS = $(MITPMODS:%=$(MITPDIR)%.c)
MITPOBJS = $(MITPMODS:%=$(MITPDIR)$(OBJSDIR)%.o)

MITREVSRC = $(MITPDIR)rev.c
MITREVOBJ = $(MITPDIR)$(OBJSDIR)rev.o

$(MITAPURIFY): $(MITPOBJS) $(MKREV)
	-$(MKREV) $(MITREVSRC) $(MITPOBJS)
	$(CC) $(CFLAGS) OBJNAME $(MITREVOBJ) $(MITREVSRC) NOLINK DEF VNUM="$(VERSION)"
#	$(CP) $(MITPDIR)$(notdir $(MITREVOBJ)) $(MITPDIR)$(OBJSDIR)
#	$(RM) $(MITPDIR)$(notdir $(MITREVOBJ))
	$(ECHO) >$(MITPDIR)$(OBJSDIR)linkfile $(MITPOBJS) $(MITREVOBJ)
	$(CC) $(CFLAGS) TO $@ LINK WITH $(MITPDIR)$(OBJSDIR)linkfile 
	-$(RM) $(MITPDIR)$(OBJSDIR)linkfile $*.lnk

$(MITPDIR)$(OBJSDIR)%.o: $(MITPDIR)%.c
	$(CC) $(CFLAGS) IDIR $(MITPDIR) OBJNAME $@ $< NOLINK
#	$(CP) $(MITPDIR)$(notdir $@) $(MITPDIR)$(OBJSDIR)
#	$(RM) $(MITPDIR)$(notdir $@)

mit-parse_clean: $(RM)
	-$(RM) $(MITPOBJS) $(MITREVOBJ)

#
# mot-parser
#
MOTPSRCS = $(MOTPMODS:%=$(MOTPDIR)%.c)
MOTPOBJS = $(MOTPMODS:%=$(MOTPDIR)$(OBJSDIR)%.o)

MOTREVSRC = $(MOTPDIR)rev.c
MOTREVOBJ = $(MOTPDIR)$(OBJSDIR)rev.o

$(MOTAPURIFY): $(MOTPOBJS) $(MKREV)
	-$(MKREV) $(MOTREVSRC) $(MOTPOBJS)
	$(CC) $(CFLAGS) OBJNAME $(MOTREVOBJ) $(MOTREVSRC) NOLINK DEF VNUM="$(VERSION)"
	$(ECHO) >$(MOTPDIR)$(OBJSDIR)linkfile $(MOTPOBJS) $(MOTREVOBJ)
	$(CC) $(CFLAGS) TO $@ LINK WITH $(MOTPDIR)$(OBJSDIR)linkfile 
	-$(RM) $(MOTPDIR)$(OBJSDIR)linkfile $*.lnk
#	assign MOTP: $(MOTPDIR)
#	-$(MKREV) $(MOTREVSRC) $(MOTPMODS:%=MOTP:$(OBJSDIR)%.o)
#	$(CC) $(CFLAGS) OBJNAME $(MOTREVOBJ) $(MOTREVSRC) NOLINK DEF VNUM="$(VERSION)"
#	echo >$(MOTPDIR)$(OBJSDIR)linkfile $(MOTPMODS:%=MOTP:$(OBJSDIR)%.o) $(MOTREVOBJ)
#	$(CC) $(CFLAGS) TO $@ LINK WITH $(MOTPDIR)$(OBJSDIR)linkfile 
#	-$(RM) $(MOTPDIR)$(OBJSDIR)linkfile $*.lnk
#	assign MOTP: remove
	$(CP) $@ bin/Mot-APurify

$(MOTPDIR)$(OBJSDIR)%.o: $(MOTPDIR)%.c
	$(CC) $(CFLAGS) IDIR $(MOTPDIR) OBJNAME $@ $< NOLINK
#	$(CP) $(MOTPDIR)$(notdir $@) $(MOTPDIR)$(OBJSDIR)
#	$(RM) $(MOTPDIR)$(notdir $@)

mot-parse_clean: $(RM)
	-$(RM) $(MOTPOBJS) $(MOTREVOBJ)

#
# runtime library
#
LMODS += APserv-sas APmalloc

LOBJS = $(LMODS:%=$(LDIR)$(OBJSDIR)%.o)

$(APLIB): $(LOBJS)
	-$(RM) $@
	$(AR) $@ R $(LOBJS) 
	copy $@ sc:lib

$(LDIR)$(OBJSDIR)%.o: $(LDIR)%.c
	$(CC) $(CFLAGS) NOSTKCHK PARMS BOTH IDIR $(LDIR) OBJNAME $@ $< NOLINK 
#	$(CP) $(LDIR)$(notdir $@) $(LDIR)$(OBJSDIR)
#	$(RM) $(LDIR)$(notdir $@)

$(LDIR)$(OBJSDIR)%.o: $(LDIR)%.a
	$(CC) $(CFLAGS) $< NOLINK 
	$(CP) $(LDIR)$(notdir $@) $(LDIR)$(OBJSDIR)
	$(RM) $(LDIR)$(notdir $@)

lib_clean: $(RM)
	-$(RM) $(LOBJS)

#
# frontend
#
APFSRCS = $(APFMODS:%=$(APFDIR)%.c)
APFOBJS = $(APFMODS:%=$(APFDIR)$(OBJSDIR)%.o)

APFREVSRC = $(APFDIR)rev.c
APFREVOBJ = $(APFDIR)$(OBJSDIR)rev.o

$(APF): $(APFOBJS) $(MKREV)
	-$(MKREV) $(APFREVSRC) $(APFOBJS)
	$(CC) $(CFLAGS) OBJNAME $(APFREVOBJ) $(APFREVSRC) NOLINK DEF VNUM="$(VERSION)"
#	$(CP) $(APFDIR)$(notdir $(APFREVOBJ)) $(APFDIR)$(OBJSDIR)
#	$(RM) $(APFDIR)$(notdir $(APFREVOBJ))
	$(ECHO) >$(APFDIR)$(OBJSDIR)linkfile $(APFOBJS) $(APFREVOBJ)
	$(CC) $(CFLAGS) TO $@ LINK WITH $(APFDIR)$(OBJSDIR)linkfile 
	-$(RM) $(APFDIR)$(OBJSDIR)linkfile $*.lnk

$(APFDIR)$(OBJSDIR)%.o: $(APFDIR)%.c
	$(CC) $(CFLAGS) IDIR $(APFDIR) OBJNAME $@ $< NOLINK
#	$(CP) $(APFDIR)$(notdir $@) $(APFDIR)$(OBJSDIR)
#	$(RM) $(APFDIR)$(notdir $@)

apf_clean: $(RM)
	-$(RM) $(APFOBJS) $(APFREVOBJ)

#
# test
#
$(TEST): $(TDIR)test.c $(MOTAPURIFY) $(APLIB) $(RM) $(APF)
	$(APF) -v -sl -motparser $(MOTAPURIFY) sc $(TDIR)test.c TO $(TDIR)test.sas LINK DBG FF NOICONS OBJNAME=ram:t/
#	sc  $(TDIR)test.c DISASM $(TDIR)test.a 
#	$(MOTAPURIFY) -sas $(TDIR)test.a
#	sc $(CFLAGS) USCORE $(TDIR)test.a TO $(TDIR)test.o
#	sc $(CFLAGS) LINK TO $@ $(TDIR)test.o LIB $(APLIB)
#	-$(RM) $(TDIR)test.a

test_clean: $(RM)

