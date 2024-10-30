      program main
      implicit none
      
      segment, t1
        real a1
      endsegment
      
      segment, t2
        pointeur a2.t1
      endsegment
      
      segment, t3
        pointeur a3.t2
      endsegment
       
      segment, t4
        pointeur a4.t3
      endsegment
       
      segment, t5
        pointeur a5.t4
      endsegment

      segment, t6
        pointeur a6.t5
      endsegment

      segment, t7
        pointeur a7.t6
      endsegment

      segment, t8
        pointeur a8.t7
      endsegment

      segment, t9
        pointeur a9.t8
      endsegment
     
      pointeur v1.t1
      pointeur v2.t2
      pointeur v3.t3
      pointeur v4.t4
      pointeur v5.t5
      pointeur v6.t6
      pointeur v7.t7
      pointeur v8.t8
      pointeur v9.t9

      write(*,*) 'main: begin'

* >> For simplicity all SEGMENTS are kept "activated" : no segact/segdes      

*     -- initialize ESOPE system
      call ooossg
      
*     -- initialize segments
      segini, v1
      v1.a1 = 1.234

      segini, v2
      v2.a2 = v1
      
      segini, v3
      v3.a3 = v2
      
      segini, v4
      v4.a4 = v3
      
      segini, v5
      v5.a5 = v4
      
      segini, v6
      v6.a6 = v5
      
      segini, v7
      v7.a7 = v6
      
      segini, v8
      v8.a8 = v7
      
      segini, v9
      v9.a9 = v8
            
*     -- print segment
      segprt, v1
      segprt, v2
      segprt, v3
      segprt, v4
      segprt, v5
      segprt, v6
      segprt, v7
      segprt, v8
      segprt, v9
 
*     -- change the inner most segment
      write(*,*) 'before change v9.a9.a8.a7.a6.a5.a4.a3.a2.a1 = ', 
     &v9.a9.a8.a7.a6.a5.a4.a3.a2.a1
      
      v9.a9.a8.a7.a6.a5.a4.a3.a2.a1 = 10*v9.a9.a8.a7.a6.a5.a4.a3.a2.a1
      
      write(*,*) 'after change v9.a9.a8.a7.a6.a5.a4.a3.a2.a1 = ', 
     &v9.a9.a8.a7.a6.a5.a4.a3.a2.a1
      
*     -- delete segment     
      segsup, v1
      segsup, v2
      segsup, v3
      segsup, v4
      segsup, v5
      segsup, v6
      segsup, v7
      segsup, v8
      segsup, v9
      v1 = 0
      v2 = 0
      v3 = 0
      v4 = 0
      v5 = 0
      v6 = 0
      v7 = 0
      v8 = 0
      v9 = 0

*     -- finalize ESOPE system
      call ooostp

      write(*,*) 'main: end'
      end
      
