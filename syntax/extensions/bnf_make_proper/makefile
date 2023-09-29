
all:
	# to be provided

# debut de code produit par "make-makefile make_proper"

make_proper_t.c: make_proper.bt make_proper.st make_proper.pt make_proper.rt 
	tables_c make_proper > $@

make_proper.bt  : make_proper.bnf
	semact make_proper.bnf 

make_proper.st: make_proper.bt make_proper.lecl
	lecl make_proper.lecl || [ $$? = 1 ]

make_proper.pt: make_proper.bt
	csynt make_proper || [ $$? = 1 ]

make_proper.rt: make_proper.bt make_proper.st make_proper.recor
	recor make_proper.recor

# fin de code produit par "make-makefile make_proper"

clean:
	# to be provided
