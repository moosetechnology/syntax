      program main
      implicit none
      
      integer d23456789_123456789_123456789_12
      integer value_index
      
      segment, node
        real values(d23456789_123456789_123456789_12)
      endsegment
      
      pointeur p.node

      write(*,*) 'main: begin'

* >> For simplicity all SEGMENTS are kept "activated" : no segact/segdes      

*     -- initialize ESOPE system
      call ooossg
      
*     -- initialize segment
      d23456789_123456789_123456789_12 = 10
      segini, p
      do value_index=1, d23456789_123456789_123456789_12
        p.values(value_index) = 100. + value_index
      enddo
     
*     -- print segment
      segprt, p
      
*     -- delete segment
      segsup, p
      p = 0

*     -- finalize ESOPE system
      call ooostp

      write(*,*) 'main: end'
      end
      
