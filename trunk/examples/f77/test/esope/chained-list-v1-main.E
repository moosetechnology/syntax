      program main
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

      write(*,*) 'main: begin'

* >> For simplicity all SEGMENTS are kept "activated" : no segact/segdes      

*     -- initialize ESOPE system
      call ooossg
      
*     -- create a list of n nodes
      call newlst(head1, 4)
      
*     -- print the list "head1"
      call prtlst(head1)
                
*     -- copy the list "head1" to "head2"
      call coplst(head1, head2)
            
*     -- delete the list "head1"
      call dellst(head1) 
      
*     -- print the list "head2"
      call prtlst(head2)
            
*     -- delete the list "head2"
      call dellst(head2) 

*     -- finalize ESOPE system
      call ooostp

      write(*,*) 'main: end'
      end
      
