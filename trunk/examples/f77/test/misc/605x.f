c  Source: https://github.com/johannesgerer/jburkardt-f77
c  File name: mxm_openmp/mxm_openmp.f
c
      program main

c*********************************************************************72
c
cc MAIN is the main program for MXM_OPENMP.
c
c  Licensing:
c
c    This code is distributed under the GNU LGPL license. 
c
c  Modified:
c
c    13 October 2011
c
c  Author:
c
c    John Burkardt
c
      implicit none

#include "omp_lib.h"

      integer n
      parameter ( n = 500 )

      double precision a(n,n)
      double precision angle
      double precision b(n,n)
      double precision c(n,n)
      integer i
      integer j
      integer k
      double precision pi
      parameter ( pi = 3.141592653589793D+00 )
      double precision s
      integer thread_num
      double precision wtime

      call timestamp ( )

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'MXM_OPENMP:'
      write ( *, '(a)' ) '  FORTRAN77/OpenMP version'
      write ( *, '(a)' ) '  Compute matrix product C = A * B.'

      thread_num = omp_get_max_threads ( )

      write ( *, '(a)' ) ' '
      write ( *, '(a,i8)' ) 
     &  '  The number of processors available = ', omp_get_num_procs ( )
      write ( *, '(a,i8)' ) 
     &  '  The number of threads available    = ', thread_num

      write ( *, '(a,i8)' ) '  The matrix order N                 = ', n
c
c  Loop 1: Evaluate A.
c
      s = 1.0D+00 / sqrt ( dble ( n ) )

      wtime = omp_get_wtime ( )

c$omp parallel shared ( a, b, c, s ) private ( angle, i, j, k )

c$omp do
      do i = 1, n
        do j = 1, n
          angle = 2.0D+00 * pi * ( i - 1 ) * ( j - 1 ) / dble ( n )
          a(i,j) = s * ( sin ( angle ) + cos ( angle ) ) 
        end do
      end do
c$omp end do
c
c  Loop 2: Copy A into B.
c
c$omp do
      do i = 1, n
        do j = 1, n
          b(i,j) = a(i,j)
        end do
      end do
c$omp end do
c
c  Loop 3: Compute C = A * B.
c
c$omp do
      do i = 1, n
        do j = 1, n
          c(i,j) = 0.0D+00
          do k = 1, n
            c(i,j) = c(i,j) + a(i,k) * b(k,j)
          end do
        end do
      end do
c$omp end do

c$omp end parallel

      wtime = omp_get_wtime ( ) - wtime
      write ( *, '(a,g14.6)' ) '  Elapsed seconds = ', wtime
      write ( *, '(a,g14.6)' ) '  C(100,100)  = ', c(100,100)
c
c  Terminate.
c
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'MXM_OPENMP:'
      write ( *, '(a)' ) '  Normal end of execution.'
      write ( *, '(a)' ) ' '
      call timestamp ( )

      stop
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
