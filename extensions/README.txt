Hubert Garavel - Tue Sep 17 11:59:35 CEST 2024

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

dag:
	This directory contains a grammar needed to build the file
	extensions/ext/dag_t.c. 'dag' means 'directed acyclic graph'.
	Here, dag denotes a restricted form of regular expression, in
	which the repetition operators {...} and {...}+ are absent.

dags2udag:
	This directory contains a translator. No documentation was found
	for this processor.

csynt_lc:
	See the README file in the csynt_lc directory.

dico:
	This directory contains a constructor to build dictionaries.
	No documentation was found for this processor.
doc:
	This directory contains documentation files that have been moved
	from trunk/doc to here.

drcg:
	This directory contains a processor drcg for Definite Range
	Concatenation Grammars. This processor is a variant of the rcg
	processor. According to Pierre Boullier, drcg was an attempt at
	embedding PROLOG in Range Concatenation Grammars. Originally,
	drcg was in the same directory as rcg, but was separated from it
	in 2024 for clarity.

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

lfg_lex:
	An undocumented variant of lfg (originally in the same directory
	as lfg, but separated from lfg in 2024, for clarity).

make_a_dico:
	This directory contains a constructor of compact dictionaries,
	which was introduced in SYNTAX 6.0 (2008). No documentation was
	found for this processor.

re:
	This directory contains a grammar needed to build the file
	extensions/ext/re_t.c ('re' means 'regular expression').

rcg:
	This directory contains a processor named rcg for Range Concatenation
	Grammars.

sdag:
	This directory contains a grammar needed to build the file
	extensions/ext/sdag_t.c. 'sdag' means 'simple directed acyclic
	graph'. A sdag represent a particular case of dag: each terminal
	of a 'normal' source string is a list of alternatives denoting
	a simple type of ambiguities.

test:
	This directory contains some programs whose sole purpose seems
	to exercise the SYNTAX libraries.

udag:
	This directory contains a grammar needed to build the file
	extensions/ext/udag_t.c. 'udag' means 'unfolded directed acyclic
	graph'. A udag represents a dag as a list of triples (index of
	source state, transition, index of target state).

xcsynt:
	This directory apparently contains a variant of CSYNT that can
	handle (deterministically) a wider class of context-free languages,
	especially RLR languages. No documentation was found for this
	processor. Ideally, its features should be merged in CSYNT rather
	than being in a separate processor. This tool triggers worrying
	compiler warnings from GCC and fatal error from SunCC (see file
	xcsynt/README.txt).

To compile all or part of these components, move them back to the trunk/
directory and uncomment them in trunk/src/makefile.

