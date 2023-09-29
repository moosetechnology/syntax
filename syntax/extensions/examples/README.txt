Wendelin Serwe - Fri Sep 21 09:51:18 CEST 2018

Pour compiler les exemples de la distribution Syntax qui utilisent les outils
de compilation GNU (autoconf, configure, make, ...) :

1) Certains outils utilisent libsx.debug.a au lieu de libsx.a, il convient donc
   de creer un lien symbolique :
	cd $SX/lib/syntax
	ln -s libsx.a libsx.debug.a

2) Il faut aussi que les binaires de Syntax ($SX/bin) soient dans $PATH.

3) Globalement, il faut suivre les instructions de compilation pour Syntax,
   decrite dans $SX/vasy/PROCEDURES (ci-dessus), mais avec des petits ajouts :

	cd $SX/examples/<???>

	aclocal
	# ajout de $SX/syntax.m4 a la fin de aclocal.m4
	cat $SX/syntax.m4 >> aclocal.m4
	autoconf
	automake -af
	# on installe dans $SX, et on cherche les binaire Syntax dans $SX
	./configure --prefix=$SX --with-syntax-prefix=$SX
	make
	make install


