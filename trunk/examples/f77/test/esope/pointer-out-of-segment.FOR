       subroutine borbk(lib,name,title)
       implicit none

c #include "struc.inc"
c #include "PSTR.inc"

c dummy arguments
       pointeur lib.PSTR
       character *(*) name
       character *(*) title

c use of external for functions
       external mypnt
       integer mypnt
       external findbk 
       integer findbk
       external findur
       integer findur

c local variables
       pointeur lb.tlib
       pointeur ur.user
       integer ibk,iur
       
       write(*,*) 'borbk: begin'

c activate structure get the first segment and sizes of arrays
       call actstr(lib)
       lb = mypnt(lib,1)
       segact, lb
       brcnt = lb.bref(/1)
       urcnt = lb.uref(/1) 

c see whether the user and the book exist in the structure       
       iur = findur(lib,name)
       if (iur .EQ. 0) then 
         write(*,*) " user",name," does not exist"
         return
       end if 
       ibk = findbk(lib,title)
       if (ibk .EQ. 0) then 
         write(*,*) " book",title," does not exist"
         return
       end if 

c set flag true if the book is borrowed    
       lb.bstatu(brcnt) = .true.

c add the book borrowed to the array  
       ur = mypnt(lib,iur)    
       segact, ur
       ubbcnt = ur.ubb(/1)
       ubbcnt = ubbcnt + 1
       segadj, ur
       ur.ubb(ubbcnt) = ibk       
       segdes, ur*MOD

c desactivate head segment  
       segdes,lb*MOD

c desactivate structure
       call desstr(lib, "MOD")
       write(*,*) 'borbk: end'
                    
       end 
