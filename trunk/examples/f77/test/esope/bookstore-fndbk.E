      integer function fndbk(lib, title)
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

#include "tlib.seg"
c begin "tlib.seg"
c data structure for modeling a book library
c segment de tete
c bref(brcnt) : ordinal position in PSTR of book segments 
c uref(urcnt) : ordinal positions in PSTR of user segments
c bstatu(brcnt) : borrowed status of books 
      integer brcnt
      integer urcnt
      
      segment, tlib
       integer bref(brcnt)
       logical bstatu(brcnt)
       integer uref(urcnt)
      end segment
c end "tlib.seg"

#include "book.seg"
c begin "book.seg"
      segment, book
       character*40 btitle
       integer bpages
       real budc
      end segment
c end "book.seg"

c arguments
      pointeur lib.pstr
      character*(*) title

c external functions
      external mypnt
      integer mypnt

     
c local variables
      integer libeta
      
      pointeur bk.book
      pointeur lb.tlib
      
      integer ibk
      integer ibk2
      character*200 title1
      character*200 title2

      write(*,*) 'fndbk: begin'

      call oooeta(lib, libeta)
      call actstr(lib)
      
      lb = mypnt(lib,1)
      segact, lb       
      urcnt = lb.uref(/1)
      brcnt = lb.bref(/1)

      title1 = title  
      ibk = 0
      do ibk2 = 1, brcnt
        bk = mypnt(lib, lb.bref(ibk2))
        segact, bk
        title2 = bk.btitle
        segdes, bk*NOMOD
        if (title2 .eq. title1) then
          ibk = ibk2
          goto 100
        end if
      end do

 100  continue
      segdes, lb*NOMOD

      fndbk = ibk
 
c deactivate the structure if activated on entry
      if(libeta.ne.1) call desstr(lib,'MOD') 

      write(*,*) 'fndbk: end'
 
      end
