      subroutine prtlib(lib)
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
      pointeur lib.pstr

c external functions
      external mypnt
      integer mypnt

c local variables     
      pointeur lb.tlib
      pointeur bk.book
      pointeur ur.user
      integer libeta

c ibk index of a book
c iur index of a user
c ibor index of borrowed book
      integer ibk, iur, ibor

      write(*,*) 'prtlib: begin'

      call oooeta(lib, libeta)
      call actstr(lib)
      
      lb = mypnt(lib,1)
      segact, lb  
  
      brcnt = lb.bref(/1)
      urcnt = lb.uref(/1) 
  
      write(*,*)
      write(*,*) '-- books --'
      do ibk=1, brcnt
        bk = mypnt(lib, lb.bref(ibk))
        segact, bk
        write(*,*) 
        write(*,*)'book: index = ', ibk 
        write(*,*)'..... title = ', bk.btitle 
        write(*,*)'..... pages = ', bk.bpages 
        write(*,*)'.....   udc = ', bk.budc 
        segdes, bk*NOMOD
      end do

      write(*,*)
      write(*,*) '-- users --'
      do iur=1, urcnt
        ur = mypnt(lib,lb.uref(iur))
        segact, ur
        write(*,*) 
        write(*,*)'user:  index = ', iur
        write(*,*)'....... name = ', ur.uname

c print the books borrowed by the user
        ubbcnt = ur.ubb(/1)
        do ibor=1,ubbcnt
          bk = mypnt(lib,lb.bref(ur.ubb(ibor)))
          segact, bk
          write(*,*) 
          write(*,*) '>> borrowed book: title = ', bk.btitle
          write(*,*) '..................pages = ', bk.bpages 
          write(*,*) '..................  udc = ', bk.budc 
          segdes, bk*NOMOD
        end do

        segdes, ur*NOMOD
      end do

c print the user who borrowed books and the indices of borrowed books    
      write(*,*)
      write(*,*) '-- borrowed books by users --'
      do iur=1, urcnt
        ur = mypnt(lib,lb.uref(iur))
        segact, ur
        ubbcnt = ur.ubb(/1)
          do ibor=1, ubbcnt
            write(*,*) 
            write(*,*)'user:    index = ', iur 
            write(*,*)'..........name = ', ur.uname
            write(*,*)'... book index = ', ur.ubb(ibor)
          end do     
        segdes, ur*NOMOD
      end do     

c desactivate head segment
      segdes, lb*NOMOD

c deactivate the structure if activated on entry
       if(libeta.ne.1) call desstr(lib,'MOD') 

       write(*,*) 'prtlib: end'

      end

