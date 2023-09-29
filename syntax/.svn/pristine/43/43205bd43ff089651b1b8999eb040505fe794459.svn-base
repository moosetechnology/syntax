
Hubert Garavel -- Mon Jun  5 10:25:33 CEST 2023

This directory contains files and scripts intended for
compiling SYNTAX using the GNU autotools, rather than using
the custom build system designed for SYNTAX. Unfortunately,
we could not boostrap SYNTAX in 2023 using the autotools
approach (for instance, the bnf processor would not compile,
because there was no provision for passing the option
-Dsxtables=bnf_tables specified in trunk/bnf/makefile). 

As of June 2023, the autotools/autoconf approach is largely
out-of-sync with the recent versions of SYNTAX in which the
"autogenese" approach has been merged with normal makefiles.




-------------------------------------------------------------------------------
Procédure pour compiler une version mono-architecture en utilisant autotools
--------------------------------------------------------------------------------

1) Si on veut utiliser les autotools, alors il faut d'abord installer les
   paquetages correspondants. Par exemple, sous Debian bullseye, il faut
   installer
	apt install m4
	apt install automake

     Puis il faut éxécuter cette suite de commandes :
	aclocal
	-- Generate 'aclocal.m4' by scanning 'configure.ac'

	autoconf
	-- Generate 'configure' from 'configure.ac'

	automake -af
	-- Generate Makefile.in for configure from Makefile.am.
	-- option -a: add missing standard files to package
	-- option -f: force update of standard files

2) ./configure --prefix=`pwd`

3) make && make install

4) Si on veut utiliser make-makefile, ou un autre programme de Vasy qui attend
   une structure particulière, il faut créer des liens manuellement (dans le
   répertoire de Syntax) :
     ln -s bin bin.`$SX/etc/bin/arch`
     ln -s lib lib.`$SX/etc/bin/arch`

Note: cette approche ne fonctionne pas pour win32, car elle ne transmet pas
les options -I$CADP/incl -DVASY_LIB et -L$CADP/bin.win32 -lm ; voir la section
suivante qui prend ceci en compte.

--------------------------------------------------------------------------------

1. To build SYNTAX using the autotool, one needs to restore in trunk/etc/bin
   the script "liens-tmp" (not to be confused with "liens-tmp.sh") that has
   been moved to extensions/etc/bin.

2. To build SYNTAX using the autotool, one needs to restore in trunk/etc/bin
   the script "menage-tmp" (not to be confused with "menage-tmp.sh") that has
   been moved to extensions/etc/bin.

