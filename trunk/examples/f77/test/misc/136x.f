c  Source: https://github.com/johannesgerer/jburkardt-f77
c  File name: asa111/asa111_prb.f
c
      program main

c*********************************************************************72
c
cc MAIN is the main program for ASA111_PRB.
c
c  Modified:
c
c    21 January 2008
c
c  Author:
c
c    John Burkardt
c
      implicit none

      call timestamp ( )

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'ASA111_PRB:'
      write ( *, '(a)' ) '  FORTRAN77 version'
      write ( *, '(a)' ) '  Test the ASA111 library.'

      call test01 ( )
c
c  Terminate.
c
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'ASA111_PRB:'
      write ( *, '(a)' ) '  Normal end of execution.'

      write ( *, '(a)' ) ' '
      call timestamp ( )

      stop
      end
      subroutine test01 ( )

c*********************************************************************72
c
cc TEST01 compares PPND against tabulated values.
c
c  Modified:
c
c    21 January 2008
c
c  Author:
c
c    John Burkardt
c
      implicit none

      double precision fx
      integer ifault
      integer n_data
      double precision ppnd
      double precision x
      double precision x2

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST01:'
      write ( *, '(a)' ) '  Compare PPND against tabulated values.'
      write ( *, '(a)' ) ' '
      write ( *, '(a,a)' )
     &  '      CDF              X                         X2    ',
     &  '                 DIFF'
      write ( *, '(a)' )
     &  '                      (tabulated)               (PPND)'
      write ( *, '(a)' ) ' '

      n_data = 0

10    continue

        call normal_01_cdf_values ( n_data, x, fx )

        if ( n_data .eq. 0 ) then
          go to 20
        end if

        x2 = ppnd ( fx, ifault )

        write ( *, '(2x,g14.6,2x,g24.16,2x,g24.16,2x,g10.4)' )
     &  fx, x, x2, dabs ( x - x2 )

      go to 10

20    continue

      return
      end
