
include $(SX)/etc/make/gccoptions

TAG=$(SX)/../outdated/oldies/TAG
LIB=$(TAG)/lib.$(ARCH)

CC=cc -I$(SXINCL) -I$(SXINCL)/util -I$(TAG)/../incl $(CFLAGS)

ALL=$(TAG)/xtag2lig.out

all: $(ALL)

NEEDS1=$(LIB)/xtag_main.o		\
       $(LIB)/xtag_sact.o		\
       $(LIB)/xtag_smp.o    		\
       $(LIB)/xtag_t.o

$(TAG)/xtag2lig.out:	$(NEEDS1)	$(SXBIBS)
	cc -o $@ $(NEEDS1) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@;exit 1)

$(LIB)/xtag_main.o:	$(SXINCL)/sxunix.h	\
			xtag.h	 	\
			xtag_main.c
	$(CC) -o $@ -c xtag_main.c

$(LIB)/xtag_sact.o:	$(SXINCL)/sxunix.h	\
			xtag.h	 	\
			xtag_sact.c
	$(CC) -o $@ -c xtag_sact.c

$(LIB)/xtag_smp.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/SS.h		\
			$(SXINCL)/varstr.h	\
			xtag.h	 	\
			xtag_smp.c
	$(CC) -o $@ -c xtag_smp.c

$(LIB)/xtag_t.o:	$(SXINCL)/sxunix.h 	\
			xtag_t.c
	$(CC) -o $@ -c xtag_t.c		\
		-DSEMACT=sxwvoid			\
		-DSEMPASS=xtag_smp

clean:
	rm -f $(ALL) $(NEEDS1)

