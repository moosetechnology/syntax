      program main
      implicit none
      
      segment, node
        real value
      endsegment
      
      pointeur q.node
      pointeur a23456789_123456789_123456789_1x.node
      pointeur a23456789_123456789_123456789_1y.node

      write(*,*) 'main: begin'

* >> For simplicity all SEGMENTS are kept "activated" : no segact/segdes      

*     -- initialize ESOPE system
      call ooossg
      
*     -- initialize segments
      segini, a23456789_123456789_123456789_1x
      q = a23456789_123456789_123456789_1x
      q.value = 3.14
      q = 0
      
      segini, a23456789_123456789_123456789_1y
      q = a23456789_123456789_123456789_1y
      q.value = 6.28
      q = 0
      
*     -- print segment
      segprt, a23456789_123456789_123456789_1x
      segprt, a23456789_123456789_123456789_1y
      
*     -- delete segment     
      segsup, a23456789_123456789_123456789_1x
      a23456789_123456789_123456789_1x = 0
      
      segsup, a23456789_123456789_123456789_1y
      a23456789_123456789_123456789_1y = 0

*     -- finalize ESOPE system
      call ooostp

      write(*,*) 'main: end'
      end
      
