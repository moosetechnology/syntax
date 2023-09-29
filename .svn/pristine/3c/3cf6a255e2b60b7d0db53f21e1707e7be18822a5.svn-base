sinclude(m4)dnl
define(`smart',ifdef(`archive',`',`skip'))dnl
define(`dummy',ifdef(`archive',`skip'))dnl
define(`archive',ifdef(`archive',$(BIB)($1),$1))dnl
define(`skip',`dnl')dnl
  ########################################################
  #                                                      #
  #                                                      #
  # Copyright (c) 1984 by Institut National de Recherche #
  #                    en Informatique et en Automatique #
  #                                                      #
  #                                                      #
  ########################################################




  ########################################################
  #                                                      #
  #  Produit de l'équipe Langages et Traducteurs. (phd)  #
  #                                                      #
  ########################################################


                 #    #  ######          #####     ##     ####
                 ##   #  #               #    #   #  #   #
                 # #  #  #####           #    #  #    #   ####
                 #  # #  #               #####   ######       #
                 #   ##  #               #       #    #  #    #
                 #    #  ######          #       #    #   ####


         #    #   ####   #####      #    ######     #    ######  #####
         ##  ##  #    #  #    #     #    #          #    #       #    #
         # ## #  #    #  #    #     #    #####      #    #####   #    #
         #    #  #    #  #    #     #    #          #    #       #####
         #    #  #    #  #    #     #    #          #    #       #   #
         #    #   ####   #####      #    #          #    ######  #    #


 #       ######          ######     #     ####   #    #     #    ######  #####
 #       #               #          #    #    #  #    #     #    #       #    #
 #       #####           #####      #    #       ######     #    #####   #    #
 #       #               #          #    #       #    #     #    #       #####
 #       #               #          #    #    #  #    #     #    #       #   #
 ######  ######          #          #     ####   #    #     #    ######  #    #

         #     #
         ##   ##    ##    #    #  ######  ######     #    #       ######
         # # # #   #  #   #   #   #       #          #    #       #
         #  #  #  #    #  ####    #####   #####      #    #       #####
         #     #  ######  #  #    #       #          #    #       #
         #     #  #    #  #   #   #       #          #    #       #
         #     #  #    #  #    #  ######  #          #    ######  ######



#########################################################################
# Historique des modifications, en ordre chronologique inverse:		#
#########################################################################
# 20030515 11:35 (phd):	Ajout de cette rubrique "modifications"		#
#########################################################################

# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(sx)
ETC=$(SX)/etc
SRC=$(ETC)/src
LIB=$(ETC)/lib
BIB=$(LIB)/libsxetc$(sxV).a
INCL=$(ETC)/incl
COMPILER=cc
CC=$(COMPILER) -I$(INCL) -I$(SX)/incl

FORCE=$(SX)/incl/sxunix.h $(ETC)/force_make

$(ETC)/everything .PRECIOUS:	$(BIB)


BNF_RWF=	archive(bnffree.o)	\
		archive(bnfread.o)	\
		archive(bnfwrite.o)
BSTR=		archive(bstr.o)
LECL_RWF=	archive(leclfree.o)	\
		archive(leclread.o)	\
		archive(leclwrite.o)
PARADIS_RWF=	archive(paradisfree.o)	\
		archive(paradisread.o)	\
		archive(paradiswrite.o)
PARSER_RWF=	archive(parserfree.o)	\
		archive(parserread.o)	\
		archive(parserwrite.o)
PRIO_RWF=	archive(priofree.o)	\
		archive(prioread.o)	\
		archive(priowrite.o)
RECOR_RWF=	archive(recorfree.o)	\
		archive(recorread.o)	\
		archive(recorwrite.o)
SEMAT_RWF=	archive(sematfree.o)	\
		archive(sematread.o)	\
		archive(sematwrite.o)

$(BIB): smart	$(FORCE)		\
		$(BNF_RWF)		\
		$(BSTR)			\
		$(LECL_RWF)		\
		$(PARADIS_RWF)		\
		$(PARSER_RWF)		\
		$(PRIO_RWF)		\
		$(RECOR_RWF)		\
		$(SEMAT_RWF)
dummy	$(AR) rv $@ $?
	cd $(LIB) ; $(RANLIB) $@


$(BNF_RWF)		:	$(FORCE)	$(INCL)/B_tables.h
$(BSTR)			:	$(FORCE)	$(INCL)/bstr.h
$(LECL_RWF)		:	$(FORCE)	$(INCL)/S_tables.h
$(PARADIS_RWF)		:	$(FORCE)	$(INCL)/PP_tables.h
$(PARSER_RWF)		:	$(FORCE)	$(INCL)/P_tables.h
$(PRIO_RWF)		:	$(FORCE)	$(INCL)/D_tables.h
$(RECOR_RWF)		:	$(FORCE)	$(INCL)/R_tables.h
$(SEMAT_RWF)		:	$(FORCE)	$(INCL)/T_tables.h

