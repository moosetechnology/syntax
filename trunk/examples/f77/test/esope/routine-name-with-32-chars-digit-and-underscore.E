      program main
      implicit none
#include "node.seg"     
      
      pointeur abc4_f.node

      write(*,*) 'main: begin'

* >> For simplicity all SEGMENTS are kept "activated" : no segact/segdes      

*     -- initialize ESOPE system
      call ooossg
      
*     -- initialize segment
      segini, abc4_f
      abc4_f.value = 789.123
      abc4_f.next = 0

*     -- print segment
      call x23456789_123456789_123456789_1z(abc4_f)
         
*     -- delete segment
      segsup, abc4_f
      abc4_f = 0

*     -- finalize ESOPE system
      call ooostp

      write(*,*) 'main: end'
      end
      
