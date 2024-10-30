      program main
      implicit none
      
#include "bar.seg"
c begin "bar.seg"
      integer idim
      integer jdim

      segment, bar
        integer idata(idim)
        integer jdata(jdim + 1, jdim + 2)
      endsegment
c end "bar.seg"

      pointeur p.bar
      integer i
      integer j
      
      write(*,*) 'main: begin'

* >> For simplicity all SEGMENTS are kept "activated" : no segact/segdes      

*     -- initialize ESOPE system
      call ooossg

*     -- create segment
      idim =  3
      jdim =  5
      
      write(*, '(1x, a, i1)') 'idim = ', idim
      write(*, '(1x, a, i1)') 'jdim = ', jdim
 
      segini, p

      write(*, '(1x, a, i1)') 'p.idata(/1) = ', p.idata(/1)
      write(*, '(1x, a, i1)') 'p.jdata(/1) = ', p.jdata(/1)
      write(*, '(1x, a, i1)') 'p.jdata(/2) = ', p.jdata(/2)
      
      do i=1, idim
        p.idata(i) = 10*i
      end do
      
      do i=1, jdim + 1
        do j=1, jdim + 2
          p.jdata(i, j) = 1000*i + 100*j 
        end do
      end do

*     -- print segment
      segprt, p

*     -- delete segment
      segsup, p

*     -- finalize ESOPE system
      call ooostp

      write(*,*) 'main: end'
      end
      
