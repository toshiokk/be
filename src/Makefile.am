# be project Makefile.am

AUTOMAKE_OPTIONS = gnu no-dependencies

DEFS= -DSYSCONFDIR=\"$(sysconfdir)\"

#AM_CFLAGS = -O0 -Wformat-truncation=0 -D_FILE_OFFSET_BITS=64
#AM_CFLAGS = -O2 -Wformat-truncation=0 -D_FILE_OFFSET_BITS=64
#AM_CFLAGS = -Wformat-truncation=0 -D_FILE_OFFSET_BITS=64
AM_CFLAGS = -Wall -D_FILE_OFFSET_BITS=64

#AM_CPPFLAGS = -O0 -Iintl -DLOCALEDIR=\"$(localedir)\" -I@includedir@
AM_CPPFLAGS = -O2 -Iintl -DLOCALEDIR=\"$(localedir)\" -I@includedir@
#AM_CPPFLAGS = -Iintl -DLOCALEDIR=\"$(localedir)\" -I@includedir@

BUILD_DIR = ./.build

bin_PROGRAMS = be
be_SOURCES = \
	main.c \
	appmode.c \
	editor.c \
	editorfile.c \
	editor2.c \
	editor3.c \
	editormove.c \
	editormove2.c \
	editorgoto.c \
	editorcut.c \
	editorcut2.c \
	help.c \
	editorkeys.c \
	editorundo.c \
	line.c \
	linewrap.c \
	buffer1.c \
	buffer2.c \
	buffers.c \
	color.c \
	debug.c \
	disp.c \
	fileio.c \
	filer.c \
	filerlist.c \
	filercmd1.c \
	filercmd2.c \
	filerdir.c \
	filerrun.c \
	filerkeys.c \
	history.c \
	keymacro.c \
	keys.c \
	menu.c \
	rcfile.c \
	regexp.c \
	search.c \
	sig.c \
	termifkey.c \
	termif.c \
	cursesif.c \
	tio.c \
	utf8.c \
	utils.c \
	utilstr.c \
	myutf8.c \
	utilpath.c \
	utilfile.c \
	winin.c \
	winout.c \
	appmode.h \
	buffer1.h \
	buffer2.h \
	buffers.h \
	config.h \
	color.h \
	debug.h \
	editor.h \
	editorfile.h \
	editor2.h \
	editor3.h \
	editormove.h \
	editormove2.h \
	editorgoto.h \
	editorcut.h \
	editorcut2.h \
	help.h \
	editorkeys.h \
	editorundo.h \
	fileio.h \
	filer.h \
	filerlist.h \
	filercmd1.h \
	filercmd2.h \
	filerdir.h \
	filerrun.h \
	filerkeys.h \
	headers.h \
	history.h \
	keymacro.h \
	keys.h \
	menu.h \
	line.h \
	linewrap.h \
	main.h \
	rcfile.h \
	regexp.h \
	search.h \
	sig.h \
	termifkey.h \
	termif.h \
	cursesif.h \
	tio.h \
	utf8.h \
	utils.h \
	utilstr.h \
	myutf8.h \
	utilpath.h \
	utilfile.h \
	disp.h \
	winin.h \
	winout.h

# Include the output of "gcc -MM *.c"
include be.dep

#man_MANS = be.1 berc.5
###be_LDADD = @GLIB_LIBS@ @LIBINTL@
##be_LDADD = -Wl,-Bstatic

##be_LDFLAGS = -Wl,-Bdynamic,-lc -Wl,-Bstatic
##be_LDFLAGS = -Wl,-Bdynamic -Wl,-Bstatic -L /usr/lib/x86_64-linux-gnu
##be_LDFLAGS = -Wl,-Bdynamic -Wl,-Bstatic,-lc -L /usr/lib/x86_64-linux-gnu
##be_LDFLAGS = -Wl,-Bdynamic,-lc -Wl,-Bstatic -L /usr/lib/x86_64-linux-gnu
##be_LDFLAGS = -Wl,-Bdynamic,-lc,-L/usr/lib/x86_64-linux-gnu -Wl,-Bstatic
##be_LDFLAGS = -Wl,-Bdynamic,-lc -Wl,-Bstatic,-L/usr/lib/x86_64-linux-gnu

#info_TEXINFOS = be.texi
#MAKEINFO = makeinfo --no-split

distclean-local:
	-rm -fv html
	-rm -fv README.html

#SUBDIRS = po m4
#SUBDIRS = po

localedir = /usr/share/locale

#ACLOCAL_AMFLAGS = -I m4
