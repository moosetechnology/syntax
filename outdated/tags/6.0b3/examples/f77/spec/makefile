# This makefile is used to bootstrap f77.

# To avoid problems with other shells:
SHELL=/bin/sh

L=f77
ppL=ppf77

all SYNTAX-produced tables are .PRECIOUS: ${L}_t.c ${L}_td.h ${ppL}_t.c 

${L}.bt:	${L}.bnf
	bnf -rhs 11 ${L}.bnf

${L}_td.h:	${L}.bt ${L}.tdef
	tdef ${L}.tdef > ${L}_td.h

${L}.dt:	${L}.bt ${L}.prio
	prio ${L}.prio

${L}.pt:	${L}.bt ${L}.dt
	csynt ${L}

${L}.st:	${L}.bt ${L}.lecl
	- lecl ${L}.lecl

${L}.rt:	${L}.bt ${L}.st ${L}.recor
	recor ${L}.recor

${L}_t.c:	${L}.st ${L}.pt ${L}.rt
	tables_c ${L} > ${L}_t.c


${ppL}.bt ${ppL}.ppt:	${ppL}.paradis
	paradis -rhs 11 ${ppL}.paradis

${ppL}.dt:	${ppL}.bt ${ppL}.prio
	prio ${ppL}.prio

${ppL}.pt:	${ppL}.bt ${ppL}.dt
	csynt ${ppL}

${ppL}.st:	${ppL}.bt ${ppL}.lecl
	- lecl ${ppL}.lecl

${ppL}.rt:	${ppL}.bt ${ppL}.st ${ppL}.recor
	recor ${ppL}.recor

${ppL}_t.c:	${ppL}.st ${ppL}.pt ${ppL}.rt ${ppL}.ppt
	tables_c ${ppL} > ${ppL}_t.c
