      program main

*  >> 'implicit none' has been removed on purpose         

* -- Condensed declarations seen in "Verpeaux_1993_Esope-Gemat-v10.0.pdf"     
      segment abc(2)
      segment xyz(3)*I
      segment ijk(5)*R
     
      write(*,*) 'main: begin'

* >> For simplicity all SEGMENTS are kept "activated" : no segact/segdes      

*     -- initialize ESOPE system
      call ooossg
      
*     -- initialize segments
      segini,abc
      abc(1)=1.1
      abc(2)=1.2

      segini,xyz
      xyz(1)=101
      xyz(2)=102
      xyz(3)=103
      
      segini,ijk
      ijk(1)=200.1
      ijk(2)=200.2
      ijk(3)=200.3
      ijk(4)=200.4
      ijk(5)=200.5
          
*     -- print segment
      segprt,abc
      segprt,xyz
      segprt,ijk
          
*     -- delete segments
      segsup,abc
      abc = 0
      
      segsup,xyz
      xyz = 0
      
      segsup,ijk
      ijk = 0

*     -- finalize ESOPE system
      call ooostp

      write(*,*) 'main: end'
      end
      
