      subroutine newlst(list, n)
*     -- create a list of "n" nodes
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
      integer n

      pointeur head.node
      integer i
      real newval
      integer iord
      integer jord

      write(*,*) '------------------'
      write(*,*) 'newlst: begin'
      
*     -- create an empty list: begin
      call inistr(list, 'list', 0, 0)

*     >> false node used to store the reference of the head
      segini, head
      head.value = -999
      head.next = 0

*     >> force the storage position "iord"
      iord = 1
      call ajpnt(list, head, 'node', iord, jord)
*     -- create an empty list: end
      
      do i = 1, n
        newval = 500. + real(i)
        call addlst(list, newval)
      enddo
      
      write(*,*) 'newlst: end'
      end

