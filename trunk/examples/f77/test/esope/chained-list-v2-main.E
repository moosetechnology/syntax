      program main
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
      pointeur list1.pstr
      pointeur list2.pstr

      write(*,*) 'main: begin'
      
* >> For simplicity all SEGMENTS are kept "activated" : no segact/segdes/actstr/desstr      

*     -- initialize ESOPE system
      call ooossg
      
*     -- create a list of n nodes
      call newlst(list1, 4)
      
*     -- print the list "list1"
      call prtlst(list1)
                
*     -- copy the list "list1" to "list2"
      call coplst(list1, list2)
            
*     -- delete the list "list1"
      call dellst(list1) 
      
*     -- print the list "list2"
      call prtlst(list2)
            
*     -- delete the list "list2"
      call dellst(list2) 

*     -- finalize ESOPE system
      call ooostp

      write(*,*) 'main: end'
      end
      
