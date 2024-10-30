      program main
      implicit none
      
*     >> insert space inside the keyword "segment"
      se gme nt, node
        real value
        pointeur next.node
      endsegment
      
*     >> insert space inside the keyword "pointeur"
*     >> remove space between the keyword "pointeur" and "abcd.node"
      po inteurabcd.node

      write(*,*) 'main: begin'

* >> For simplicity all SEGMENTS are kept "activated" : no segact/segdes      

*     -- initialize ESOPE system
      call ooossg
      
*     -- initialize segment
      segini, abcd
      abcd.value = 789.123
      abcd.next = 0
      
*     -- print segment
      segprt, abcd
      
*     -- delete segment
      segsup, abcd
      abcd = 0

*     -- finalize ESOPE system
      call ooostp

      write(*,*) 'main: end'
      end
      
