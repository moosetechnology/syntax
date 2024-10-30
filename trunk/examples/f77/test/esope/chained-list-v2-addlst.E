      subroutine addlst(list, newval)
*     -- add "value" to the list
      implicit none
#include "PSTR.inc"
c begin "PSTR.inc"
*-PSTR
*:PSTR    SEGMENT D'ENTETE DES STRUCTURES : NOUVELLE DEFINITION
*:NOMCOD  NOM DU CODE AUQUEL APPARTIENT LA STRUCTURE (CRONOS, APOLLO ..)
*:DATE    DATE DE DEBUT DE JOB
*:HEURE   HEURE DE DEBUT DE JOB
*:TYPSEG  TYPE DES SEGMENTS APPARTENANTS A LA STRUCTURE
*:TYPREF  TYPE DES OBJETS REFERENCES PAR LA STRUCTURE
*:IBSEG   NUMERO MAX. DU SEGMENT AJOUTE A LA STRUCTURE
*:IBREF   NUMERO MAX. DE L'OBJET REFERENCE PAR LA STRUCTURE
*:PNTSEG  POINTEURS DES SEGMENTS APPARTENANTS A LA STRUCTURE
*:PNTREF  POINTEURS DES OBJETS REFERENCES PAR LA STRUCTURE
      SEGMENT , PSTR
        CHARACTER*8 NOMCOD, DATE, HEURE, TYPSEG(NBSEG), TYPREF(NBREF)
        INTEGER     IBSEG, IBREF,        PNTSEG(NBSEG), PNTREF(NBREF)
      ENDSEGMENT
*----------------------------------------------------------------------
c end "PSTR.inc"
#include "node.seg"
c begin "node.seg"
      segment, node
        real value
        integer next
      endsegment
c end "node.seg"

      pointeur list.pstr
      real newval

      pointeur head.node
      pointeur some.node

      integer jord

      write(*,*) '------------------'
      write(*,*) 'addlst: begin'

      head = mypnt(list, 1)
      
      segini, some
      some.value = newval
      some.next = head.next
      
*     >> store "some" at a free position "jord" (being returned)
      call ajpnt(list, some, 'node', 0, jord)

*     >> update the reference to the head
      head.next = jord
     
      write(*,*) 'addlst: end'
      end

