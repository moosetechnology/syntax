c  Source: https://github.com/johannesgerer/jburkardt-f77
c  File name: f77_class/hello2.f
c
      program hello2

c*********************************************************************72

      integer i
      integer imax

      i=0
      imax=12

10    continue

      i=i+1

      write(*,*)'Hello, person number ',i
      if (i.lt.imax) go to 10

      write(*,*)'Goodbye!'

      stop
      end
