c  Source: https://github.com/johannesgerer/jburkardt-f77
c  File name: asa183/asa183_prb.f
c
      program main

c*********************************************************************72
c
cc MAIN is the main program for ASA183_PRB.
c
c  Discussion:
c
c    ASA183_PRB calls sample problems for the ASA183 library.
c
c  Modified:
c
c    08 July 2008
c
c  Author:
c
c    John Burkardt
c
      implicit none

      call timestamp ( )

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'ASA183_PRB'
      write ( *, '(a)' ) '  FORTRAN77 version'
      write ( *, '(a)' ) '  Test the routines in the ASA183 library.'

      call test01
      call test02
      call test03

      call test04
      call test05
      call test06

      call test07
      call test08
      call test09

      call test10
      call test11
      call test12

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'ASA183_PRB'
      write ( *, '(a)' ) '  Normal end of execution.'

      write ( *, '(a)' ) ' '
      call timestamp ( )

      stop
      end
      subroutine test01

c*********************************************************************72
c
cc TEST01 tests R4_RANDOM.
c
c  Modified:
c
c    08 July 2008
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i
      real r
      real r4_random
      integer s1
      integer s2
      integer s3

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST01'
      write ( *, '(a)' ) '  R4_RANDOM computes pseudorandom values.'
      write ( *, '(a)' ) '  Three seeds, S1, S2, and S3, are used.'

      s1 = 12345
      s2 = 34567
      s3 = 56789

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '      R                 S1        S2        S3'
      write ( *, '(a)' ) ' '
      write ( *, '(2x,14x,2x,i8,2x,i8,2x,i8)' ) s1, s2, s3

      do i = 1, 10
        r = r4_random ( s1, s2, s3 )
        write ( *, '(2x,g14.6,2x,i8,2x,i8,2x,i8)' ) r, s1, s2, s3
      end do

      return
      end
      subroutine test02

c*********************************************************************72
c
cc TEST02 tests R4_RANDOM.
c
c  Modified:
c
c    08 July 2008
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer n
      parameter ( n = 100000 )

      integer i
      real r4_random
      integer s1
      integer s2
      integer s3
      real u(n)
      real u_avg
      real u_var

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST02'
      write ( *, '(a)' ) '  Examine the average and variance of a'
      write ( *, '(a)' ) '  sequence generated by R4_RANDOM.'
c
c  Start with known seeds.
c
      s1 = 12345
      s2 = 34567
      s3 = 56789

      write ( *, '(a)' ) ' '
      write ( *, '(a,i10,a)' ) '  Now compute ', n, ' elements.'

      u_avg = 0.0E+00
      do i = 1, n
        u(i) = r4_random ( s1, s2, s3 )
        u_avg = u_avg + u(i)
      end do

      u_avg = u_avg / real ( n )

      u_var = 0.0E+00
      do i = 1, n
        u_var = u_var + ( u(i) - u_avg ) * ( u(i) - u_avg )
      end do

      u_var = u_var / real ( n - 1 )

      write ( *, '(a)' ) ' '
      write ( *, '(a,f10.6)' ) '  Average value = ', u_avg
      write ( *, '(a,f10.6)' ) '  Expecting       ', 0.5E+00

      write ( *, '(a)' ) ' '
      write ( *, '(a,f10.6)' ) '  Variance =      ', u_var
      write ( *, '(a,f10.6)' ) '  Expecting       ', 1.0E+00 / 12.0E+00

      return
      end
      subroutine test03

c*********************************************************************72
c
cc TEST03 tests R4_RANDOM.
c
c  Modified:
c
c    08 July 2008
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i
      real r
      real r4_random
      integer s1
      integer s1_save
      integer s2
      integer s2_save
      integer s3
      integer s3_save

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST03'
      write ( *, '(a)' ) '  Show how the seeds used by R4_RANDOM,'
      write ( *, '(a)' ) '  which change on each step, can be reset to'
      write ( *, '(a)' ) '  restore any part of the sequence.'

      s1_save = 12345
      s2_save = 34567
      s3_save = 56789

      s1 = s1_save
      s2 = s2_save
      s3 = s3_save

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Begin sequence with following seeds:'
      write ( *, '(a)' ) ' '
      write ( *, '(a,i8)' ) '  S1 = ', s1
      write ( *, '(a,i8)' ) '  S2 = ', s2
      write ( *, '(a,i8)' ) '  S3 = ', s3
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '         I      R                 S1        S2        S3'
      write ( *, '(a)' ) ' '

      do i = 1, 10

        r = r4_random ( s1, s2, s3 )
        write ( *, '(2x,i8,2x,g14.6,2x,i8,2x,i8,2x,i8)' ) 
     &    i, r, s1, s2, s3

        if ( i == 5 ) then
          s1_save = s1
          s2_save = s2
          s3_save = s3
        end if

      end do

      s1 = s1_save
      s2 = s2_save
      s3 = s3_save

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Restart the sequence, using the seeds'
      write ( *, '(a)' ) '  produced after step 5:'
      write ( *, '(a)' ) ' '
      write ( *, '(a,i8)' ) '  S1 = ', s1
      write ( *, '(a,i8)' ) '  S2 = ', s2
      write ( *, '(a,i8)' ) '  S3 = ', s3
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '         I      R                 S1        S2        S3'
      write ( *, '(a)' ) ' '

      do i = 1, 10

        r = r4_random ( s1, s2, s3 )
        write ( *, '(2x,i8,2x,g14.6,2x,i8,2x,i8,2x,i8)' ) 
     &    i, r, s1, s2, s3

      end do

      return
      end
      subroutine test04

c*********************************************************************72
c
cc TEST04 tests R4_UNI.
c
c  Modified:
c
c    08 July 2008
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i
      real r
      real r4_uni
      integer s1
      integer s2

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST04'
      write ( *, '(a)' ) '  R4_UNI computes pseudorandom values.'
      write ( *, '(a)' ) '  Two seeds, S1 and S2, are used.'

      s1 = 12345
      s2 = 34567

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '      R                     S1            S2'
      write ( *, '(a)' ) ' '
      write ( *, '(2x,14x,2x,i12,2x,i12)' ) s1, s2

      do i = 1, 10
        r = r4_uni ( s1, s2 )
        write ( *, '(2x,g14.6,2x,i12,2x,i12)' ) r, s1, s2
      end do

      return
      end
      subroutine test05

c*********************************************************************72
c
cc TEST05 tests R4_UNI.
c
c  Modified:
c
c    08 July 2008
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer n
      parameter ( n = 100000 )

      integer i
      real r4_uni
      integer s1
      integer s2
      real u(n)
      real u_avg
      real u_var

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST05'
      write ( *, '(a)' ) '  Examine the average and variance of a'
      write ( *, '(a)' ) '  sequence generated by R4_UNI.'
c
c  Start with known seeds.
c
      s1 = 12345
      s2 = 34567

      write ( *, '(a)' ) ' '
      write ( *, '(a,i10,a)' ) '  Now compute ', n, ' elements.'

      u_avg = 0.0E+00
      do i = 1, n
        u(i) = r4_uni ( s1, s2 )
        u_avg = u_avg + u(i)
      end do

      u_avg = u_avg / real ( n )

      u_var = 0.0E+00
      do i = 1, n
        u_var = u_var + ( u(i) - u_avg ) * ( u(i) - u_avg )
      end do

      u_var = u_var / real ( n - 1 )

      write ( *, '(a)' ) ' '
      write ( *, '(a,f10.6)' ) '  Average value = ', u_avg
      write ( *, '(a,f10.6)' ) '  Expecting       ', 0.5E+00

      write ( *, '(a)' ) ' '
      write ( *, '(a,f10.6)' ) '  Variance =      ', u_var
      write ( *, '(a,f10.6)' ) '  Expecting       ', 1.0E+00 / 12.0E+00

      return
      end
      subroutine test06

c*********************************************************************72
c
cc TEST06 tests R4_UNI.
c
c  Modified:
c
c    08 July 2008
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i
      real r
      real r4_uni
      integer s1
      integer s1_save
      integer s2
      integer s2_save

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST06'
      write ( *, '(a)' ) '  Show how the seeds used by R4_UNI,'
      write ( *, '(a)' ) '  which change on each step, can be reset to'
      write ( *, '(a)' ) '  restore any part of the sequence.'

      s1_save = 12345
      s2_save = 34567

      s1 = s1_save
      s2 = s2_save

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Begin sequence with following seeds:'
      write ( *, '(a)' ) ' '
      write ( *, '(a,i8)' ) '  S1 = ', s1
      write ( *, '(a,i8)' ) '  S2 = ', s2
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '         I      R                     S1            S2'
      write ( *, '(a)' ) ' '

      do i = 1, 10

        r = r4_uni ( s1, s2 )
        write ( *, '(2x,i8,2x,g14.6,2x,i12,2x,i12)' ) 
     &    i, r, s1, s2

        if ( i == 5 ) then
          s1_save = s1
          s2_save = s2
        end if

      end do

      s1 = s1_save
      s2 = s2_save

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Restart the sequence, using the seeds'
      write ( *, '(a)' ) '  produced after step 5:'
      write ( *, '(a)' ) ' '
      write ( *, '(a,i12)' ) '  S1 = ', s1
      write ( *, '(a,i12)' ) '  S2 = ', s2
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '         I      R                     S1            S2'
      write ( *, '(a)' ) ' '

      do i = 1, 10

        r = r4_uni ( s1, s2 )
        write ( *, '(2x,i8,2x,g14.6,2x,i12,2x,i12)' ) 
     &    i, r, s1, s2

      end do

      return
      end
      subroutine test07

c*********************************************************************72
c
cc TEST07 tests R8_RANDOM.
c
c  Modified:
c
c    08 July 2008
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i
      double precision r
      double precision r8_random
      integer s1
      integer s2
      integer s3

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST07'
      write ( *, '(a)' ) '  R8_RANDOM computes pseudorandom values.'
      write ( *, '(a)' ) '  Three seeds, S1, S2, and S3, are used.'

      s1 = 12345
      s2 = 34567
      s3 = 56789

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '      R                 S1        S2        S3'
      write ( *, '(a)' ) ' '
      write ( *, '(2x,14x,2x,i8,2x,i8,2x,i8)' ) s1, s2, s3

      do i = 1, 10
        r = r8_random ( s1, s2, s3 )
        write ( *, '(2x,g14.6,2x,i8,2x,i8,2x,i8)' ) r, s1, s2, s3
      end do

      return
      end
      subroutine test08

c*********************************************************************72
c
cc TEST08 tests R8_RANDOM.
c
c  Modified:
c
c    08 July 2008
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer n
      parameter ( n = 100000 )

      integer i
      double precision r8_random
      integer s1
      integer s2
      integer s3
      double precision u(n)
      double precision u_avg
      double precision u_var

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST08'
      write ( *, '(a)' ) '  Examine the average and variance of a'
      write ( *, '(a)' ) '  sequence generated by R8_RANDOM.'
c
c  Start with known seeds.
c
      s1 = 12345
      s2 = 34567
      s3 = 56789

      write ( *, '(a)' ) ' '
      write ( *, '(a,i10,a)' ) '  Now compute ', n, ' elements.'

      u_avg = 0.0D+00
      do i = 1, n
        u(i) = r8_random ( s1, s2, s3 )
        u_avg = u_avg + u(i)
      end do

      u_avg = u_avg / dble ( n )

      u_var = 0.0D+00
      do i = 1, n
        u_var = u_var + ( u(i) - u_avg ) * ( u(i) - u_avg )
      end do

      u_var = u_var / dble ( n - 1 )

      write ( *, '(a)' ) ' '
      write ( *, '(a,f10.6)' ) '  Average value = ', u_avg
      write ( *, '(a,f10.6)' ) '  Expecting       ', 0.5D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a,f10.6)' ) '  Variance =      ', u_var
      write ( *, '(a,f10.6)' ) '  Expecting       ', 1.0D+00 / 12.0D+00

      return
      end
      subroutine test09

c*********************************************************************72
c
cc TEST09 tests R8_RANDOM.
c
c  Modified:
c
c    08 July 2008
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i
      double precision r
      double precision r8_random
      integer s1
      integer s1_save
      integer s2
      integer s2_save
      integer s3
      integer s3_save

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST09'
      write ( *, '(a)' ) '  Show how the seeds used by R8_RANDOM,'
      write ( *, '(a)' ) '  which change on each step, can be reset to'
      write ( *, '(a)' ) '  restore any part of the sequence.'

      s1_save = 12345
      s2_save = 34567
      s3_save = 56789

      s1 = s1_save
      s2 = s2_save
      s3 = s3_save

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Begin sequence with following seeds:'
      write ( *, '(a)' ) ' '
      write ( *, '(a,i8)' ) '  S1 = ', s1
      write ( *, '(a,i8)' ) '  S2 = ', s2
      write ( *, '(a,i8)' ) '  S3 = ', s3
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '         I      R                 S1        S2        S3'
      write ( *, '(a)' ) ' '

      do i = 1, 10

        r = r8_random ( s1, s2, s3 )
        write ( *, '(2x,i8,2x,g14.6,2x,i8,2x,i8,2x,i8)' ) 
     &    i, r, s1, s2, s3

        if ( i == 5 ) then
          s1_save = s1
          s2_save = s2
          s3_save = s3
        end if

      end do

      s1 = s1_save
      s2 = s2_save
      s3 = s3_save

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Restart the sequence, using the seeds'
      write ( *, '(a)' ) '  produced after step 5:'
      write ( *, '(a)' ) ' '
      write ( *, '(a,i8)' ) '  S1 = ', s1
      write ( *, '(a,i8)' ) '  S2 = ', s2
      write ( *, '(a,i8)' ) '  S3 = ', s3
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '         I      R                 S1        S2        S3'
      write ( *, '(a)' ) ' '

      do i = 1, 10

        r = r8_random ( s1, s2, s3 )
        write ( *, '(2x,i8,2x,g14.6,2x,i8,2x,i8,2x,i8)' ) 
     &    i, r, s1, s2, s3

      end do

      return
      end
      subroutine test10

c*********************************************************************72
c
cc TEST10 tests R8_UNI.
c
c  Modified:
c
c    08 July 2008
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i
      double precision r
      double precision r8_uni
      integer s1
      integer s2

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST10'
      write ( *, '(a)' ) '  R8_UNI computes pseudorandom values.'
      write ( *, '(a)' ) '  Two seeds, S1 and S2, are used.'

      s1 = 12345
      s2 = 34567

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '      R                     S1            S2'
      write ( *, '(a)' ) ' '
      write ( *, '(2x,14x,2x,i12,2x,i12)' ) s1, s2

      do i = 1, 10
        r = r8_uni ( s1, s2 )
        write ( *, '(2x,g14.6,2x,i12,2x,i12)' ) r, s1, s2
      end do

      return
      end
      subroutine test11

c*********************************************************************72
c
cc TEST11 tests R8_UNI.
c
c  Modified:
c
c    08 July 2008
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer n
      parameter ( n = 100000 )

      integer i
      double precision r8_uni
      integer s1
      integer s2
      double precision u(n)
      double precision u_avg
      double precision u_var

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST11'
      write ( *, '(a)' ) '  Examine the average and variance of a'
      write ( *, '(a)' ) '  sequence generated by R8_UNI.'
c
c  Start with known seeds.
c
      s1 = 12345
      s2 = 34567

      write ( *, '(a)' ) ' '
      write ( *, '(a,i10,a)' ) '  Now compute ', n, ' elements.'

      u_avg = 0.0D+00
      do i = 1, n
        u(i) = r8_uni ( s1, s2 )
        u_avg = u_avg + u(i)
      end do

      u_avg = u_avg / dble ( n )

      u_var = 0.0D+00
      do i = 1, n
        u_var = u_var + ( u(i) - u_avg ) * ( u(i) - u_avg )
      end do

      u_var = u_var / dble ( n - 1 )

      write ( *, '(a)' ) ' '
      write ( *, '(a,f10.6)' ) '  Average value = ', u_avg
      write ( *, '(a,f10.6)' ) '  Expecting       ', 0.5D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a,f10.6)' ) '  Variance =      ', u_var
      write ( *, '(a,f10.6)' ) '  Expecting       ', 1.0D+00 / 12.0D+00

      return
      end
      subroutine test12

c*********************************************************************72
c
cc TEST12 tests R8_UNI.
c
c  Modified:
c
c    08 July 2008
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i
      double precision r
      double precision r8_uni
      integer s1
      integer s1_save
      integer s2
      integer s2_save

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST12'
      write ( *, '(a)' ) '  Show how the seeds used by R8_UNI,'
      write ( *, '(a)' ) '  which change on each step, can be reset to'
      write ( *, '(a)' ) '  restore any part of the sequence.'

      s1_save = 12345
      s2_save = 34567

      s1 = s1_save
      s2 = s2_save

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Begin sequence with following seeds:'
      write ( *, '(a)' ) ' '
      write ( *, '(a,i8)' ) '  S1 = ', s1
      write ( *, '(a,i8)' ) '  S2 = ', s2
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '         I      R                     S1            S2'
      write ( *, '(a)' ) ' '

      do i = 1, 10

        r = r8_uni ( s1, s2 )
        write ( *, '(2x,i8,2x,g14.6,2x,i12,2x,i12)' ) 
     &    i, r, s1, s2

        if ( i == 5 ) then
          s1_save = s1
          s2_save = s2
        end if

      end do

      s1 = s1_save
      s2 = s2_save

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Restart the sequence, using the seeds'
      write ( *, '(a)' ) '  produced after step 5:'
      write ( *, '(a)' ) ' '
      write ( *, '(a,i12)' ) '  S1 = ', s1
      write ( *, '(a,i12)' ) '  S2 = ', s2
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '         I      R                     S1            S2'
      write ( *, '(a)' ) ' '

      do i = 1, 10

        r = r8_uni ( s1, s2 )
        write ( *, '(2x,i8,2x,g14.6,2x,i12,2x,i12)' ) 
     &    i, r, s1, s2

      end do

      return
      end
