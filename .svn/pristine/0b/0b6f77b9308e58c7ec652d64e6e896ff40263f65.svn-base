
include $(SX)/etc/make/gccoptions

all .PRECIOUS:	ppc1.spec ppc2.spec ppc3.spec				\
		ppc1.paradis ppc1.lecl ppc1.prio ppc1.recor ppc1.tdef	\
		ppc2.paradis ppc2.lecl ppc2.prio ppc2.recor ppc2.tdef	\
		ppc3.paradis ppc3.lecl ppc3.prio ppc3.recor ppc3.tdef

.SUFFIXES: .recor .prio .tdef .lecl .paradis .spec

.spec.paradis:
	sed -e '1,/^%Syntactic_Specification/d' -e '/^%%/,\$$d' < $*.spec | cmp -s - $*.paradis || (sed -e '1,/^%Syntactic_Specification/d' -e '/^%%/,\$$d' < $*.spec > $*.paradis) && touch -r $< $@

.spec.lecl:
	sed -e '1,/^%Lexical_Specification/d' -e '/^%%/,\$$d' < $*.spec | cmp -s - $*.lecl || (sed -e '1,/^%Lexical_Specification/d' -e '/^%%/,\$$d' < $*.spec > $*.lecl) && touch -r $< $@

.spec.tdef:
	sed -e '1,/^%Terminal_Define/d' -e '/^%%/,\$$d' < $*.spec | cmp -s - $*.tdef || (sed -e '1,/^%Terminal_Define/d' -e '/^%%/,\$$d' < $*.spec > $*.tdef) && touch -r $< $@

.spec.prio:
	(sed -e '1,/^%Priorities/d' -e '/^%%/,\$$d' < $*.spec; sed -e '/%prec/!d' < $*.spec) | cmp -s - $*.prio || ((sed -e '1,/^%Priorities/d' -e '/^%%/,\$$d' < $*.spec; sed -e '/%prec/!d' < $*.spec) > $*.prio) && touch -r $< $@

.spec.recor:
	sed -e '1,/^%Error_Recovery/d' -e '/^%%/,\$$d' < $*.spec | cmp -s - $*.recor || (sed -e '1,/^%Error_Recovery/d' -e '/^%%/,\$$d' < $*.spec > $*.recor) && touch -r $< $@


CPP=gcc -E -C -traditional -P -x c

ppc1.spec:	ppc123.spec
	@sed -e 's+^\(#.*\)/\*.*+\1+' $? | ${CPP} -DPPC1 - > $@ && touch -r $? $@

ppc2.spec:	ppc123.spec
	@sed -e 's+^\(#.*\)/\*.*+\1+' $? | ${CPP} -DPPC2 - > $@ && touch -r $? $@

ppc3.spec:	ppc123.spec
	@sed -e 's+^\(#.*\)/\*.*+\1+' $? | ${CPP} -DPPC3 - > $@ && touch -r $? $@
