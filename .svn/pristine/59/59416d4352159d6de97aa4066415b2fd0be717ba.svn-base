Hubert Garavel - Wed Feb 15 12:18:34 CET 2023

Le fichier lecld_main.c n'a pas ete maintenu depuis 2008 et ne compile plus 
en 2023.

lecl/src/lecld_main.c:84:14: warning: function declaration isn't a prototype [-Wstrict-prototypes]
lecl/src/lecld_main.c:102:14: warning: function declaration isn't a prototype [-Wstrict-prototypes]
lecl/src/lecld_main.c:117:15: warning: function declaration isn't a prototype [-Wstrict-prototypes]
lecl/src/lecld_main.c:117:15: error: 'language_name' redeclared as different kind of symbol
lecl/src/lecld_main.c:135:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
lecl/src/lecld_main.c:135:15: error: conflicting types for 'mkstemp'
lecl/src/lecld_main.c:137:15: warning: function declaration isn't a prototype [-Wstrict-prototypes]
lecl/src/lecld_main.c:144:7: warning: declaration of 'pathname' shadows a parameter [-Wshadow]
lecl/src/lecld_main.c:206:6: error: too few arguments to function 'sxexit'
lecl/src/lecld_main.c:215:7: error: too few arguments to function 'SXDS->scanact'
lecl/src/lecld_main.c:220:7: error: too few arguments to function 'SXDS->scanact'
lecl/src/lecld_main.c:233:3: error: too few arguments to function 'sxexit'
lecl/src/lecld_main.c:240:6: error: too few arguments to function 'sxexit'
lecl/src/lecld_main.c:246:6: error: too few arguments to function 'sxexit'
lecl/src/lecld_main.c:252:7: error: too few arguments to function 'SXDS->scanact'
lecl/src/lecld_main.c:257:7: error: too few arguments to function 'SXDS->scanact'
lecl/src/lecld_main.c:370:7: warning: function declaration isn't a prototype [-Wstrict-prototypes]

------------------------------------------------------------------------------

Le code de lecld se trouvait originellement dans trunk/lecl.

La partie du Makefile utilise pour compiler lecld etait la suivante (mais la
compilation etait desactivee par Pierre Boullier) :

LECLD=          $(LIB)/lecld_main.o

$(SXAUX)/lecld.out:	$(LECLD)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(LECLD) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/lecld_main.o:	$(SXINCL)/sxdynam_scanner.h	\
			$(SXINCL)/util/B_tables.h	\
			$(SRC)/lecld_main.c
	$(CC) $(SRC)/lecld_main.c

