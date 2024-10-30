       subroutine newusr(lib, name)
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

#include "user.seg"
c begin "user.seg"
c ubb(ubbcnt) : ordinal positions of books borrowed by the user 
      integer ubbcnt
      segment, user
       character*40 uname
       integer ubb(ubbcnt)
      end segment
c end "user.seg"

c arguments
       pointeur lib.PSTR
       character *(*) name 

c external functions
       external mypnt
       integer mypnt
          
c local variables       
       integer libeta
       integer  jord
       pointeur lb.tlib
       pointeur ur.user
     
       write(*,*) 'newusr: begin'

c activate the structure
       call oooeta(lib, libeta)
       call actstr(lib)   

c create a new book           
       ubbcnt = 0
       segini, ur
       ur.uname = name
       segdes, ur*MOD

c add the new book to the structure  
       call ajpnt(lib, ur,'user', 0, jord)

c update the head segment of the structure
       lb = mypnt(lib,1)
       segact, lb
       brcnt = lb.bref(/1)
       urcnt = lb.uref(/1)
       urcnt = urcnt + 1
       segadj, lb
       lb.uref(urcnt) = jord
       segdes, lb*MOD

c deactivate the structure if activated on entry
       if(libeta.ne.1) call desstr(lib,'MOD') 

       write(*,*) 'newusr: end'
       end 
