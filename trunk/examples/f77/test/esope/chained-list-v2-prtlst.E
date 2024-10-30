      subroutine prtlst(list)
*     -- print a list of nodes
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

      pointeur head.node
      pointeur some.node
      integer next_ord

      write(*,*) '------------------'
      write(*,*) 'prtlst: begin'

      head = mypnt(list, 1)
      write(*,*)
      write(*,'(a, i6.6)') 'head = ', head
      write(*,'(a, f5.0)') '    value = ', head.value
      write(*,'(a, i3.3)') '     next = ', head.next

      next_ord = head.next

10    if (next_ord .ne. 0) then
        some = mypnt(list, next_ord)
        write(*,*)
        write(*,'(a, i6.6)') 'node = ', some
        write(*,'(a, f4.0)') '    value = ', some.value
        write(*,'(a, i3.3)') '     next = ', some.next
        next_ord = some.next
        segdes, some
        goto 10
      endif
      
      write(*,*) 'prtlst: end'
      end
