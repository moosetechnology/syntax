      subroutine wrstr(pstr, strunt)

c     -- Store a instance of "pstr" into a file referenced by "strunt"
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
      integer  pidx

      integer  iord

      integer  iseg
      integer  nbseg
      integer  nblnk


      write(*,*) 'wrstr: begin'

c     -- activate the structure
      call oooeta(pstr, streta)
      call actstr(pstr)

c     -- store the head of the structure
      pseg = pstr
      call oooxln(pseg, pdim, pdeb, pfin)
      write(strunt) pdim, pdeb, pfin
      write(strunt) (pseg.xdata(pidx), pidx=pdeb, pfin)


c     -- store the elements of the structure
      call dimstr(pstr, nbseg, nblnk)
      write(strunt) nbseg, nblnk

      do 10, iseg=1, nbseg
        iord = iseg
        pseg = mypnt(pstr, iord)
        if ( pseg .ne. 0 ) then
            segact, pseg
            call oooxln(pseg, pdim, pdeb, pfin)
            write(strunt) pdim, pdeb, pfin
            write(strunt) (pseg.xdata(pidx), pidx=pdeb, pfin)
            segdes, pseg*NOMOD
        endif
 10   continue

      if ( nblnk .ne. 0 ) then
        write(*,*) 'wrstr: failed ; nblnk .ne. 0'
        stop
      endif

c     -- deactivate the structure if activated on entry
      if(streta.ne.1) call desstr(pstr,'MOD')

      write(*,*) 'wrstr: end'

      end
