
Fri Apr 28 12:56:04 CEST 2023

This example was provided by the VASY and CONVECS teams 
(https://vasy.inria.fr and https://convecs.inria.fr).

It is an example of a compiler front-end generated using SYNTAX for a subset 
of the LUSTRE synchronous language.

This example was first written in the late 80's for an old version of the
LUSTRE language. In 2023, this example was revised to better follow the
definition of LUSTRE, to simplify the LECL file, to use SEMC instead of
SEMAT, and to dump in XML an abstract syntax tree for the LUSTRE program
given as input.

This example shows how SEMC can be use to (concisely and efficiently) produce
an XML file, directly from the BNF grammar, without the burden of defining,
building, storing in memory, and pretty-printing an abstract syntax tree.

The makefile invokes all the necessary SYNTAX tools to build the compiler
front-end.

Some examples of LUSTRE programs are given in the test/*.lus files.

