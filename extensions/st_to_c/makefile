
include $(SX)/etc/make/gccoptions

LECL=$(SX)/../extensions/st_to_c

LIB=$(LECL)/lib.$(ARCH)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(SXAUX)/st_to_c.out

STTOC=		$(LIB)/st_to_c.o		\
		$(LIB)/leclread.o		\
		$(LIB)/leclfree.o

$(SXAUX)/st_to_c.out:	$(STTOC)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(STTOC) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/st_to_c.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxsstmt.h		\
			$(SXINCL)/put_edit.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/util/S_tables.h	\
			st_to_c.c
	$(CC) -o $@ -c st_to_c.c

$(LIB)/leclread.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/util/S_tables.h	\
			$(SXUTIL)/leclread.c
	$(CC) -o $@ -c $(SXUTIL)/leclread.c

$(LIB)/leclfree.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/util/S_tables.h	\
			$(SXUTIL)/leclfree.c
	$(CC) -o $@ -c $(SXUTIL)/leclfree.c

clean:
	/bin/rm -f $(LIB)/*.o $(SXAUX)/st_to_c.out

