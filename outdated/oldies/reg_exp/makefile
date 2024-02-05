
include $(SX)/etc/make/gccoptions

# LECL=$(SX)/lecl
LECL=$(SX)/../outdated/oldies/reg_exp
LECLSRC=$(LECL)/src
LECLLIB=$(LECL)/lib
LECLINCL=$(LECL)/incl

RE=$(SX)/../outdated/oldies/reg_exp
RESRC=$(RE)/src
RELIB=$(RE)/lib
REINCL=$(RE)/incl

CC=cc -I$(REINCL) -I$(LECLINCL) -I$(SXINCL) -I$(SXINCL)/util -I$(SX)/../extensions/incl $(CFLAGS)

ALL=$(RE)/reg_exp.out

all: $(ALL)

NEEDS=		$(RELIB)/reg_exp_main.o

$(RE)/reg_exp.out:	$(NEEDS)	$(SXBIBS)
	cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(RELIB)/reg_exp_main.o: $(SXINCL)/sxunix.h				\
			$(SX)/lecl/lecl_ag.h			\
			$(SX)/../extensions/incl/sxdynam_scanner.h	\
			$(RESRC)/reg_exp_main.c
	$(CC) -o $@ -c $(RESRC)/reg_exp_main.c

clean:
	rm -f $(ALL) $(NEEDS)

