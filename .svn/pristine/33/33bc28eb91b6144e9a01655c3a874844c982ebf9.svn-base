
include $(SX)/etc/make/gccoptions

ND=$(SX)/../outdated/oldies
NDSRC=$(ND)/src
NDLIB=$(ND)/lib

NDETC=$(ND)/etc
NDETCLIB=$(NDETC)/lib
NDETCINCL=$(NDETC)/incl

C=$(ND)/c
SRC=$(C)/src
LIB=$(C)/lib
INCL=$(C)/incl

CC=cc -I$(SXINCL) -I$(NDETCINCL) -I$(INCL) $(CFLAGS)

all:	$(C)/c.out			\
	$(C)/dc.out			\
	$(C)/ndc.out

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
	cc -o $@ $(C_NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/sxmain.o:	$(SXINCL)/sxunix.h	\
			$(SRC)/sxmain.c
	$(CC) -o $@ -c $(SRC)/sxmain.c


$(LIB)/c_t.o:		$(SXINCL)/sxunix.h 	\
			$(SXINCL)/sxba.h 	\
			$(SRC)/c_t.c
	$(CC) -o $@ -c $(SRC)/c_t.c		\
		-DESAMBIG=c_desambig		\
		-DPARSACT=sxvoid		\
		-DSEMACT=sxvoid


$(LIB)/c_desambig.o:	$(SXINCL)/sxunix.h		\
			$(ND)/incl/sxba.h		\
			$(ND)/incl/XxY.h		\
			$(ND)/incl/XH.h			\
			$(ND)/incl/SS.h			\
			$(ND)/incl/sxnd.h		\
			$(ND)/incl/sxsymbol_table.h	\
			$(SRC)/c_desambig.c
	$(CC) -o $@ -c $(SRC)/c_desambig.c

$(C)/dc.out:	$(DC_NEEDS)	$(SXBIBS)
	cc -o $@ $(DC_NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/dc_t.o:		$(SXINCL)/sxunix.h 	\
			$(SXINCL)/sxba.h 	\
			$(SRC)/dc_t.c
	$(CC) -o $@ -c $(SRC)/dc_t.c		\
		-DPARSACT=dc_parsact		\
		-Dsempass=dc_smp

$(LIB)/dc_parsact.o:	$(SXINCL)/sxunix.h		\
			$(ND)/incl/sxba.h		\
			$(ND)/incl/XxY.h		\
			$(ND)/incl/sxsymbol_table.h	\
			$(SRC)/dc_parsact.c
	$(CC) -o $@ -c $(SRC)/dc_parsact.c


$(LIB)/dc_smp.o:	$(SXINCL)/sxunix.h		\
			$(ND)/incl/sxba.h		\
			$(ND)/incl/XxY.h		\
			$(INCL)/dc_tdef.h		\
			$(ND)/incl/sxsymbol_table.h	\
			$(SRC)/dc_smp.c
	$(CC) -o $@ -c $(SRC)/dc_smp.c

$(C)/ndc.out:	$(NDC_NEEDS)	$(SXBIBS)
	cc -o $@ $(NDC_NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/ndc_t.o:		$(SXINCL)/sxunix.h 	\
			$(SXINCL)/sxba.h 	\
			$(SRC)/ndc_t.c
	$(CC) -o $@ -c $(SRC)/ndc_t.c		\
		-Dsxndprecovery=sxndpallcorrections    \
		-DESAMBIG=sxvoid		\
		-DPARSACT=ndc_parsact		\
		-Dsempass=sxvoid

$(LIB)/ndc_parsact.o:	$(SXINCL)/sxunix.h		\
			$(ND)/incl/sxba.h		\
			$(ND)/incl/XxY.h		\
			$(ND)/incl/sxsymbol_table.h	\
			$(ND)/incl/sxnd.h		\
			$(SRC)/ndc_parsact.c
	$(CC) -o $@ -c $(SRC)/ndc_parsact.c

clean:
	@echo "clean target for c should be provided"

