c  Source: https://github.com/johannesgerer/jburkardt-f77
c  File name: quad_serial/quad_serial.f
c
      program main

c*********************************************************************72
c
cc MAIN is the main program for QUAD_SERIAL.
c
c  Licensing:
c
c    This code is distributed under the GNU LGPL license.
c
c  Modified:
c
c    12 January 2007
c
c  Author:
c
c    John Burkardt
c
      double precision a
      double precision b
      double precision error
      double precision exact
      external f
      double precision f
      integer i
      integer n
      double precision total
      double precision wtime
      double precision wtime1
      double precision wtime2
      double precision x

      a =  0.0
      b = 10.0
      n = 10000000
      exact = 0.49936338107645674464D+00

      call timestamp ( )
      write ( *, * ) ' '
      write ( *, * ) 'QUAD:'
      write ( *, * ) '  FORTRAN77 version'
      write ( *, * ) '  Estimate the integral of f(x) from A to B.'
      write ( *, * ) '  f(x) = 50 / (pi * ( 2500 * x * x + 1 ) ).'
      write ( *, * ) ' '
      write ( *, * ) '  A        = ', a
      write ( *, * ) '  B        = ', b
      write ( *, * ) '  N        = ', n
      write ( *, * ) '  Exact    = ', exact

      call cpu_time ( wtime1 )

      total = 0.0D+00
      do i = 1, n
        x = ( ( n - i ) * a + ( i - 1 ) * b ) / ( n - 1 )
        total = total + f ( x )
      end do

      call cpu_time ( wtime2 )

      total = ( b - a ) * total / dble ( n )
      error = abs ( total - exact )
      wtime = wtime2 - wtime1

      write ( *, * ) ' '
      write ( *, * ) '  Estimate = ', total
      write ( *, * ) '  Error    = ', error
      write ( *, * ) '  Time     = ', wtime
c
c  Terminate.
c
      write ( *, * ) ' '
      write ( *, * ) 'QUAD_SERIAL:'
      write ( *, * ) '  Normal end of execution.'
      write ( *, * ) ' '
      call timestamp ( )

      stop
      end
      function f ( x )

c*********************************************************************72
c
cc F evaluates the function.
c
      double precision f
      double precision pi
      double precision x

      pi = 3.141592653589793D+00
      f = 50.0D+00 / ( pi * ( 2500.0D+00 * x * x + 1.0D+00 ) )

      return
      end
      subroutine timestamp ( )

c*********************************************************************72
c
cc TIMESTAMP prints out the current YMDHMS date as a timestamp.
c
c  Licensing:
c
c    This code is distributed under the GNU LGPL license.
c
c  Modified:
c
c    12 January 2007
c
c  Author:
c
c    John Burkardt
c
c  Parameters:
c
c    None
c
      implicit none

      character * ( 8 ) ampm
      integer d
      character * ( 8 ) date
      integer h
      integer m
      integer mm
      character * ( 9 ) month(12)
      integer n
      integer s
      character * ( 10 ) time
      integer y

      save month

      data month /
     &  'January  ', 'February ', 'March    ', 'April    ', 
     &  'May      ', 'June     ', 'July     ', 'August   ', 
     &  'September', 'October  ', 'November ', 'December ' /

      call date_and_time ( date, time )

      read ( date, '(i4,i2,i2)' ) y, m, d
      read ( time, '(i2,i2,i2,1x,i3)' ) h, n, s, mm

      if ( h .lt. 12 ) then
        ampm = 'AM'
      else if ( h .eq. 12 ) then
        if ( n .eq. 0 .and. s .eq. 0 ) then
          ampm = 'Noon'
        else
          ampm = 'PM'
        end if
      else
        h = h - 12
        if ( h .lt. 12 ) then
          ampm = 'PM'
        else if ( h .eq. 12 ) then
          if ( n .eq. 0 .and. s .eq. 0 ) then
            ampm = 'Midnight'
          else
            ampm = 'AM'
          end if
        end if
      end if

      write ( *, 
     &  '(i2,1x,a,1x,i4,2x,i2,a1,i2.2,a1,i2.2,a1,i3.3,1x,a)' ) 
     &  d, month(m), y, h, ':', n, ':', s, '.', mm, ampm

      return
      end
