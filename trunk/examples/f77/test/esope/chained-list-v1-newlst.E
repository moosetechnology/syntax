      subroutine newlst(head, n)
*     -- create a list of "n" nodes
      implicit none
#include "node.seg"
c begin "node.seg"

      segment, node
        real value
        pointeur next.node
      endsegment

c end "node.seg"
      pointeur head.node
      integer n
      
      integer i
      real newval

      write(*,*) '------------------'
      write(*,*) 'newlst: begin'
      
      head = 0
      do i = 1, n
        newval = 500. + real(i)
        call addlst(head, newval)
      enddo
      
      write(*,*) 'newlst: end'
      end
