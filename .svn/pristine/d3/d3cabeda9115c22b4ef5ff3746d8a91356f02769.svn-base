noinst_LIBRARIES =	libsxutil.a

SXSRC =		$(top_srcdir)/src
SXUTIL =	$(top_srcdir)/util
SXINCL =	$(top_srcdir)/incl

AM_CPPFLAGS = -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

# ... et pour quelques warnings de plus
#CC=cc -Wcomment -Wformat -Wimplicit -Wreturn-type -Wmain -Wswitch -I$(INCL) # -Wunused -Wuninitialized -Wtrigraphs -Wparentheses -Wtraditional  -Wredundant-decls  -Wchar-subscripts

LIBSXUTIL =	$(SXUTIL)/bnffree.c		\
		$(SXUTIL)/bnfread.c		\
		$(SXUTIL)/bnfwrite.c		\
		$(SXUTIL)/bstr.c		\
		$(SXUTIL)/leclfree.c		\
		$(SXUTIL)/leclread.c		\
		$(SXUTIL)/leclwrite.c		\
		$(SXUTIL)/paradisfree.c	\
		$(SXUTIL)/paradisread.c	\
		$(SXUTIL)/paradiswrite.c	\
		$(SXUTIL)/parserfree.c	\
		$(SXUTIL)/parserread.c	\
		$(SXUTIL)/parserwrite.c	\
		$(SXUTIL)/priofree.c		\
		$(SXUTIL)/prioread.c		\
		$(SXUTIL)/priowrite.c		\
		$(SXUTIL)/recorfree.c		\
		$(SXUTIL)/recorread.c		\
		$(SXUTIL)/recorwrite.c	\
		$(SXUTIL)/sematfree.c		\
		$(SXUTIL)/sematread.c		\
		$(SXUTIL)/sematwrite.c

libsxutil_a_SOURCES = $(SXUTIL)

