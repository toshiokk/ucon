bin_PROGRAMS = ucon
ucon_SOURCES = \
		stdincs.h ucon.h \
		mytypes.h sequence.h \
		myminmax.h myminmax.c \
		mystring.h mystring.c \
		myutf8.h myutf8.c \
		mydebug.h mydebug.c \
		main.c main.h \
		fbcommon.c fbcommon.h \
		fbbpp.c fbbpp.h \
		font.c font.h \
		picofont.c picofont.h \
		message.c message.h \
		hex.c hex.h \
		pen.c pen.h \
		util.c util.h \
		vterm.c vterm.h \
		vtermlow.c vtermlow.h \
		term.cpp term.h

#ucon_LDADD = -lutil
# -lpng

install-exec-hook:
	chown root:utmp $(DESTDIR)$(bindir)/ucon
	chmod ug+s $(DESTDIR)$(bindir)/ucon
	$(INSTALL) scripts/ucon10 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon11 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon20 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon21 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon22 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon30 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon31 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon40 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon12 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon13 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon24 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon25 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon26 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon36 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon14 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon15 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon28 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon29 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon42 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon16 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon17 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon32 $(DESTDIR)$(bindir)
	$(INSTALL) scripts/ucon33 $(DESTDIR)$(bindir)

install-data-local:
	$(INSTALL_DATA) $(srcdir)/fonts/ucon10.hex.gz $(DESTDIR)$(bindir)
	$(INSTALL_DATA) $(srcdir)/fonts/ucon12.hex.gz $(DESTDIR)$(bindir)
	$(INSTALL_DATA) $(srcdir)/fonts/ucon14.hex.gz $(DESTDIR)$(bindir)
	$(INSTALL_DATA) $(srcdir)/fonts/ucon16.hex.gz $(DESTDIR)$(bindir)
	mkdir -p $(HOME)/.imj
	$(INSTALL_DATA) im/imj.dic $(HOME)/.imj
	echo "USER: $(USER)"
	echo "HOME: $(HOME)"
#
#AM_CFLAGS = -fno-stack-protector  # WORKAROUND: imj module cause Abort on stack smashing
