      program main
      implicit none
      
      segment, a23456789_123456789_123456789_1z
        real value
      endsegment
      
      segment, a23456789_123456789_123456789_1x
        integer count
      endsegment
      
      pointeur p.a23456789_123456789_123456789_1z
      pointeur q.a23456789_123456789_123456789_1x

      write(*,*) 'main: begin'

* >> For simplicity all SEGMENTS are kept "activated" : no segact/segdes      

*     -- initialize ESOPE system
      call ooossg
      
*     -- initialize segments
      segini, p
      p.value = 3.14

      segini, a23456789_123456789_123456789_1z
      value = 6.28
 
      segini, q
      q.count = 123
 
      segini, a23456789_123456789_123456789_1x
      count = 246
      
*     -- print segments
      segprt, p
      segprt, a23456789_123456789_123456789_1z
      segprt, q
      segprt, a23456789_123456789_123456789_1x
      
*     -- delete segments
      segsup, p
      p = 0
      segsup, q
      q = 0

*     -- finalize ESOPE system
      call ooostp

      write(*,*) 'main: end'
      end
      
