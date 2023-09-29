
This example is based on SIMPROC, a small procedural language used by Didier
Parigot in 1991 as a test case for the FNC-2 attribute grammar system. A
presentation of this example can be found here (see Annex B):
    https://vasy.inria.fr/publications/Vivien-96.html

This example shows how SYNTAX and TRAIAN can be used to implement a compiler
for SIMPROC, including parsing, abstract tree construction and C code
generation. 

The lexical and syntactic analysis parts of the SIMPROC compiler are specified
and implemented using the SYNTAX compiler-generation tool, which can be
downloaded from https://sourcesup.renater.fr/projects/syntax.

The semantic analysis and code generation parts of the SIMPROC compiler are
specified in the LOTOS NT language, which is then translated to C using the
TRAIAN compiler, which is available from https://vasy.inria.fr/traian.

This directory contains the following files:

   - SYNTAX source code

        simproc.lecl    :       lexical grammar
        simproc.semc    :       concrete syntax grammar with semantic actions
				to construct the abstract syntax tree
        simproc.recor   :       syntax error recovery patterns
        simproc.prio    :       resolution of syntax conflicts (here empty)

   - LOTOS NT source code

        simproc.lnt     :       definition of abstract syntax tree, semantic
				checks, and C code generation

   - C code

        simproc.fnt     :       a few functions declared as "external" as
				simproc.lnt and written directly in C
	main.c          :       main program ("glue" between SYNTAX and TRAIAN)

The sub-directory "test" contains a few examples of SIMPROC programs. Some
examples (e.g., "fact-error.sim" and "term-error.sim") are intentionally bogus.

1. If both SYNTAX and TRAIAN are installed, one can build the SIMPROC
   compiler by typing at the shell prompt:

 	make SX=/usr/local/syntax

   where $SX is set to the "trunk" directory in which SYNTAX is installed,
   and where the environment variable $LNTDIR should be set to the directory
   in which TRAIAN is installed. This generates an executable program named
   "simproc".

   If either SYNTAX or TRAIAN is not installed, typing this command will
   only perform the steps feasible without the missing tool, and will not
   generate the "simproc" program.

2. The SIMPROC compiler can be tested by typing:

	sh run-test

   The corresponding outputs are stored in the sub-directory test/logs.

3. Cleanup of the current directory can be obtained by typing:

	make clean

[Last updated: 2023/05/26]

