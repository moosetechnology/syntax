      subroutine prtlst(head)
*     -- print a list of nodes
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
      write(*,*) 'prtlst: begin'
      
      some = head
10    if (some .ne. 0) then
        write(*,*)
        write(*,'(a, i6.6)') 'node = ', some
        write(*,'(a, f4.0)') '    value = ', some.value
        write(*,'(a, i6.6)') '     next = ', some.next
        some = some.next
        goto 10
      endif
      
      write(*,*) 'prtlst: end'
      end
