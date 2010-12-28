# $Id: Makefile 2373 2006-01-12 00:54:22Z soda $

top_builddir = ../../..
top_srcdir = $(top_builddir)
srcdir = .

include $(top_srcdir)/makes/var.mk
include $(srcdir)/../Makefile.inc

DOCBOOK_DIR=$(top_srcdir)/doc/docbook/ja/ref/man5
include $(DOCBOOK_DIR)/Makefile.inc
MAN = $(DOCBOOK)
EXTRA_CLEAN_TARGETS = manpage.links manpage.refs

include $(top_srcdir)/makes/man.mk
