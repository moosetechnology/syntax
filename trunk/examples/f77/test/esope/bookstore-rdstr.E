      subroutine rdstr(pstr, strunt)

c     -- Load a instance of "pstr" from a file referenced by "strunt"
c     >> Simplified version of the so-called "SAHPHTOOL ARCHIVEUR"

      implicit none

#include <xseg.seg>
c begin "xseg.seg"
c     -- segment generique pour manipule la memoire de tout segment
      integer xsize
      segment,xseg
        integer xdata(xsize)  
      endsegment
c end "xseg.seg"

c arguments
c     >> [#include "PSTR.inc"] is omitted because unneeded and because it generates name conflict
c     >> [pstr.pstr] acts as a generic pointer
      pointeur pstr.pstr

      integer strunt

c local variables
      integer streta

      pointeur pseg.xseg
      integer  pdim
      integer  pdeb
      integer  pfin
      integer  pindx

      integer  iord
      integer  jord

      integer  iseg
      integer  nbseg
      integer  nblnk
      character*8 typseg

      character*8 typpnt
      external typpnt

      integer mypnt
      external mypnt


      write(*,*) 'rdstr: begin'

c     -- load the head of the structure
      read(strunt) pdim, pdeb, pfin
      xsize = pdim
      segini, pseg
      read(strunt) (pseg.xdata(pindx), pindx=pdeb, pfin)
      pstr = pseg

c     -- load the elements of the structure
      read(strunt) nbseg, nblnk

      do 10, iseg=1, nbseg
        iord = iseg
        pseg = mypnt(pstr, iord)
        if ( pseg .ne. 0 ) then
            read(strunt) pdim, pdeb, pfin
            xsize = pdim
            segini, pseg
            read(strunt) (pseg.xdata(pindx), pindx=pdeb, pfin)

            typseg = typpnt(pstr, iord)
            call ajpnt(pstr, pseg, typseg, iord, jord)
            segdes, pseg*MOD
        endif
 10   continue

      if ( nblnk .ne. 0 ) then
        write(*,*) 'rdstr: failed ; nblnk .ne. 0'
        stop
      endif

c     -- deactivate the structure if activated on entry
      call desstr(pstr,'MOD')

      write(*,*) 'rdstr: end'

      end
