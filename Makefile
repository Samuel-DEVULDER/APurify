#
# Makefile: distrib..
#

.INCLUDE	globals

CP    = COPY QUIET
ECHO  = echo
RM    = DELETE >nil:
MKDIR = MakeDir

#
# system stuff
#

LHA		= lha 
LHAARG		= -r -e r

#
# apurify stuff
#
DDIR		= $(MAINDIR)distrib/
DISTRIB		= $(DDIR)apurify_v$(VERSION).lha
TDIR		= t:APurify

#
# make all
#
all:
	make -f Makefile.dcc
	make -f Makefile.pdc
	make -f Makefile.sas
	make -f Makefile.gcc

#
# make clean
#
clean:
	make -f Makefile.dcc clean
	make -f Makefile.pdc clean
	make -f Makefile.sas clean
	make -f Makefile.gcc clean

#
# make archive for distribution
#
bin/makehistory: utils/makehistory.c
	dcc utils/makehistory.c -o bin/makehistory -s

distrib:
	$(CP) bin/Mit-APurify.dcc bin/MIT-APurify
	$(CP) bin/Mot-APurify.dcc bin/MOT-APurify
	$(CP) bin/APF.dcc bin/APF
	bin/makehistory $(VERSION) doc/apurify_v$(VERSION).readme doc/history
	$(CP) QUIET doc/apurify_v$(VERSION).readme README
	$(CP) QUIET doc/apurify_v$(VERSION).readme distrib/
	-$(CP) `which lha` ram:lha
	-$(RM) $(DISTRIB)
	-$(MKDIR) $(TDIR)
	$(CP) bin/MIT-APurify bin/MOT-APurify bin/APF $(TDIR)/Bin
	$(CP) AREXX/APgolded.ged AREXX/APemacs.elx AREXX/APed.rx $(TDIR)/AREXX
	$(CP) lib/libAPur-gcc.a lib/APur-pdc.lib lib/APur-pdc.dir lib/APur-dcc.lib lib/APur-sas.lib $(TDIR)/Lib
        $(CP) doc/#?(|.info) $(TDIR)/Doc
	$(CP) README(|.info) $(TDIR)/
	$(CP) APurify-install(|.info) $(TDIR)/
	$(CP) test/test.c test/test.dcc test/test.pdc test/test.gcc test/test.sas $(TDIR)/Test
	-$(CP) ../APurify.info $(TDIR).info
	-$(CP) /APurify.info $(TDIR).info
	$(CP) Doc.info Bin.info Lib.info $(TDIR)
	$(ECHO) >>$(DDIR)apurify_v$(VERSION).readme "*N****** Contents *******N"
	-$(RM) $(DISTRIB)
	$(LHA) $(LHAARG) $(DISTRIB) $(TDIR) $(TDIR).info
	$(LHA) v >>$(DDIR)apurify_v$(VERSION).readme $(DISTRIB)
	-$(RM) distrib.lha
	ram:lha a distrib.lha $(DISTRIB) $(DDIR)apurify_v$(VERSION).readme

#
# make backup
#
backup:
	-$(RM) backup.lha
	$(LHA) $(LHAARG) backup.lha $(MAINDIR)
	$(LHA) -e d backup.lha "#?(.o|.lha|@|#|!)"
	$(LHA) -e d backup.lha "bin/#?.#?" "torture#?"
