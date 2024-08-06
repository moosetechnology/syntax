
include $(SX)/etc/make/gccoptions

CX=$(SX)/examples/cx
BIN=$(CX)/bin.$(ARCH)
LIB=$(CX)/lib.$(ARCH)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util -Dsxtables=cx_tables $(CFLAGS) -c

all: $(BIN)/cx.out

NEEDS=		$(LIB)/cx_t.o			\
		$(LIB)/cx_main.o		\
		$(LIB)/cx_act.o

$(BIN)/cx.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/cx_t.o: 	$(SXINCL)/sxunix.h		\
		cx_t.c
	$(CC) -o $@ cx_t.c		\
			-Dsxtables=cx_tables	\
			-DSEMACT=cx_act		\
			-DSEMPASS=sxvoid

$(LIB)/cx_main.o:	$(SXINCL)/sxunix.h	\
			cx_main.c
	$(CC) -o $@ cx_main.c

$(LIB)/cx_act.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/put_edit.h	\
			cx_act.c
	$(CC) -o $@ cx_act.c

# debut de code produit par "make-makefile cx"

cx_t.c: cx.bt cx.st cx.pt cx.rt 
	tables_c cx > $@

cx.bt: cx.bnf
	bnf cx.bnf 

cx.pt: cx.bt
	csynt cx || [ $$? = 1 ]

cx.st: cx.bt cx.lecl
	lecl cx.lecl || [ $$? = 1 ]

cx.rt: cx.bt cx.st cx.recor
	recor cx.recor

# fin de code produit par "make-makefile cx"

clean:
	rm -f $(BIN)/*.out $(LIB)/*.o
	rm -f cx.bt cx.pt cx.rt cx.st cx_t.c 
	rm -f cx.bn.l cx.lc.l cx.rc.l

