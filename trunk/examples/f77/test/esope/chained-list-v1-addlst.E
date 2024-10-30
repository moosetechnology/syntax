      subroutine addlst(head, newval)
*     -- add "value" to the list
      implicit none
#include "node.seg"
c begin "node.seg"

      segment, node
        real value
        pointeur next.node
      endsegment

c end "node.seg"
      pointeur head.node
      real newval
      
      pointeur some.node

      write(*,*) '------------------'
      write(*,*) 'addlst: begin'

      segini, some
      some.value = newval
      some.next = head
      head = some
      
      write(*,*) 'addlst: end'
      end

