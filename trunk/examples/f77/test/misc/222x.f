c  Source: https://github.com/johannesgerer/jburkardt-f77
c  File name: files_multiple/files_multiple.f
c
      program main

c*********************************************************************72
c
cc FILES_MULTIPLE demonstrates how to work with multiple files.
c
c  Licensing:
c
c    This code is distributed under the GNU LGPL license. 
c
c  Modified:
c
c    01 December 2012
c
c  Author:
c
c    John Burkardt
c
      implicit none

      call timestamp ( )
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'FILES_MULTIPLE:'
      write ( *, '(a)' ) '  FORTRAN77 version.'
      write ( *, '(a)' ) 
     &  '  Demonstrate how to work with multiple files.'

      call test01 ( )
      call test02 ( )
c
c  Terminate.
c
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'FILES_MULTIPLE:'
      write ( *, '(a)' ) '  Normal end of execution.'
      write ( *, '(a)' ) ' '
      call timestamp ( )

      stop
      end
      subroutine test01 ( )

c*********************************************************************72
c
cc TEST01 writes data to four files which are open simultaneously.
c
c  Discussion:
c
c    The files contain the integers which are divisible by 2, 3, 5, and 7.
c
c    divisor1.txt  divisor2.txt  divisor3.txt  divisor4.txt
c        0             0             0             0
c        2             3             5             7
c        4             6            10            14
c        6             9            15            21
c        8            12            20            28
c       ...
c
c  Licensing:
c
c    This code is distributed under the GNU LGPL license. 
c
c  Modified:
c
c    19 September 2012
c
c  Author:
c
c    John Burkardt
c
      implicit none
c
c  FILENUM can be increased, and the program will still work the same way.
c
      integer filenum
      parameter ( filenum = 4 )

      integer divisor(filenum)
      character * ( 80 ) filename(filenum)
      integer fileunit(filenum)
      integer i
      integer j
      integer prime
      character * ( 80 ) template

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST01'
      write ( *, '(a)' ) '  Create 4 files, and count to 100.'
      write ( *, '(a)' ) '  "divisor1.txt" will contain multiples of 2;'
      write ( *, '(a)' ) '  "divisor2.txt" will contain multiples of 3;'
      write ( *, '(a)' ) '  "divisor3.txt" will contain multiples of 5;'
      write ( *, '(a)' ) '  "divisor4.txt" will contain multiples of 7;'
c
c  Set the divisors.
c  PRIME is a function which can return the I-th prime number.
c
      do i = 1, filenum
        divisor(i) = prime ( i )
      end do
c
c  Define the file names for an arbitrary number of files
c  by incrementing the numeric information in a "template".
c
      template = 'divisor0.txt'

      do i = 1, filenum
        call filename_inc ( template )
        filename(i) = template
      end do
c
c  Associate a unit number with each file.
c  While "fileunit(i) = i" will probably work, you might find that
c  some numbers are reserved for system use.  In that case, the call
c  "call get_unit ( fileunit(i) )" should work instead.
c
      do i = 1, filenum
        fileunit(i) = i
c       call get_unit ( fileunit(i) )
        open ( unit = fileunit(i), file = filename(i) )
        write ( fileunit(i), '(a,i2)' ) 'Multiples of ', divisor(i)
      end do

      do j = 0, 100
        do i = 1, filenum
          if ( mod ( j, divisor(i) ) .eq. 0 ) then
            write ( fileunit(i), * ) j
          end if
        end do
      end do
c
c  Close the files.
c
      do i = 1, filenum
        close ( unit = fileunit(i) )
      end do

      return
      end
      subroutine test02 ( )

c*********************************************************************72
c
cc TEST02 writes selected data to four files which are open simultaneously.
c
c  Discussion:
c
c    The vector X contains 100 random numbers, and we update X 20 times.
c    We want to keep track of X(10), X(25), X(64) and X(81) on each step.
c
c              x1.txt        x2.txt        x3.txt        x4.txt
c           X(10) values  X(25) values  X(64) values  X(81) values
c     1         2.1           1.7           5.3           9.8
c     2         2.0           3.6           6.2           7.3
c     3         2.5           4.1           5.9           6.8
c    ..   
c    10         5.0           5.2           5.3           5.6
c
c  Licensing:
c
c    This code is distributed under the GNU LGPL license. 
c
c  Modified:
c
c    01 December 2012
c
c  Author:
c
c    John Burkardt
c
      implicit none
c
c  FILENUM can be increased, but more entries in K would have to be defined
c  by the user.
c
      integer filenum
      parameter ( filenum = 4 )
      integer n
      parameter ( n = 100 )

      integer divisor(filenum)
      character * ( 80 ) filename(filenum)
      integer fileunit(filenum)
      integer i
      integer j
      integer k(filenum)
      integer seed
      character * ( 80 ) template
      double precision x(n)
      double precision y(n)

      save k

      data k / 10, 25, 64, 81 /

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST02'
      write ( *, '(a)' ) '  Create 4 files.'
      write ( *, '(a)' ) '  The vector X contains 100 values,'
      write ( *, '(a)' ) '  and is set 20 times.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '  Write X(10), X(25), X(64) and X(81) to the files'
      write ( *, '(a)' ) '  "x1.txt", "x2.txt", "x3.txt" and "x4.txt"'
c
c  Define the file names for an arbitrary number of files
c  by incrementing the numeric information in a "template".
c
      template = 'x0.txt'

      do i = 1, filenum
        call filename_inc ( template )
        filename(i) = template
      end do
c
c  Associate a unit number with each file.
c  While "fileunit(i) = i" will probably work, you might find that
c  some numbers are reserved for system use.  In that case, the call
c  "call get_unit ( fileunit(i) )" should work instead.
c
      do i = 1, filenum
        fileunit(i) = i
c       call get_unit ( fileunit(i) )
        open ( unit = fileunit(i), file = filename(i) )
        write ( fileunit(i), '(a,i2,a)' ) 'Values of X(', k(i), ')'
      end do
c
c  Initial X vector is random.
c
      seed = 123456789
      call r8vec_uniform_01 ( n, seed, x )
c
c  Write selected data to file, then update data.
c
      do j = 0, 20

        do i = 1, filenum
          write ( fileunit(i), '(2x,i2,2x,g14.6)' ) j, x(k(i))
        end do

        if ( j .lt. 20 ) then

          y(1) = ( x(1) + x(2) ) / 2.0D+00
          do i = 2, 99
            y(i) = ( x(i-1) + x(i) + x(i+1) ) / 3.0D+00
          end do
          y(100) = ( x(99) + x(100) ) / 2.0D+00

          do i = 1, 100
            x(i) = y(i)
          end do

        end if

      end do
c
c  Close the files.
c
      do i = 1, filenum
        close ( unit = fileunit(i) )
      end do

      return
      end
      function ch_is_digit ( c )

c*********************************************************************72
c
cc CH_IS_DIGIT returns TRUE if a character is a decimal digit.
c
c  Licensing:
c
c    This code is distributed under the GNU LGPL license.
c
c  Modified:
c
c    15 January 1999
c
c  Author:
c
c    John Burkardt
c
c  Parameters:
c
c    Input, character C, the character to be analyzed.
c
c    Output, logical CH_IS_DIGIT, TRUE if C is a digit, FALSE otherwise.
c
      implicit none

      character c
      logical ch_is_digit

      if ( lge ( c, '0' ) .and. lle ( c, '9' ) ) then
        ch_is_digit = .true.
      else
        ch_is_digit = .false.
      end if

      return
      end
      subroutine ch_to_digit ( c, digit )

c*********************************************************************72
c
cc CH_TO_DIGIT returns the integer value of a base 10 digit.
c
c  Example:
c
c     C   DIGIT
c    ---  -----
c    '0'    0
c    '1'    1
c    ...  ...
c    '9'    9
c    ' '    0
c    'X'   -1
c
c  Licensing:
c
c    This code is distributed under the GNU LGPL license.
c
c  Modified:
c
c    04 August 1999
c
c  Author:
c
c    John Burkardt
c
c  Parameters:
c
c    Input, character C, the decimal digit, '0' through '9' or blank
c    are legal.
c
c    Output, integer DIGIT, the corresponding integer value.  If C was
c    'illegal', then DIGIT is -1.
c
      implicit none

      character c
      integer digit

      if ( lge ( c, '0' ) .and. lle ( c, '9' ) ) then

        digit = ichar ( c ) - 48

      else if ( c .eq. ' ' ) then

        digit = 0

      else

        digit = -1

      end if

      return
      end
      subroutine digit_inc ( c )

c*********************************************************************72
c
cc DIGIT_INC increments a decimal digit.
c
c  Example:
c
c    Input  Output
c    -----  ------
c    '0'    '1'
c    '1'    '2'
c    ...
c    '8'    '9'
c    '9'    '0'
c    'A'    'A'
c
c  Licensing:
c
c    This code is distributed under the GNU LGPL license.
c
c  Modified:
c
c    04 August 1999
c
c  Author:
c
c    John Burkardt
c
c  Parameters:
c
c    Input/output, character C, a digit to be incremented.
c
      implicit none

      character c
      integer digit

      call ch_to_digit ( c, digit )

      if ( digit .eq. -1 ) then
        return
      end if

      digit = digit + 1

      if ( digit .eq. 10 ) then
        digit = 0
      end if

      call digit_to_ch ( digit, c )

      return
      end
      subroutine digit_to_ch ( digit, c )

c*********************************************************************72
c
cc DIGIT_TO_CH returns the character representation of a decimal digit.
c
c  Example:
c
c    DIGIT   C
c    -----  ---
c      0    '0'
c      1    '1'
c    ...    ...
c      9    '9'
c     17    '*'
c
c  Licensing:
c
c    This code is distributed under the GNU LGPL license.
c
c  Modified:
c
c    04 August 1999
c
c  Author:
c
c    John Burkardt
c
c  Parameters:
c
c    Input, integer DIGIT, the digit value between 0 and 9.
c
c    Output, character C, the corresponding character, or '*' if DIGIT
c    was illegal.
c
      implicit none

      character c
      integer digit

      if ( 0 .le. digit .and. digit .le. 9 ) then

        c = char ( digit + 48 )

      else

        c = '*'

      end if

      return
      end
      subroutine filename_inc ( filename )

c*********************************************************************72
c
cc FILENAME_INC increments a partially numeric filename.
c
c  Discussion:
c
c    It is assumed that the digits in the name, whether scattered or
c    connected, represent a number that is to be increased by 1 on
c    each call.  Non-numeric letters of the name are unaffected.
c
c    If the name is empty, then the routine stops.
c
c    If the name contains no digits, the empty string is returned.
c
c  Example:
c
c      Input          Output
c      -----          ------
c      a7to11.txt     a7to12.txt
c      a7to99.txt     a8to00.txt
c      a9to99.txt     a0to00.txt
c      cat.txt        ' '
c      ' '            STOP!
c
c  Licensing:
c
c    This code is distributed under the GNU LGPL license.
c
c  Modified:
c
c    22 November 2011
c
c  Author:
c
c    John Burkardt
c
c  Parameters:
c
c    Input/output, character*(*) FILENAME.
c    On input, a character string to be incremented.
c    On output, the incremented string.
c
      implicit none

      character c
      logical ch_is_digit
      integer change
      integer digit
      character*(*) filename
      integer i
      integer lens

      lens = len_trim ( filename )

      if ( lens .le. 0 ) then
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'FILENAME_INC - Fatal error!'
        write ( *, '(a)' ) '  The input string is empty.'
        stop
      end if

      change = 0

      do i = lens, 1, -1

        c = filename(i:i)

        if ( ch_is_digit ( c ) ) then

          change = change + 1

          call digit_inc ( c )

          filename(i:i) = c

          if ( c .ne. '0' ) then
            return
          end if

        end if

      end do
c
c  No digits were found.  Return blank.
c
      if ( change .eq. 0 ) then
        filename = ' '
        return
      end if

      return
      end
      subroutine get_unit ( unit )

c*********************************************************************72
c
cc GET_UNIT returns a free FORTRAN unit number.
c
c  Discussion:
c
c    A "free" FORTRAN unit number is an integer between 1 and 99 which
c    is not currently associated with an I/O device.  A free FORTRAN unit
c    number is needed in order to open a file with the OPEN command.
c
c    If UNIT = 0, then no free FORTRAN unit could be found, although
c    all 99 units were checked (except for units 5, 6 and 9, which
c    are commonly reserved for console I/O).
c
c    Otherwise, UNIT is an integer between 1 and 99, representing a
c    free FORTRAN unit.  Note that GET_UNIT assumes that units 5 and 6
c    are special, and will never return those values.
c
c  Licensing:
c
c    This code is distributed under the GNU LGPL license.
c
c  Modified:
c
c    26 October 2008
c
c  Author:
c
c    John Burkardt
c
c  Parameters:
c
c    Output, integer UNIT, the free unit number.
c
      implicit none

      integer i
      integer unit

      unit = 0

      do i = 1, 99

        if ( i .ne. 5 .and. i .ne. 6 .and. i .ne. 9 ) then

          open ( unit = i, err = 10, status = 'scratch' )
          close ( unit = i )

          unit = i

          return
        end if

10      continue

      end do

      return
      end
      function prime ( n )

c*********************************************************************72
c
cc PRIME returns any of the first PRIME_MAX prime numbers.
c
c  Discussion:
c
c    PRIME_MAX is 1600, and the largest prime stored is 13499.
c
c    Thanks to Bart Vandewoestyne for pointing out a typo, 18 February 2005.
c
c  Licensing:
c
c    This code is distributed under the GNU LGPL license.
c
c  Modified:
c
c    21 January 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    Milton Abramowitz, Irene Stegun,
c    Handbook of Mathematical Functions,
c    National Bureau of Standards, 1964,
c    ISBN: 0-486-61272-4,
c    LC: QA47.A34.
c
c    Daniel Zwillinger,
c    CRC Standard Mathematical Tables and Formulae,
c    30th Edition,
c    CRC Press, 1996, pages 95-98.
c
c  Parameters:
c
c    Input, integer N, the index of the desired prime number.
c    In general, is should be true that 0 <= N <= PRIME_MAX.
c    N = -1 returns PRIME_MAX, the index of the largest prime available.
c    N = 0 is legal, returning PRIME = 1.
c
c    Output, integer PRIME, the N-th prime.  If N is out of range,
c    PRIME is returned as -1.
c
      implicit none

      integer prime_max
      parameter ( prime_max = 1600 )

      integer i
      integer n
      integer npvec(prime_max)
      integer prime

      save npvec

      data ( npvec(i), i = 1, 100 ) /
     &      2,    3,    5,    7,   11,   13,   17,   19,   23,   29,
     &     31,   37,   41,   43,   47,   53,   59,   61,   67,   71,
     &     73,   79,   83,   89,   97,  101,  103,  107,  109,  113,
     &    127,  131,  137,  139,  149,  151,  157,  163,  167,  173,
     &    179,  181,  191,  193,  197,  199,  211,  223,  227,  229,
     &    233,  239,  241,  251,  257,  263,  269,  271,  277,  281,
     &    283,  293,  307,  311,  313,  317,  331,  337,  347,  349,
     &    353,  359,  367,  373,  379,  383,  389,  397,  401,  409,
     &    419,  421,  431,  433,  439,  443,  449,  457,  461,  463,
     &    467,  479,  487,  491,  499,  503,  509,  521,  523,  541 /

      data ( npvec(i), i = 101, 200 ) /
     &    547,  557,  563,  569,  571,  577,  587,  593,  599,  601,
     &    607,  613,  617,  619,  631,  641,  643,  647,  653,  659,
     &    661,  673,  677,  683,  691,  701,  709,  719,  727,  733,
     &    739,  743,  751,  757,  761,  769,  773,  787,  797,  809,
     &    811,  821,  823,  827,  829,  839,  853,  857,  859,  863,
     &    877,  881,  883,  887,  907,  911,  919,  929,  937,  941,
     &    947,  953,  967,  971,  977,  983,  991,  997, 1009, 1013,
     &   1019, 1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069,
     &   1087, 1091, 1093, 1097, 1103, 1109, 1117, 1123, 1129, 1151,
     &   1153, 1163, 1171, 1181, 1187, 1193, 1201, 1213, 1217, 1223 /

      data ( npvec(i), i = 201, 300 ) /
     &   1229, 1231, 1237, 1249, 1259, 1277, 1279, 1283, 1289, 1291,
     &   1297, 1301, 1303, 1307, 1319, 1321, 1327, 1361, 1367, 1373,
     &   1381, 1399, 1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451,
     &   1453, 1459, 1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511,
     &   1523, 1531, 1543, 1549, 1553, 1559, 1567, 1571, 1579, 1583,
     &   1597, 1601, 1607, 1609, 1613, 1619, 1621, 1627, 1637, 1657,
     &   1663, 1667, 1669, 1693, 1697, 1699, 1709, 1721, 1723, 1733,
     &   1741, 1747, 1753, 1759, 1777, 1783, 1787, 1789, 1801, 1811,
     &   1823, 1831, 1847, 1861, 1867, 1871, 1873, 1877, 1879, 1889,
     &   1901, 1907, 1913, 1931, 1933, 1949, 1951, 1973, 1979, 1987 /

      data ( npvec(i), i = 301, 400 ) /
     &   1993, 1997, 1999, 2003, 2011, 2017, 2027, 2029, 2039, 2053,
     &   2063, 2069, 2081, 2083, 2087, 2089, 2099, 2111, 2113, 2129,
     &   2131, 2137, 2141, 2143, 2153, 2161, 2179, 2203, 2207, 2213,
     &   2221, 2237, 2239, 2243, 2251, 2267, 2269, 2273, 2281, 2287,
     &   2293, 2297, 2309, 2311, 2333, 2339, 2341, 2347, 2351, 2357,
     &   2371, 2377, 2381, 2383, 2389, 2393, 2399, 2411, 2417, 2423,
     &   2437, 2441, 2447, 2459, 2467, 2473, 2477, 2503, 2521, 2531,
     &   2539, 2543, 2549, 2551, 2557, 2579, 2591, 2593, 2609, 2617,
     &   2621, 2633, 2647, 2657, 2659, 2663, 2671, 2677, 2683, 2687,
     &   2689, 2693, 2699, 2707, 2711, 2713, 2719, 2729, 2731, 2741 /

      data ( npvec(i), i = 401, 500 ) /
     &   2749, 2753, 2767, 2777, 2789, 2791, 2797, 2801, 2803, 2819,
     &   2833, 2837, 2843, 2851, 2857, 2861, 2879, 2887, 2897, 2903,
     &   2909, 2917, 2927, 2939, 2953, 2957, 2963, 2969, 2971, 2999,
     &   3001, 3011, 3019, 3023, 3037, 3041, 3049, 3061, 3067, 3079,
     &   3083, 3089, 3109, 3119, 3121, 3137, 3163, 3167, 3169, 3181,
     &   3187, 3191, 3203, 3209, 3217, 3221, 3229, 3251, 3253, 3257,
     &   3259, 3271, 3299, 3301, 3307, 3313, 3319, 3323, 3329, 3331,
     &   3343, 3347, 3359, 3361, 3371, 3373, 3389, 3391, 3407, 3413,
     &   3433, 3449, 3457, 3461, 3463, 3467, 3469, 3491, 3499, 3511,
     &   3517, 3527, 3529, 3533, 3539, 3541, 3547, 3557, 3559, 3571 /

      data ( npvec(i), i = 501, 600 ) /
     &   3581, 3583, 3593, 3607, 3613, 3617, 3623, 3631, 3637, 3643,
     &   3659, 3671, 3673, 3677, 3691, 3697, 3701, 3709, 3719, 3727,
     &   3733, 3739, 3761, 3767, 3769, 3779, 3793, 3797, 3803, 3821,
     &   3823, 3833, 3847, 3851, 3853, 3863, 3877, 3881, 3889, 3907,
     &   3911, 3917, 3919, 3923, 3929, 3931, 3943, 3947, 3967, 3989,
     &   4001, 4003, 4007, 4013, 4019, 4021, 4027, 4049, 4051, 4057,
     &   4073, 4079, 4091, 4093, 4099, 4111, 4127, 4129, 4133, 4139,
     &   4153, 4157, 4159, 4177, 4201, 4211, 4217, 4219, 4229, 4231,
     &   4241, 4243, 4253, 4259, 4261, 4271, 4273, 4283, 4289, 4297,
     &   4327, 4337, 4339, 4349, 4357, 4363, 4373, 4391, 4397, 4409 /

      data ( npvec(i), i = 601, 700 ) /
     &   4421, 4423, 4441, 4447, 4451, 4457, 4463, 4481, 4483, 4493,
     &   4507, 4513, 4517, 4519, 4523, 4547, 4549, 4561, 4567, 4583,
     &   4591, 4597, 4603, 4621, 4637, 4639, 4643, 4649, 4651, 4657,
     &   4663, 4673, 4679, 4691, 4703, 4721, 4723, 4729, 4733, 4751,
     &   4759, 4783, 4787, 4789, 4793, 4799, 4801, 4813, 4817, 4831,
     &   4861, 4871, 4877, 4889, 4903, 4909, 4919, 4931, 4933, 4937,
     &   4943, 4951, 4957, 4967, 4969, 4973, 4987, 4993, 4999, 5003,
     &   5009, 5011, 5021, 5023, 5039, 5051, 5059, 5077, 5081, 5087,
     &   5099, 5101, 5107, 5113, 5119, 5147, 5153, 5167, 5171, 5179,
     &   5189, 5197, 5209, 5227, 5231, 5233, 5237, 5261, 5273, 5279 /

      data ( npvec(i), i = 701, 800 ) /
     &   5281, 5297, 5303, 5309, 5323, 5333, 5347, 5351, 5381, 5387,
     &   5393, 5399, 5407, 5413, 5417, 5419, 5431, 5437, 5441, 5443,
     &   5449, 5471, 5477, 5479, 5483, 5501, 5503, 5507, 5519, 5521,
     &   5527, 5531, 5557, 5563, 5569, 5573, 5581, 5591, 5623, 5639,
     &   5641, 5647, 5651, 5653, 5657, 5659, 5669, 5683, 5689, 5693,
     &   5701, 5711, 5717, 5737, 5741, 5743, 5749, 5779, 5783, 5791,
     &   5801, 5807, 5813, 5821, 5827, 5839, 5843, 5849, 5851, 5857,
     &   5861, 5867, 5869, 5879, 5881, 5897, 5903, 5923, 5927, 5939,
     &   5953, 5981, 5987, 6007, 6011, 6029, 6037, 6043, 6047, 6053,
     &   6067, 6073, 6079, 6089, 6091, 6101, 6113, 6121, 6131, 6133 /

      data ( npvec(i), i = 801, 900 ) /
     &   6143, 6151, 6163, 6173, 6197, 6199, 6203, 6211, 6217, 6221,
     &   6229, 6247, 6257, 6263, 6269, 6271, 6277, 6287, 6299, 6301,
     &   6311, 6317, 6323, 6329, 6337, 6343, 6353, 6359, 6361, 6367,
     &   6373, 6379, 6389, 6397, 6421, 6427, 6449, 6451, 6469, 6473,
     &   6481, 6491, 6521, 6529, 6547, 6551, 6553, 6563, 6569, 6571,
     &   6577, 6581, 6599, 6607, 6619, 6637, 6653, 6659, 6661, 6673,
     &   6679, 6689, 6691, 6701, 6703, 6709, 6719, 6733, 6737, 6761,
     &   6763, 6779, 6781, 6791, 6793, 6803, 6823, 6827, 6829, 6833,
     &   6841, 6857, 6863, 6869, 6871, 6883, 6899, 6907, 6911, 6917,
     &   6947, 6949, 6959, 6961, 6967, 6971, 6977, 6983, 6991, 6997 /

      data ( npvec(i), i = 901, 1000 ) /
     &   7001, 7013, 7019, 7027, 7039, 7043, 7057, 7069, 7079, 7103,
     &   7109, 7121, 7127, 7129, 7151, 7159, 7177, 7187, 7193, 7207,
     &   7211, 7213, 7219, 7229, 7237, 7243, 7247, 7253, 7283, 7297,
     &   7307, 7309, 7321, 7331, 7333, 7349, 7351, 7369, 7393, 7411,
     &   7417, 7433, 7451, 7457, 7459, 7477, 7481, 7487, 7489, 7499,
     &   7507, 7517, 7523, 7529, 7537, 7541, 7547, 7549, 7559, 7561,
     &   7573, 7577, 7583, 7589, 7591, 7603, 7607, 7621, 7639, 7643,
     &   7649, 7669, 7673, 7681, 7687, 7691, 7699, 7703, 7717, 7723,
     &   7727, 7741, 7753, 7757, 7759, 7789, 7793, 7817, 7823, 7829,
     &   7841, 7853, 7867, 7873, 7877, 7879, 7883, 7901, 7907, 7919 /

      data ( npvec(i), i = 1001, 1100 ) /
     &   7927, 7933, 7937, 7949, 7951, 7963, 7993, 8009, 8011, 8017,
     &   8039, 8053, 8059, 8069, 8081, 8087, 8089, 8093, 8101, 8111,
     &   8117, 8123, 8147, 8161, 8167, 8171, 8179, 8191, 8209, 8219,
     &   8221, 8231, 8233, 8237, 8243, 8263, 8269, 8273, 8287, 8291,
     &   8293, 8297, 8311, 8317, 8329, 8353, 8363, 8369, 8377, 8387,
     &   8389, 8419, 8423, 8429, 8431, 8443, 8447, 8461, 8467, 8501,
     &   8513, 8521, 8527, 8537, 8539, 8543, 8563, 8573, 8581, 8597,
     &   8599, 8609, 8623, 8627, 8629, 8641, 8647, 8663, 8669, 8677,
     &   8681, 8689, 8693, 8699, 8707, 8713, 8719, 8731, 8737, 8741,
     &   8747, 8753, 8761, 8779, 8783, 8803, 8807, 8819, 8821, 8831 /

      data ( npvec(i), i = 1101, 1200 ) /
     &   8837, 8839, 8849, 8861, 8863, 8867, 8887, 8893, 8923, 8929,
     &   8933, 8941, 8951, 8963, 8969, 8971, 8999, 9001, 9007, 9011,
     &   9013, 9029, 9041, 9043, 9049, 9059, 9067, 9091, 9103, 9109,
     &   9127, 9133, 9137, 9151, 9157, 9161, 9173, 9181, 9187, 9199,
     &   9203, 9209, 9221, 9227, 9239, 9241, 9257, 9277, 9281, 9283,
     &   9293, 9311, 9319, 9323, 9337, 9341, 9343, 9349, 9371, 9377,
     &   9391, 9397, 9403, 9413, 9419, 9421, 9431, 9433, 9437, 9439,
     &   9461, 9463, 9467, 9473, 9479, 9491, 9497, 9511, 9521, 9533,
     &   9539, 9547, 9551, 9587, 9601, 9613, 9619, 9623, 9629, 9631,
     &   9643, 9649, 9661, 9677, 9679, 9689, 9697, 9719, 9721, 9733 /

      data ( npvec(i), i = 1201, 1300 ) /
     &   9739, 9743, 9749, 9767, 9769, 9781, 9787, 9791, 9803, 9811,
     &   9817, 9829, 9833, 9839, 9851, 9857, 9859, 9871, 9883, 9887,
     &   9901, 9907, 9923, 9929, 9931, 9941, 9949, 9967, 9973,10007,
     &  10009,10037,10039,10061,10067,10069,10079,10091,10093,10099,
     &  10103,10111,10133,10139,10141,10151,10159,10163,10169,10177,
     &  10181,10193,10211,10223,10243,10247,10253,10259,10267,10271,
     &  10273,10289,10301,10303,10313,10321,10331,10333,10337,10343,
     &  10357,10369,10391,10399,10427,10429,10433,10453,10457,10459,
     &  10463,10477,10487,10499,10501,10513,10529,10531,10559,10567,
     &  10589,10597,10601,10607,10613,10627,10631,10639,10651,10657 /

      data ( npvec(i), i = 1301, 1400 ) /
     &  10663,10667,10687,10691,10709,10711,10723,10729,10733,10739,
     &  10753,10771,10781,10789,10799,10831,10837,10847,10853,10859,
     &  10861,10867,10883,10889,10891,10903,10909,10937,10939,10949,
     &  10957,10973,10979,10987,10993,11003,11027,11047,11057,11059,
     &  11069,11071,11083,11087,11093,11113,11117,11119,11131,11149,
     &  11159,11161,11171,11173,11177,11197,11213,11239,11243,11251,
     &  11257,11261,11273,11279,11287,11299,11311,11317,11321,11329,
     &  11351,11353,11369,11383,11393,11399,11411,11423,11437,11443,
     &  11447,11467,11471,11483,11489,11491,11497,11503,11519,11527,
     &  11549,11551,11579,11587,11593,11597,11617,11621,11633,11657 /

      data ( npvec(i), i = 1401, 1500 ) /
     &  11677,11681,11689,11699,11701,11717,11719,11731,11743,11777,
     &  11779,11783,11789,11801,11807,11813,11821,11827,11831,11833,
     &  11839,11863,11867,11887,11897,11903,11909,11923,11927,11933,
     &  11939,11941,11953,11959,11969,11971,11981,11987,12007,12011,
     &  12037,12041,12043,12049,12071,12073,12097,12101,12107,12109,
     &  12113,12119,12143,12149,12157,12161,12163,12197,12203,12211,
     &  12227,12239,12241,12251,12253,12263,12269,12277,12281,12289,
     &  12301,12323,12329,12343,12347,12373,12377,12379,12391,12401,
     &  12409,12413,12421,12433,12437,12451,12457,12473,12479,12487,
     &  12491,12497,12503,12511,12517,12527,12539,12541,12547,12553 /

      data ( npvec(i), i = 1501, 1600 ) /
     &  12569,12577,12583,12589,12601,12611,12613,12619,12637,12641,
     &  12647,12653,12659,12671,12689,12697,12703,12713,12721,12739,
     &  12743,12757,12763,12781,12791,12799,12809,12821,12823,12829,
     &  12841,12853,12889,12893,12899,12907,12911,12917,12919,12923,
     &  12941,12953,12959,12967,12973,12979,12983,13001,13003,13007,
     &  13009,13033,13037,13043,13049,13063,13093,13099,13103,13109,
     &  13121,13127,13147,13151,13159,13163,13171,13177,13183,13187,
     &  13217,13219,13229,13241,13249,13259,13267,13291,13297,13309,
     &  13313,13327,13331,13337,13339,13367,13381,13397,13399,13411,
     &  13417,13421,13441,13451,13457,13463,13469,13477,13487,13499 /

      if ( n .eq. -1 ) then
        prime = prime_max
      else if ( n .eq. 0 ) then
        prime = 1
      else if ( n .le. prime_max ) then
        prime = npvec(n)
      else
        prime = -1
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'PRIME - Fatal error!'
        write ( *, '(a,i8)' ) '  Illegal prime index N = ', n
        write ( *, '(a,i8)' )
     &    '  N should be between 1 and PRIME_MAX =', prime_max
        stop
      end if

      return
      end
      subroutine r8vec_uniform_01 ( n, seed, r )

c*********************************************************************72
c
cc R8VEC_UNIFORM_01 returns a unit pseudorandom R8VEC.
c
c  Discussion:
c
c    An R8VEC is a vector of R8's.
c
c  Licensing:
c
c    This code is distributed under the GNU LGPL license.
c
c  Modified:
c
c    17 July 2006
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    Paul Bratley, Bennett Fox, Linus Schrage,
c    A Guide to Simulation,
c    Springer Verlag, pages 201-202, 1983.
c
c    Bennett Fox,
c    Algorithm 647:
c    Implementation and Relative Efficiency of Quasirandom
c    Sequence Generators,
c    ACM Transactions on Mathematical Software,
c    Volume 12, Number 4, pages 362-376, 1986.
c
c    Peter Lewis, Allen Goodman, James Miller,
c    A Pseudo-Random Number Generator for the System/360,
c    IBM Systems Journal,
c    Volume 8, pages 136-143, 1969.
c
c  Parameters:
c
c    Input, integer N, the number of entries in the vector.
c
c    Input/output, integer SEED, the "seed" value, which should NOT be 0.
c    On output, SEED has been updated.
c
c    Output, double precision R(N), the vector of pseudorandom values.
c
      implicit none

      integer n

      integer i
      integer k
      integer seed
      double precision r(n)

      do i = 1, n

        k = seed / 127773

        seed = 16807 * ( seed - k * 127773 ) - k * 2836

        if ( seed .lt. 0 ) then
          seed = seed + 2147483647
        end if

        r(i) = dble ( seed ) * 4.656612875D-10

      end do

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
