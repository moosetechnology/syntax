
include $(SX)/etc/make/gccoptions

SXEXTBIBS=$(SXLIB)/libsxext.a $(SXBIBS)

ND=$(SX)/../outdated/oldies

C=$(ND)/c
LIB=$(C)/lib.$(ARCH)

CC=cc -I$(SXINCL) -I$(SXEXTINCL) $(CFLAGS)

ALL=	$(C)/c.out			\
	$(C)/dc.out			\
	$(C)/ndc.out

all: $(ALL)

C_NEEDS=$(LIB)/sxmain.o			\
	$(LIB)/c_desambig.o		\
	$(LIB)/c_t.o

DC_NEEDS=$(LIB)/sxmain.o		\
	 $(LIB)/dc_parsact.o		\
	 $(LIB)/dc_smp.o		\
	 $(LIB)/dc_t.o

NDC_NEEDS=$(LIB)/sxmain.o		\
	 $(LIB)/ndc_parsact.o		\
	 $(LIB)/ndc_t.o

$(C)/c.out:	$(C_NEEDS)	$(SXBIBS)
	cc -o $@ $(C_NEEDS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm -f $@;exit 1)

$(LIB)/sxmain.o:	$(SXINCL)/sxunix.h	\
			sxmain.c
	$(CC) -o $@ -c sxmain.c


$(LIB)/c_t.o:		$(SXINCL)/sxunix.h 	\
			$(SXINCL)/sxba.h 	\
			c_t.c
	$(CC) -o $@ -c c_t.c		\
		-DESAMBIG=c_desambig		\
		-DPARSACT=sxjvoid		\
		-DSEMACT=sxwvoid


$(LIB)/c_desambig.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/SS.h			\
			$(SXEXTINCL)/sxnd.h		\
			$(SXEXTINCL)/sxsymbol_table.h	\
			c_desambig.c
	$(CC) -o $@ -c c_desambig.c


$(C)/dc.out:	$(DC_NEEDS)	$(SXBIBS)
	cc -o $@ $(DC_NEEDS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm -f $@;exit 1)

$(LIB)/dc_t.o:		$(SXINCL)/sxunix.h 	\
			$(SXINCL)/sxba.h 	\
			dc_t.c
	$(CC) -o $@ -c dc_t.c		\
		-DPARSACT=dc_parsact		\
		-DSEMPASS=dc_smp

$(LIB)/dc_parsact.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/XxY.h		\
			$(SXEXTINCL)/sxsymbol_table.h	\
			dc_parsact.c
	$(CC) -o $@ -c dc_parsact.c


$(LIB)/dc_smp.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/XxY.h		\
			dc_tdef.h		\
			$(SXEXTINCL)/sxsymbol_table.h	\
			dc_smp.c
	$(CC) -o $@ -c dc_smp.c

$(C)/ndc.out:	$(NDC_NEEDS)	$(SXBIBS)
	cc -o $@ $(NDC_NEEDS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm -f $@;exit 1)

$(LIB)/ndc_t.o:		$(SXINCL)/sxunix.h 	\
			$(SXINCL)/sxba.h 	\
			ndc_t.c
	$(CC) -o $@ -c ndc_t.c		\
		-Dsxndprecovery=sxndpallcorrections    \
		-DESAMBIG=sxvoid_int		\
		-DPARSACT=ndc_parsact		\
		-DSEMPASS=sxvoid

$(LIB)/ndc_parsact.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/XxY.h		\
			$(SXEXTINCL)/sxsymbol_table.h	\
			$(SXEXTINCL)/sxnd.h		\
			ndc_parsact.c
	$(CC) -o $@ -c ndc_parsact.c

clean:
	rm -f $(ALL) $(C_NEEDS) $(DC_NEEDS) $(NDC_NEEDS)

