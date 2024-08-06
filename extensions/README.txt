Hubert Garavel - Fri Apr 19 09:43:46 CEST 2024

The 'extensions' directory contains files that were originally in the
'trunk' directory, but have been set apart in January 2023 because it
was felt that they were auxiliary software components, not mandatory
for compiler construction, but rather intended to analysis of natural
languages. The contents of the 'extensions' directory be be inferred
as follows.

bin:
	This directory contains scripts that have been moved from
	trunk/bin to here.

bnf:
	This directory contains Perl scripts that have been moved
	from trunk/bnf to here, and an extended version of bnf
	(dedicated to natural language processing) in which the
	-huge option is active. This -huge option is meant to process
	automatically-generated grammars with tenths of thousands
	of rules and defining an Earley-like parser. This option
	(as far as Pierre Boullier can tell) mostly suppresses
	calculations done by the "standard" BNF, which would be
	too costly to do on large grammars.

DAG:
	This directory contains the 'dag' processor. No documentation
	was found for this processor.

csynt_lc:
	See the README file in the csynt_lc directory.

dico:
	This directory contains a constructor to build dictionaries.
	No documentation was found for this processor.

doc:
	This directory contains documentation files that have been moved
	from trunk/doc to here.

dummy_csynt:
	This directory contains a processor implementing a CSYNT-processor 
	variant that does nothing. No documentation was found for this
	processor.

examples:
	This directory contains examples files that have been moved from
	trunk/examples to here.

ext:
	This directory contains C source files that have been moved from
	trunk/src to here. These components are not needed for deterministic
	parsing.

incl:
	This directory contains C include files that have been moved from
	trunk/incl to here.

lecld:
	This directory contains C code fragment for a companion tool of
	LECL named "lecld" (presumably, LECL using a dynamic scanner). The
	makefile disallowed the compilation of this tool and, moreover, the
	code no longer compiles in 2023.

lfg:
	See the README file in the lfg directory.

make_a_dico:
	This directory contains a constructor of compact dictionaries,
	which was introduced in SYNTAX 6.0 (2008). No documentation was
	found for this processor.

rcg:
	This directory contains processors (rcg and drcg) for (Definite)
	Range Concatenation Grammars. No documentation was found for these
	processors.

xcsynt:
	This directory apparently contains a variant of CSYNT that can
	handle (deterministically) a wider class of context-free languages,
	especially RLR languages. No documentation was found for this
	processor. Ideally, its features should be merged in CSYNT rather
	than being in a separate processor. This tool triggers worrying
	compiler warnings from GCC and fatal error from SunCC (see file
	xcsynt/README.txt).

yax:
	This directory contains a processor (yax) that accepts a BNF grammar
	that contains semantic definitions "a la YACC" on top of syntactic
	definitions "a la SYNTAX". This processor was introduced in version
	2.0 of SYNTAX by Philippe Deschamp. It was seldom used and, in
	practical applications, is often replaced by the TABC processor
	(which is called SEMC at present).

ysx:
	This directory contains a conversion tool from YACC sources to SYNTAX
	specifications. A manual page exists in extensions/doc/man/ysx.1.
	This conversion tool has not been updated since 1987 and is probably
	not up-to-date with respect to the current YACC format.

To compile all or part of these components, move them back to the trunk/
directory and uncomment them in trunk/src/makefile.

