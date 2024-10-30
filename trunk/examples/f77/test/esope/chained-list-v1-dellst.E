      subroutine dellst(head)
*     -- delete a list of nodes
      implicit none
#include "node.seg"
c begin "node.seg"

      segment, node
        real value
        pointeur next.node
      endsegment

c end "node.seg"
      pointeur head.node
      
      pointeur some.node

      write(*,*) '------------------'
      write(*,*) 'dellst: begin'
      
10    if (head .ne. 0) then
        some = head
        head = some.next
        segsup, some
        goto 10
      endif
      
      write(*,*) 'dellst: end'
      end
