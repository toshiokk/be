# be project Makefile.am

AUTOMAKE_OPTIONS = gnu no-dependencies

DEFS= -DSYSCONFDIR=\"$(sysconfdir)\"
AM_CFLAGS = -Wall -D_FILE_OFFSET_BITS=64

bin_PROGRAMS = be
be_SOURCES = \
	main.c \
	appmode.c \
	editor.c \
	editor2.c \
	editormove.c \
	editormove2.c \
	editorgoto.c \
	editorcut.c \
	editorcut2.c \
	editorlist.c \
	editorkeys.c \
	editorundo.c \
	line.c \
	linewrap.c \
	buffer.c \
	buffers.c \
	color.c \
	debug.c \
	disp.c \
	fileio.c \
	filer.c \
	filer2.c \
	filercmd.c \
	filerrun.c \
	filerkeys.c \
	history.c \
	keymacro.c \
	keys.c \
	rcfile.c \
	regexp.c \
	search.c \
	sig.c \
	termif.c \
	cursesif.c \
	tio.c \
	utf8.c \
	utils.c \
	utilstr.c \
	myutf8.c \
	utilpath.c \
	winin.c \
	winout.c \
	appmode.h \
	buffer.h \
	buffers.h \
	config.h \
	color.h \
	debug.h \
	editor.h \
	editor2.h \
	editormove.h \
	editormove2.h \
	editorgoto.h \
	editorcut.h \
	editorcut2.h \
	editorlist.h \
	editorkeys.h \
	editorundo.h \
	fileio.h \
	filer.h \
	filer2.h \
	filercmd.h \
	filerrun.h \
	filerkeys.h \
	headers.h \
	history.h \
	keymacro.h \
	keys.h \
	line.h \
	linewrap.h \
	main.h \
	rcfile.h \
	regexp.h \
	search.h \
	sig.h \
	termif.h \
	cursesif.h \
	tio.h \
	utf8.h \
	utils.h \
	utilstr.h \
	myutf8.h \
	utilpath.h \
	disp.h \
	winin.h \
	winout.h

# Include the output of "gcc -MM *.c"
include be.dep

#man_MANS = be.1 berc.5
#be_LDADD = @GLIB_LIBS@ @LIBINTL@

#info_TEXINFOS = be.texi
#MAKEINFO = makeinfo --no-split

distclean-local:
	-rm -fv html
	-rm -fv README.html

#SUBDIRS = po m4
#SUBDIRS = po

localedir = /usr/share/locale
AM_CPPFLAGS = -Iintl -DLOCALEDIR=\"$(localedir)\" -I@includedir@

#ACLOCAL_AMFLAGS = -I m4
