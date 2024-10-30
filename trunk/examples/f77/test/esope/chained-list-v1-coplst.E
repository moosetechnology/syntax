      subroutine coplst(head1, head2)
*     -- copy the list "head1" to "head2"
      implicit none
#include "node.seg"
c begin "node.seg"

      segment, node
        real value
        pointeur next.node
      endsegment

c end "node.seg"
      pointeur head1.node
      pointeur head2.node
      
      pointeur tail1.node
      pointeur tail2.node
      
      pointeur some1.node
      pointeur some2.node

      write(*,*) '------------------'
      write(*,*) 'coplst: begin'

      head2 = 0
      tail2 = head2
      
      tail1 = head1
      
10    if (tail1 .ne. 0) then
        segini, some2
        some2.value = tail1.value
        some2.next = 0

        if (tail2 .eq. 0) then
            tail2 = some2
            head2 = some2
        else
            tail2.next = some2
            tail2 = some2
        endif
        
        tail1 = tail1.next
        goto 10
      endif
      
      write(*,*) 'coplst: end'
      end
