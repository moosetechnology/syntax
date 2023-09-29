AUTOMAKE_OPTIONS= foreign

SXBIN = $(syntaxbin)
SXSRC = $(syntaxsrc)
SXINCL= $(syntaxincl)
### No debug (shoud be default)
BIBS= $(syntaxlib)
CFLAGS = -O2
### With debug (requires the debug library to be installed, which in turns requires the ./configure
### script to have been run with the --enable-debug option
#BIBS= $(syntaxlibdir)/libsx.debug.a
#CFLAGS = -ggdb -DEBUG 

COMPILER=gcc
CC_FLAGS = $(COMPILER) -I$(SXINCL) $(CFLAGS) -Iincl
PREPRO=cd $(SRCG); $(COMPILER) -I$(SXINCL) -I$(INCL) $(CFLAGS) -Wp,-C,-dD,-dM -E -x c


all: nothing

nothing:
	@echo "### WARNING: No default target";\
	echo "Please specify explicitely a target, i.e. something of one of the following forms:";\
	echo " - grammar_name.out           standard (non-huge) parser with lexer (lecl/recor)";\
	echo " - grammar_name.lex.out:      standard (non-huge) parser without lexer (a lexicon can";\
	echo "                              be provided as a .lex file in the 'spec' folder)";\
	echo " - grammar_name.enb.lex.out:  same as previous, with exact n-best forests support";\
	echo " - grammar_name.huge.out:     huge parser (without lexer, lexicon is possible)";\
	echo " - grammar_name.enb.huge.out: same as previous, with exact n-best forests support";\
        echo "Compiling a PCFG parser requires a .bnf file in the 'spec' folder. ";\
	echo "Each production has the following format:";\
	echo "   <LHS> = <RHS1> <RHS2> ... <RHSn> ; proba";\
	echo "The grammar must be proper. A non-proper grammar can be turned into a proper";\
	echo "one with the tool make_proper.pl, located in the bnf subdirectory of SYNTAX";\
	echo "The left-hand side of the first rule is the axiom."

%.enb.huge.out: \
	lib/enb_sxsem_mngr.o		\
	lib/exact_nbest_semact.o	\
	lib/%_proba.o			\
	lib/%.huge_udag_scanner.o	\
	lib/%.huge_lexicalizer_mngr.o	\
	lib/%.huge_earley_parser.o	\
	$(BIBS)
	$(CC_FLAGS) -o $@ $^ -lm

%.huge.out: \
	lib/sxsem_mngr.o		\
	lib/%_proba.o			\
	lib/%.huge_udag_scanner.o	\
	lib/%.huge_lexicalizer_mngr.o	\
	lib/%.huge_earley_parser.o	\
	$(BIBS)
	$(CC_FLAGS) -o $@ $^ -lm

%.enb.lex.out: \
	lib/enb_sxsem_mngr.o		\
	lib/exact_nbest_semact.o	\
	lib/%_proba.o			\
	lib/%.lex_udag_scanner.o	\
	lib/%_lexicalizer_mngr.o	\
	lib/%_earley_parser.o		\
	lib/%_t.o			\
	$(BIBS)
	$(CC_FLAGS) -o $@ $^ -lm

%.lex.out: \
	lib/sxsem_mngr.o		\
	lib/%_proba.o			\
	lib/%.lex_udag_scanner.o	\
	lib/%_lexicalizer_mngr.o	\
	lib/%_earley_parser.o		\
	lib/%_t.o			\
	$(BIBS)
	$(CC_FLAGS) -o $@ $^ -lm


%.enb.out: \
	lib/enb_sxsem_mngr.o		\
	lib/exact_nbest_semact.o	\
	lib/%_proba.o			\
	lib/%.std_udag_scanner.o	\
	lib/%_lexicalizer_mngr.o	\
	lib/%_earley_parser.o		\
	lib/%_t.o			\
	$(BIBS)
	$(CC_FLAGS) -o $@ $^ -lm

%.out: \
	lib/sxsem_mngr.o		\
	lib/%_proba.o			\
	lib/%.std_udag_scanner.o	\
	lib/%_lexicalizer_mngr.o	\
	lib/%_earley_parser.o		\
	lib/%_t.o			\
	$(BIBS)
	$(CC_FLAGS) -o $@ $^ -lm


#------------------------------------------------------------------
# Construction des tables de l'analyseur earley pour L(G) - huge grammar mode
#------------------------------------------------------------------

incl/%.huge_earley.h spec/%.huge.bn.l: spec/%.huge.bnf
	cd spec && bnf -h $*.huge.bnf > ../incl/$*.huge_earley.h && cd ..

spec/%.huge.bnf: spec/%.bnf
	cp -p $< $@

#------------------------------------------------------------------
# Construction des tables de l'analyseur earley pour L(G) - standard mode
#------------------------------------------------------------------

incl/%_earley.h: spec/%.bt
	@test -d incl || mkdir incl
	cd spec && csynt_lc -lc $* > ../incl/$*_earley.h && cd ..

src/%_t.c: spec/%.pt spec/%.st spec/%.rt
	@test -d src || mkdir src
	cd spec && tables_c $* > ../src/$*_t.c && cd ..

spec/%.pt: spec/%.bt
	cd spec && dummy_csynt $* && cd ..

spec/%.bt spec/%.bn.l: spec/%.bnf
	cd spec && bnf -t $*.bnf > $*.tlist && cd ..

#	cd spec && bnf -t -col1 $*.bnf > $*.tlist && cd ..

spec/%.lecl: spec/default.lecl
	cp -p $< $@

spec/%.recor: spec/default.recor
	cp -p $< $@

spec/%.tdef: spec/default.tdef
	cp -p $< $@

spec/%.st spec/%.lc.l: spec/%.lecl spec/%.bt
	-(cd spec &&  lecl $*.lecl && cd ..)

spec/%.rt spec/%.rc.l: spec/%.recor spec/%.bt
	cd spec && recor $*.recor && cd ..

incl/%_tdef.h: spec/%.tdef spec/%.st
	@test -d incl || mkdir incl
	cd spec && tdef $*.tdef > ../incl/$*_tdef.h && cd ..

# Building the lexicon C tables for lex-style parsers
incl/%_lex.h: spec/%.bt spec/%.lex
	@test -d incl || mkdir incl
	cd spec && cat $*.tlist $*.lex | make_a_dico  -wn 500000 -wl 10 -comb 0 -cvt 10000 -md > ../$@ && cd ..



#------------------------------------------------------------------
# Quelques modules de syntax a compiler
#------------------------------------------------------------------

lib/%.huge_udag_scanner.o: $(SXSRC)/udag_scanner.c incl/%.huge_earley.h
	@test -d lib || mkdir lib
	$(CC_FLAGS) -c $(SXSRC)/udag_scanner.c -o $@	\
	-Dkeywords_h=\"$*.huge_earley.h\"

lib/%.std_udag_scanner.o: $(SXSRC)/udag_scanner.c
	@test -d lib || mkdir lib
	$(CC_FLAGS) -c $(SXSRC)/udag_scanner.c -o $@	\
	-Ddummy_tables=T_tables 

lib/%.lex_udag_scanner.o: $(SXSRC)/udag_scanner.c	\
			  incl/%_lex.h
	@test -d lib || mkdir lib
	$(CC_FLAGS) -c $(SXSRC)/udag_scanner.c -o $@	\
	-Dsxdfa_comb_dico_h=\"$*_lex.h\"

# Lexicalizer (invoked by -Dis_lex when compiling earley_parser.c)
# -Dis_check_rhs_sequences allows for option -crs k (cf. Boullier & Sagot 07)
# -Dis_set_automaton allows for -dsa (cf. Boullier 03)
lib/%_lexicalizer_mngr.o: $(SXSRC)/lexicalizer_mngr.c incl/%_earley.h
	@test -d lib || mkdir lib
	$(CC_FLAGS) -c $(SXSRC)/lexicalizer_mngr.c -o $@ \
	-DLC_TABLES_H=\"$*_earley.h\"	\
	-DMULTI_ANCHOR			\
	-Dis_set_automaton		\
	-Dis_check_rhs_sequences

lib/%_earley_parser.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/varstr.h	\
			$(SXINCL)/earley.h	\
			$(SXINCL)/sxba.h	\
			$(SXSRC)/earley_parser.c	\
			incl/%_earley.h 
	@test -d lib || mkdir lib
	$(CC_FLAGS) -c $(SXSRC)/earley_parser.c -o $@	\
		-Dbeam_kind=16		\
		-DSXTTY_IS_SXSTDERR	\
		-DLC_TABLES_H=\"$*_earley.h\"	\
		-Dis_recognizer=1	\
		-Dis_parser=1		\
		-DSEMANTICS="sxsem_mngr_semact" \
		-Dis_rcvr=1		\
		-Dis_1LA=0		\
		-Dis_supertagging=0	\
		-Dis_lex=1

lib/sxsem_mngr.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/earley.h		\
                        $(SXINCL)/sxspf.h         	\
			$(SXINCL)/varstr.h		\
			$(SXSRC)/sxsem_mngr.c
	@test -d lib || mkdir lib
	$(CC_FLAGS) -c $(SXSRC)/sxsem_mngr.c -o lib/sxsem_mngr.o	\
		-DDEFAULT_MAP=\"n\"	\
		-DTIMEOUT_HANDLER	\
		-DMEMOFLW_HANDLER	\
		-DSXTRAP_HANDLER	\
		-Dnbest -Doutput

lib/enb_sxsem_mngr.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/earley.h		\
                        $(SXINCL)/sxspf.h         	\
                        $(SXINCL)/varstr.h         	\
			$(SXSRC)/sxsem_mngr.c
	@test -d lib || mkdir lib
	$(CC_FLAGS) -c $(SXSRC)/sxsem_mngr.c -o lib/enb_sxsem_mngr.o	\
		-DDEFAULT_MAP=\"nu\"	\
		-DTIMEOUT_HANDLER	\
		-DMEMOFLW_HANDLER	\
		-DSXTRAP_HANDLER	\
		-DLOG=1				\
		-DEBUG=1			\
		-Dexact_nbest -Dnbest -Doutput

# Test d'un algo qui apre`s nbest ne conserve ds la foret partagee que les nbest
lib/exact_nbest_semact.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/earley.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h		\
			$(SXINCL)/nbest.h	\
			$(SXSRC)/exact_nbest_semact.c
	@test -d lib || mkdir lib
	$(CC_FLAGS) -c $(SXSRC)/exact_nbest_semact.c -o lib/exact_nbest_semact.o	\
		-DMEASURES


#------------------------------------------------------------------
# Compilation des tables de l'analyseur
#------------------------------------------------------------------

lib/%_t.o: src/%_t.c
	@test -d lib || mkdir lib
	$(CC_FLAGS) -c $< -o $@ \
	-Dsxtables=T_tables \
        -DSCANACT_AND_SEMACT_ARE_IDENTICAL \
        -DSEMACT=sxivoid


#------------------------------------------------------------------
# Construction des tables de probabilites
#------------------------------------------------------------------

src/%_proba.c: spec/%.bnf $(SXBIN)/bnf2proba
	@test -d src || mkdir src
	$(SXBIN)/bnf2proba < $< > $@

lib/%_proba.o: src/%_proba.c
	@test -d lib || mkdir lib
	$(CC_FLAGS) -c $< -o $@






#------------------------------------------------------------------

clean:
	- rm -f lib/*.o
	- rm -f incl/*.h
	- rm -f spec/*.tlist spec/*.bt
	- rm -f src/*_proba.c
	- rm -f *.out


