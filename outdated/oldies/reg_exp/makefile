
include $(SX)/etc/make/gccoptions

# LECL=$(SX)/lecl
LECL=$(SX)/../outdated/oldies/reg_exp
LECLSRC=$(LECL)/src
LECLLIB=$(LECL)/lib.$(ARCH)
LECLINCL=$(LECL)/incl

RE=$(SX)/../outdated/oldies/reg_exp
RELIB=$(RE)/lib.$(ARCH)

CC=cc -I$(LECLINCL) -I$(SXINCL) -I$(SXINCL)/util -I$(SXEXTINCL) $(CFLAGS)

ALL=$(RE)/reg_exp.out

all: $(ALL)

NEEDS=		$(RELIB)/reg_exp_main.o

$(RE)/reg_exp.out:	$(NEEDS)	$(SXBIBS)
	cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(RELIB)/reg_exp_main.o: $(SXINCL)/sxunix.h				\
			$(SX)/lecl/lecl_ag.h			\
			$(SXEXTINCL)/sxdynam_scanner.h	\
			reg_exp_main.c
	$(CC) -o $@ -c reg_exp_main.c

clean:
	rm -f $(ALL) $(NEEDS)

