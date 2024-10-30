      program main
      implicit none
      
      segment, node
      endsegment
     
      pointeur empty.node

      write(*,*) 'main: begin'

* >> For simplicity all SEGMENTS are kept "activated" : no segact/segdes      

*     -- initialize ESOPE system
      call ooossg
      
*     -- initialize segments
      segini, empty
      
*     -- print segments
      segprt, empty
      
*     -- delete segments
      segsup, empty
      empty = 0


*     -- finalize ESOPE system
      call ooostp

      write(*,*) 'main: end'
      end
      
