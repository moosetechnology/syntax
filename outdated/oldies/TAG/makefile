
include $(SX)/etc/make/gccoptions

TAG=$(SX)/../outdated/oldies/TAG
SRC=$(TAG)/src
LIB=$(TAG)/lib
INCL=$(TAG)/incl

CC=cc -I$(INCL) -I$(SXINCL) -I$(SXINCL)/util -I$(TAG)/../incl $(CFLAGS)

all: $(TAG)/xtag2lig.out

NEEDS1=$(LIB)/xtag_main.o		\
       $(LIB)/xtag_sact.o		\
       $(LIB)/xtag_smp.o    		\
       $(LIB)/xtag_t.o

$(TAG)/xtag2lig.out:	$(NEEDS1)	$(SXBIBS)
	cc -o $@ $(NEEDS1) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/xtag_main.o:	$(SXINCL)/sxunix.h	\
			$(INCL)/xtag.h	 	\
			$(SRC)/xtag_main.c
	$(CC) -o $@ -c $(SRC)/xtag_main.c

$(LIB)/xtag_sact.o:	$(SXINCL)/sxunix.h	\
			$(INCL)/xtag.h	 	\
			$(SRC)/xtag_sact.c
	$(CC) -o $@ -c $(SRC)/xtag_sact.c

$(LIB)/xtag_smp.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/SS.h		\
			$(SXINCL)/varstr.h	\
			$(INCL)/xtag.h	 	\
			$(SRC)/xtag_smp.c
	$(CC) -o $@ -c $(SRC)/xtag_smp.c

$(LIB)/xtag_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/xtag_t.c
	$(CC) -o $@ -c $(SRC)/xtag_t.c		\
		-Dsxtables=xtag_tables		\
		-DSCANACT=xtag_scan_act		\
		-DSEMACT=sxvoid			\
		-Dsempass=xtag_smp

clean:
	@echo "clean target for TAG should be added"

