noinst_LIBRARIES =	libsxetc.a

SXSRC =		$(top_srcdir)/src
SXETCSRC =	$(top_srcdir)/etc/src
SXINCL =	$(top_srcdir)/incl
SXETCINCL =	$(top_srcdir)/etc/incl

AM_CPPFLAGS = -I$(SXINCL) -I$(SXETCINCL) $(CFLAGS)

# ... et pour quelques warnings de plus
#CC=cc -Wcomment -Wformat -Wimplicit -Wreturn-type -Wmain -Wswitch -I$(INCL) # -Wunused -Wuninitialized -Wtrigraphs -Wparentheses -Wtraditional  -Wredundant-decls  -Wchar-subscripts

LIBSXETC =	$(SXETCSRC)/bnffree.c		\
		$(SXETCSRC)/bnfread.c		\
		$(SXETCSRC)/bnfwrite.c		\
		$(SXETCSRC)/bstr.c		\
		$(SXETCSRC)/leclfree.c		\
		$(SXETCSRC)/leclread.c		\
		$(SXETCSRC)/leclwrite.c		\
		$(SXETCSRC)/paradisfree.c	\
		$(SXETCSRC)/paradisread.c	\
		$(SXETCSRC)/paradiswrite.c	\
		$(SXETCSRC)/parserfree.c	\
		$(SXETCSRC)/parserread.c	\
		$(SXETCSRC)/parserwrite.c	\
		$(SXETCSRC)/priofree.c		\
		$(SXETCSRC)/prioread.c		\
		$(SXETCSRC)/priowrite.c		\
		$(SXETCSRC)/recorfree.c		\
		$(SXETCSRC)/recorread.c		\
		$(SXETCSRC)/recorwrite.c	\
		$(SXETCSRC)/sematfree.c		\
		$(SXETCSRC)/sematread.c		\
		$(SXETCSRC)/sematwrite.c

libsxetc_a_SOURCES = $(LIBSXETC)

