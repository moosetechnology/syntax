#! /bin/make

LANGUAGE	= simproc

SXBIN		= $(SX)/bin
SXINCL		= $(SX)/incl

# -----------------------------------------------------------------------------

all :
	if [ ! -f $(SXINCL)/sxunix.h ] ;\
	then make nosyntax ; \
	elif [ ! -x $(LNTDIR)/com/traian ] ; \
	then make notraian ; \
	else make $(LANGUAGE) ; \
	fi

# -----------------------------------------------------------------------------

SYNTAX_INPUTS = $(LANGUAGE).semc $(LANGUAGE).lecl $(LANGUAGE).recor $(LANGUAGE).prio

SYNTAX_OUTPUTS = $(LANGUAGE)_s.c $(LANGUAGE)_t.c

notraian : $(SYNTAX_OUTPUTS)

$(SYNTAX_OUTPUTS) : $(SYNTAX_INPUTS)
	$(SXBIN)/semc -rhs 15 $(LANGUAGE).semc > $(LANGUAGE)_s.c
	$(SXBIN)/lecl $(LANGUAGE).lecl
	$(SXBIN)/recor $(LANGUAGE).recor
	$(SXBIN)/prio $(LANGUAGE).prio
	$(SXBIN)/csynt $(LANGUAGE)
	$(SXBIN)/tables_c $(LANGUAGE) > $(LANGUAGE)_t.c
	@/bin/rm -f  *.bn.l *.lc.l *.rc.l *.pr.l *.bt *.dt *.pt *.rt *.st 

# -----------------------------------------------------------------------------

TRAIAN_INPUTS = $(LANGUAGE).lnt

TRAIAN_OUTPUTS = $(LANGUAGE).c

nosyntax : $(TRAIAN_OUTPUTS)

$(TRAIAN_OUTPUTS) : $(TRAIAN_INPUTS)
	$(LNTDIR)/com/traian -noindent $(LANGUAGE).lnt

# -----------------------------------------------------------------------------

ARCH =`$(LNTDIR)/com/arch`
CC = cc
CFLAGS = -O
LOCAL_CFLAGS = -Wno-strict-prototypes -Wno-switch-default -Wno-bad-function-cast 
INCL = -I. -I$(LNTDIR)/incl -I$(SXINCL)

$(LANGUAGE) : $(SYNTAX_OUTPUTS) $(TRAIAN_OUTPUTS) main.c
	$(CC) -c $(CFLAGS) $(LOCAL_CFLAGS) $(INCL) $(LANGUAGE)_s.c
	$(CC) -c $(CFLAGS) $(INCL) -DSEMACT=$(LANGUAGE)_act -DSCANACT=scanner_act $(LANGUAGE)_t.c
	$(CC) $(CFLAGS) $(LOCAL_CFLAGS) $(INCL) main.c $(LANGUAGE)_s.o $(LANGUAGE)_t.o $(LNTDIR)/bin.$(ARCH)/lotosnt_exceptions.o -L$(SX)/lib.$(ARCH) -lsx -lm -o $@
	@/bin/rm -f *.o

# -----------------------------------------------------------------------------

clean :
	/bin/rm -f *.err $(LANGUAGE)_s.c $(LANGUAGE)_t.c
	/bin/rm -f $(SYNTAX_OUTPUTS) $(TRAIAN_OUTPUTS) $(LANGUAGE)

