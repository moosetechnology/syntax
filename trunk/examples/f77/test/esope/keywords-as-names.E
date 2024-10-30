      program main
      implicit none
      
      segment, IMPLIED
        real SEGMENT
      endsegment
      
      segment, POINTEUR
        integer OPEN
      endsegment
      
      pointeur WRITE.IMPLIED
      pointeur READ.POINTEUR

      write(*,*) 'main: begin'

* >> For simplicity all SEGMENTS are kept "activated" : no segact/segdes      

*     -- initialize ESOPE system
      call ooossg
      
*     -- initialize segments
      segini, WRITE
      WRITE.SEGMENT = 3.14

      segini, IMPLIED
      SEGMENT = 6.28
 
      segini, READ
      READ.OPEN = 123
 
      segini, POINTEUR
      OPEN = 246
      
*     -- print segments
      segprt, WRITE
      segprt, IMPLIED
      segprt, READ
      segprt, POINTEUR
      
*     -- delete segments
      segsup, WRITE
      WRITE = 0
      segsup, READ
      READ = 0

*     -- finalize ESOPE system
      call ooostp

      write(*,*) 'main: end'
      end
      
