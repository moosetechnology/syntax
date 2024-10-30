      program main
      implicit none
      
      external evaluate_value_count
      integer evaluate_value_count
      
      integer param1
      integer param2
      
      segment, node
        real values(evaluate_value_count(param1, param2))
      endsegment
      
      integer value_index
      integer value_count
      integer expected_value_count
      pointeur p.node

      write(*,*) 'main: begin'

* >> For simplicity all SEGMENTS are kept "activated" : no segact/segdes      

*     -- initialize ESOPE system
      call ooossg
      
*     -- initialize segment
      param1 = 2
      param2 = 5
      
      segini, p
     
      value_count = p.values(/1)
      expected_value_count = evaluate_value_count(param1, param2)

      write(*,*) 'main: param1=', param1
      write(*,*) 'main: param2=', param2
      write(*,*) 'main: value_count=', value_count
      write(*,*) 'main: expected_value_count=', expected_value_count
      
      do value_index=1, value_count
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
      
