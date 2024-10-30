       subroutine borbk(lib, name, title)
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
       character *(*) title

c external functions
       external mypnt
       integer mypnt
       
       external fndbk 
       integer fndbk
       
       external fndur
       integer fndur

c local variables
       integer libeta
       
       pointeur lb.tlib
       pointeur ur.user
       
       integer ibk
       integer iur
       
       write(*,*) 'borbk: begin'

c see whether the user exists in the structure       
       iur = fndur(lib, name)
       if (iur .eq. 0) then 
         write(*,*) 'cannot find user ', name
         return
       endif 
       write(*,*) 'borbk: iur =', iur
       
c see whether the book exists in the structure       
       ibk = fndbk(lib, title)
       if (ibk .eq. 0) then 
         write(*,*) 'cannot find book ', title
         return
       endif 
       write(*,*) 'borbk: ibk=', ibk

c activate the structure
       call oooeta(lib, libeta)
       call actstr(lib)
       
       lb = mypnt(lib,1)
       segact, lb
       brcnt = lb.bref(/1)
       urcnt = lb.uref(/1) 

c update borrowed status of the book
       if (lb.bstatu(ibk)) then
         write(*,*) 'cannot borrow an already borrowed book ', title
         return
       else
         lb.bstatu(ibk) = .true.
       endif     

c update the borrowed books by the user
       ur = mypnt(lib, lb.uref(iur))    
       segact, ur
       ubbcnt = ur.ubb(/1)
       ubbcnt = ubbcnt + 1
       segadj, ur
       ur.ubb(ubbcnt) = ibk       
       segdes, ur*MOD

       segdes,lb*MOD

c deactivate the structure if activated on entry
       if(libeta.ne.1) call desstr(lib,'MOD') 
       write(*,*) 'borbk: end'
                    
       end 
