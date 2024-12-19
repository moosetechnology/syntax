      program main
      implicit none
      
      segment, node
        real value
      endsegment
      
      pointeur p.node

      write(*,*) 'main: begin'

* >> For simplicity all SEGMENTS are kept "activated" : no segact/segdes      

*     -- initialize ESOPE system
      call ooossg
      
*     -- initialize segments
* >>  a declaration of pointer here would be illegal

      segini, p
      p.value = 3.14
            
*     -- print segment
      segprt, p
      
*     -- delete segment     
      segsup, p
      p = 0
      
*     -- int and print a variable
      n = 12*12      
      write(*,*) 'n = ', n

*     -- finalize ESOPE system
      call ooostp

      write(*,*) 'main: end'
      end
      
