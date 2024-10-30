      program main
      implicit none
      
      segment, node
        real value
      endsegment
      
      pointeur p.node
      
      real    v23456789_123456789_123456789_1z
      integer v23456789_123456789_123456789_1x

      write(*,*) 'main: begin'

* >> For simplicity all SEGMENTS are kept "activated" : no segact/segdes      

*     -- initialize ESOPE system
      call ooossg
      
*     -- initialize variables
      v23456789_123456789_123456789_1z = 3.14
      write(*,*) 'v23456789_123456789_123456789_1z = ', 
     &            v23456789_123456789_123456789_1z

      v23456789_123456789_123456789_1x = 123
      write(*,*) 'v23456789_123456789_123456789_1x = ', 
     &            v23456789_123456789_123456789_1x
      
*     -- initialize segment
      segini, p
      p.value = v23456789_123456789_123456789_1z
      
*     -- print segment
      segprt, p
      
*     -- delete segment
      segsup, p
      p = 0

*     -- finalize ESOPE system
      call ooostp

      write(*,*) 'main: end'
      end
      
