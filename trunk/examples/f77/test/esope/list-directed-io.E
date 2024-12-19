      program main
      implicit none
      
      integer i
      integer j
      integer k
      integer char_index
 
      integer nco
      parameter(nco=10)
      real acol(nco)
      character*(4) scol(nco)
      
      character*(26) char_list
      parameter(char_list='ABCDEFGHIJKLMNOPQRSTUVWXYZ')
    
      character*3 sep   
      parameter(sep='-->')
      
*     >> inline function of type 'integer -> character*0'
      character*0 nils
      
*     >> inline function of type 'integer -> integer'
      integer nili

      nils(k) = ''
      nili(k) = 0
     
      write(6,*) 
      write(6,*) 'main: begin'

*     -- initialize ESOPE system
*     >> even, if in that case, it is a pure Fortran-77 test case
      call ooossg
      
*     -- initialize arrays
      do i=1, nco
        acol(i) = sin(real(i))
      enddo

      do i=1, nco
        scol(i) = ' '
        do j=1, len(scol)
          char_index =  1 + (len(char_list) - 1)*abs(sin(real(i*j)))
          char_index = min(len(char_list), max(1, char_index))
          scol(i)(j:j) = char_list(char_index:char_index)
        enddo
      enddo
      
      write(6,*) 
      write(6,*) '-- print 1 --'
*     -- print 'acol'
      write(6,'(10(e12.5,a))') ((acol(k),' '), k=1,nco)
      
      write(6,*) 
      write(6,*) '-- print 2 --'
*     -- print 'acol' again with 'sep' as a separator
      write(6,'(10(e12.5,a))') ((acol(k),sep), k=1,nco)
      
      write(6,*) 
      write(6,*) '-- print 3 --'
*     -- other 'list-directed-I/O'
      write(6,'(10(a,a))') ((' ', scol(k)), k=1,10)

      write(6,*) 
      write(6,*) '-- print 4 --'
*     >> fails at compile-time: 
*     write(6,'(10(a,a))') (('x','y'), k=1,10)
*     >> workaround 
      write(6,'(10(a,a))') (('a', 'b' // nils(k)), k=1,10)

      write(6,*) 
      write(6,*) '-- print 5 --'
*     >> fails at run-time: 
*     write(6,'(10(i2,i2))') ((11, 22), k=1,10)
*     >> workaround 
      write(6,'(10(i2,i2))') ((11, 22 + nili(k)), k=1,10)
      
*     -- finalize ESOPE system
      call ooostp

      write(6,*) 
      write(6,*) 'main: end'
      end
      
