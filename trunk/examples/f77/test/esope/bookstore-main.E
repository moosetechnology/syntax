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

c local variables
      pointeur lib.PSTR
      integer libunt
      character*8 libfln

      character*(40) line

      line = '----------------------------------------'
      write(*,*) line
      write(*,*) 'main: begin'

      call ooossg

      write(*,*) line
      write(*,*) 'main: Create an empty library structure'
      call newlib(lib)

      write(*,*) line
      write(*,*) 'main: Create books'
      call newbk(lib,'Le petit chaperon rouge',10,2.12)
      call newbk(lib,'Croc blanc',100,4.15)

      write(*,*) line
      write(*,*) 'main: Create users'
      call newusr(lib,'Emilie')
      call newusr(lib,'Mike')

      write(*,*) line
      write(*,*) 'main: Print inventory before any transaction'
      call prtlib (lib)

      write(*,*) line
      write(*,*) 'main: Perform a transaction and print inventory'
      call borbk (lib,'Mike', 'Croc blanc')
      call prtlib (lib)

      write(*,*) line
      write(*,*) 'main: Perform a transaction and print inventory'
      call relbk(lib, 'Mike', 'Croc blanc')
      call prtlib (lib)

      write(*,*) line
      write(*,*) 'main: Store the library on a file'
      libunt = 1
      libfln = 'lib.data'
      open(unit=libunt, file=libfln,
     &access='sequential', form='unformatted', status='new')

      call wrstr(lib, libunt)
      close(unit=libunt, status='keep')

      write(*,*) line
      write(*,*) 'main: Delete the library'
      call supstr(lib)
      lib = 0


      write(*,*) line
      write(*,*) 'main: Load the library from a file'
      libunt = 1
      libfln = 'lib.data'
      open(unit=libunt, file=libfln,
     &access='sequential', form='unformatted', status='old')

      call rdstr(lib, libunt)

      close(unit=libunt, status='keep')

      write(*,*) line
      write(*,*) 'main: Print inventory'
      call prtlib (lib)

      write(*,*) line
      write(*,*) 'main: Delete the library'
      call supstr(lib)
      lib = 0

      call ooostp

      write(*,*) 'main: end'
      end

