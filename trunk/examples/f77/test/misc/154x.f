c  Source: https://github.com/johannesgerer/jburkardt-f77
c  File name: asa239/asa239.f
c
      function alngam ( xvalue, ifault )

c*********************************************************************72
c
cc ALNGAM computes the logarithm of the gamma function.
c
c  Modified:
c
c    30 March 1999
c
c  Author:
c
c    Alan Macleod
c    Modifications by John Burkardt
c
c  Reference:
c
c    Allan Macleod,
c    Algorithm AS 245:
c    A Robust and Reliable Algorithm for the Logarithm of the Gamma Function,
c    Applied Statistics,
c    Volume 38, Number 2, 1989, pages 397-402.
c
c  Parameters:
c
c    Input, double precision XVALUE, the argument of the Gamma function.
c
c    Output, integer IFAULT, error flag.
c    0, no error occurred.
c    1, XVALUE is less than or equal to 0.
c    2, XVALUE is too big.
c
c    Output, double precision ALNGAM, the logarithm of the gamma function of X.
c
      implicit none

      double precision alngam
      double precision alr2pi
      parameter ( alr2pi = 0.918938533204673D+00 )
      integer ifault
      double precision r1(9)
      double precision r2(9)
      double precision r3(9)
      double precision r4(5)
      double precision x
      double precision x1
      double precision x2
      double precision xlge
      parameter ( xlge = 5.10D+06 )
      double precision xlgst
      parameter ( xlgst = 1.0D+30 )
      double precision xvalue
      double precision y

      data r1 /
     &  -2.66685511495D+00,
     &  -24.4387534237D+00,
     &  -21.9698958928D+00,
     &   11.1667541262D+00,
     &   3.13060547623D+00,
     &   0.607771387771D+00,
     &   11.9400905721D+00,
     &   31.4690115749D+00,
     &   15.2346874070D+00 /

      data r2 /
     &  -78.3359299449D+00,
     &  -142.046296688D+00,
     &   137.519416416D+00,
     &   78.6994924154D+00,
     &   4.16438922228D+00,
     &   47.0668766060D+00,
     &   313.399215894D+00,
     &   263.505074721D+00,
     &   43.3400022514D+00 /

      data r3 /
     &  -2.12159572323D+05,
     &   2.30661510616D+05,
     &   2.74647644705D+04,
     &  -4.02621119975D+04,
     &  -2.29660729780D+03,
     &  -1.16328495004D+05,
     &  -1.46025937511D+05,
     &  -2.42357409629D+04,
     &  -5.70691009324D+02 /

      data r4 / 
     &   0.279195317918525D+00, 
     &   0.4917317610505968D+00,
     &   0.0692910599291889D+00, 
     &   3.350343815022304D+00,
     &   6.012459259764103D+00 /

      x = xvalue
      alngam = 0.0D+00
c
c  Check the input.
c
      if ( xlgst .le. x ) then
        ifault = 2
        return
      end if

      if ( x .le. 0.0D+00 ) then
        ifault = 1
        return
      end if

      ifault = 0
c
c  Calculation for 0 < X < 0.5 and 0.5 <= X < 1.5 combined.
c
      if ( x .lt. 1.5D+00 ) then

        if ( x .lt. 0.5D+00 ) then

          alngam = - dlog ( x )
          y = x + 1.0D+00
c
c  Test whether X < machine epsilon.
c
          if ( y .eq. 1.0D+00 ) then
            return
          end if

        else

          alngam = 0.0D+00
          y = x
          x = ( x - 0.5D+00 ) - 0.5D+00

        end if

        alngam = alngam + x * ((((
     &      r1(5)   * y 
     &    + r1(4) ) * y 
     &    + r1(3) ) * y
     &    + r1(2) ) * y 
     &    + r1(1) ) / ((((
     &                y 
     &    + r1(9) ) * y 
     &    + r1(8) ) * y
     &    + r1(7) ) * y 
     &    + r1(6) )

        return

      end if
c
c  Calculation for 1.5 <= X < 4.0.
c
      if ( x .lt. 4.0D+00 ) then

        y = ( x - 1.0D+00 ) - 1.0D+00

        alngam = y * ((((
     &      r2(5)   * x 
     &    + r2(4) ) * x 
     &    + r2(3) ) * x 
     &    + r2(2) ) * x
     &    + r2(1) ) / ((((
     &                x 
     &    + r2(9) ) * x 
     &    + r2(8) ) * x 
     &    + r2(7) ) * x
     &    + r2(6) )
c
c  Calculation for 4.0 <= X < 12.0.
c
      else if ( x .lt. 12.0D+00 ) then

        alngam = ((((
     &      r3(5)   * x 
     &    + r3(4) ) * x 
     &    + r3(3) ) * x 
     &    + r3(2) ) * x 
     &    + r3(1) ) / (((( 
     &                x 
     &    + r3(9) ) * x 
     &    + r3(8) ) * x 
     &    + r3(7) ) * x 
     &    + r3(6) )
c
c  Calculation for X >= 12.0.
c
      else

        y = dlog ( x )
        alngam = x * ( y - 1.0D+00 ) - 0.5D+00 * y + alr2pi

        if ( x .le. xlge ) then

          x1 = 1.0D+00 / x
          x2 = x1 * x1

          alngam = alngam + x1 * ( ( 
     &           r4(3)   * 
     &      x2 + r4(2) ) * 
     &      x2 + r4(1) ) / ( ( 
     &      x2 + r4(5) ) * 
     &      x2 + r4(4) )

        end if

      end if

      return
      end
      function alnorm ( x, upper )

c*********************************************************************72
c
cc ALNORM computes the cumulative density of the standard normal distribution.
c
c  Modified:
c
c    28 March 1999
c
c  Author:
c
c    David Hill
c    Modifications by John Burkardt
c
c  Reference:
c
c    David Hill,
c    Algorithm AS 66:
c    The Normal Integral,
c    Applied Statistics,
c    Volume 22, Number 3, 1973, pages 424-427.
c
c  Parameters:
c
c    Input, double precision X, is one endpoint of the semi-infinite interval
c    over which the integration takes place.
c
c    Input, logical UPPER, determines whether the upper or lower
c    interval is to be integrated:
c    .TRUE.  => integrate from X to + Infinity;
c    .FALSE. => integrate from - Infinity to X.
c
c    Output, double precision ALNORM, the integral of the standard normal
c    distribution over the desired interval.
c
      double precision a1
      double precision a2
      double precision a3
      double precision b1
      double precision b2
      double precision c1
      double precision c2
      double precision c3
      double precision c4
      double precision c5
      double precision c6
      double precision con
      double precision d1
      double precision d2
      double precision d3
      double precision d4
      double precision d5
      double precision ltone
      double precision p
      double precision q
      double precision r
      double precision utzero

      parameter ( a1 = 5.75885480458D+00 )
      parameter ( a2 = 2.62433121679D+00 )
      parameter ( a3 = 5.92885724438D+00 )
      parameter ( b1 = -29.8213557807D+00 )
      parameter ( b2 = 48.6959930692D+00 )
      parameter ( c1 = -0.000000038052D+00 )
      parameter ( c2 = 0.000398064794D+00 )
      parameter ( c3 = -0.151679116635D+00 )
      parameter ( c4 = 4.8385912808D+00 )
      parameter ( c5 = 0.742380924027D+00 )
      parameter ( c6 = 3.99019417011D+00 )
      parameter ( con = 1.28D+00 )
      parameter ( d1 = 1.00000615302D+00 )
      parameter ( d2 = 1.98615381364D+00 )
      parameter ( d3 = 5.29330324926D+00 )
      parameter ( d4 = -15.1508972451D+00 )
      parameter ( d5 = 30.789933034D+00 )
      parameter ( ltone = 7.0D+00 )
      parameter ( p = 0.398942280444D+00 )
      parameter ( q = 0.39990348504D+00 )
      parameter ( r = 0.398942280385D+00 )
      parameter ( utzero = 18.66D+00 )

      double precision alnorm
      logical up
      logical upper
      double precision x
      double precision y
      double precision z

      up = upper
      z = x

      if ( z .lt. 0.0D+00 ) then
        up = .not. up
        z = - z
      end if

      if ( ltone .lt. z .and. 
     &  ( ( .not. up ) .or. utzero .lt. z ) ) then

        if ( up ) then
          alnorm = 0.0D+00
        else
          alnorm = 1.0D+00
        end if

        return

      end if

      y = 0.5D+00 * z * z

      if ( z .le. con ) then

        alnorm = 0.5D+00 - z * ( p - q * y
     &    / ( y + a1 + b1 
     &    / ( y + a2 + b2 
     &    / ( y + a3 ))))

      else

        alnorm = r * dexp ( - y )
     &    / ( z + c1 + d1
     &    / ( z + c2 + d2
     &    / ( z + c3 + d3
     &    / ( z + c4 + d4
     &    / ( z + c5 + d5
     &    / ( z + c6 ))))))

      end if

      if ( .not. up ) then
        alnorm = 1.0D+00 - alnorm
      end if

      return
      end
      subroutine gamma_inc_values ( n_data, a, x, fx )

c*********************************************************************72
c
cc GAMMA_INC_VALUES returns some values of the incomplete Gamma function.
c
c  Discussion:
c
c    The (normalized) incomplete Gamma function P(A,X) is defined as:
c
c      PN(A,X) = 1/Gamma(A) * Integral ( 0 <= T <= X ) T**(A-1) * exp(-T) dT.
c
c    With this definition, for all A and X,
c
c      0 <= PN(A,X) <= 1
c
c    and
c
c      PN(A,INFINITY) = 1.0
c
c    In Mathematica, the function can be evaluated by:
c
c      1 - GammaRegularized[A,X]
c
c  Modified:
c
c    19 January 2008
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
c    Stephen Wolfram,
c    The Mathematica Book,
c    Fourth Edition,
c    Cambridge University Press, 1999,
c    ISBN: 0-521-64314-7,
c    LC: QA76.95.W65.
c
c  Parameters:
c
c    Input/output, integer N_DATA.  The user sets N_DATA to 0 before the
c    first call.  On each call, the routine increments N_DATA by 1, and
c    returns the corresponding data; when there is no more data, the
c    output value of N_DATA will be 0 again.
c
c    Output, double precision A, the parameter of the function.
c
c    Output, double precision X, the argument of the function.
c
c    Output, double precision FX, the value of the function.
c
      implicit none

      integer n_max
      parameter ( n_max = 20 )

      double precision a
      double precision a_vec ( n_max )
      double precision fx
      double precision fx_vec ( n_max )
      integer n_data
      double precision x
      double precision x_vec ( n_max )

      data a_vec /
     &   0.10D+00, 
     &   0.10D+00, 
     &   0.10D+00, 
     &   0.50D+00, 
     &   0.50D+00, 
     &   0.50D+00, 
     &   0.10D+01, 
     &   0.10D+01, 
     &   0.10D+01, 
     &   0.11D+01, 
     &   0.11D+01, 
     &   0.11D+01, 
     &   0.20D+01, 
     &   0.20D+01, 
     &   0.20D+01, 
     &   0.60D+01, 
     &   0.60D+01, 
     &   0.11D+02, 
     &   0.26D+02, 
     &   0.41D+02 /
      data fx_vec /
     &   0.7382350532339351D+00, 
     &   0.9083579897300343D+00, 
     &   0.9886559833621947D+00, 
     &   0.3014646416966613D+00, 
     &   0.7793286380801532D+00, 
     &   0.9918490284064973D+00, 
     &   0.9516258196404043E-01, 
     &   0.6321205588285577D+00, 
     &   0.9932620530009145D+00, 
     &   0.7205974576054322E-01, 
     &   0.5891809618706485D+00, 
     &   0.9915368159845525D+00, 
     &   0.01018582711118352D+00, 
     &   0.4421745996289254D+00, 
     &   0.9927049442755639D+00, 
     &   0.4202103819530612E-01, 
     &   0.9796589705830716D+00, 
     &   0.9226039842296429D+00, 
     &   0.4470785799755852D+00, 
     &   0.7444549220718699D+00 /
      data x_vec /
     &   0.30E-01, 
     &   0.30D+00, 
     &   0.15D+01, 
     &   0.75D-01, 
     &   0.75D+00, 
     &   0.35D+01, 
     &   0.10D+00, 
     &   0.10D+01, 
     &   0.50D+01, 
     &   0.10D+00, 
     &   0.10D+01, 
     &   0.50D+01, 
     &   0.15D+00, 
     &   0.15D+01, 
     &   0.70D+01, 
     &   0.25D+01, 
     &   0.12D+02, 
     &   0.16D+02, 
     &   0.25D+02, 
     &   0.45D+02 /

      if ( n_data .lt. 0 ) then
        n_data = 0
      end if

      n_data = n_data + 1

      if ( n_max .lt. n_data ) then
        n_data = 0
        a = 0.0D+00
        x = 0.0D+00
        fx = 0.0D+00
      else
        a = a_vec(n_data)
        x = x_vec(n_data)
        fx = fx_vec(n_data)
      end if

      return
      end
      function gammad ( x, p, ifault )

c*********************************************************************72
c
cc GAMMAD computes the Incomplete Gamma Integral
c
c  Auxiliary functions:
c
c    ALOGAM = logarithm of the gamma function, 
c    ALNORM = algorithm AS66
c
c  Modified:
c
c    31 March 1999
c
c  Author:
c
c    B Shea
c    Modifications by John Burkardt
c
c  Reference:
c
c    B Shea,
c    Algorithm AS 239:
c    Chi-squared and Incomplete Gamma Integral,
c    Applied Statistics,
c    Volume 37, Number 3, 1988, pages 466-473.
c
c  Parameters:
c
c    Input, double precision X, P, the parameters of the incomplete gamma ratio.
c    0 <= X, and 0 < P.
c
c    Output, integer IFAULT, error flag.
c    0, no error.
c    1, X < 0 or P <= 0.
c
c    Output, double precision GAMMAD, the value of the incomplete Gamma integral.
c
      implicit none

      double precision a
      double precision alnorm
      double precision alngam
      double precision an
      double precision arg
      double precision b
      double precision c
      double precision elimit
      parameter ( elimit = - 88.0D+00 )
      double precision gammad
      integer ifault
      double precision oflo
      parameter ( oflo = 1.0D+37 )
      double precision p
      double precision plimit
      parameter ( plimit = 1000.0D+00 )
      double precision pn1
      double precision pn2
      double precision pn3
      double precision pn4
      double precision pn5
      double precision pn6
      double precision rn
      double precision tol
      parameter ( tol = 1.0D-14 )
      logical upper
      double precision x
      double precision xbig
      parameter ( xbig = 1.0D+08 )

      gammad = 0.0D+00
c
c  Check the input.
c
      if ( x .lt. 0.0D+00 ) then
        ifault = 1
        return
      end if

      if ( p .le. 0.0D+00 ) then
        ifault = 1
        return
      end if

      ifault = 0

      if ( x .eq. 0.0D+00 ) then
        gammad = 0.0D+00
        return
      end if
c
c  If P is large, use a normal approximation.
c
      if ( plimit .lt. p ) then

        pn1 = 3.0D+00 * dsqrt ( p ) * ( ( x / p )**( 1.0D+00 / 3.0D+00 ) 
     &  + 1.0D+00 / ( 9.0D+00 * p ) - 1.0D+00 )

        upper = .false.
        gammad = alnorm ( pn1, upper )
        return

      end if
c
c  If X is large set GAMMAD = 1.
c
      if ( xbig .lt. x ) then
        gammad = 1.0D+00
        return
      end if
c
c  Use Pearson's series expansion.
c  (Note that P is not large enough to force overflow in ALOGAM).
c  No need to test IFAULT on exit since P > 0.
c
      if ( x .le. 1.0D+00 .or. x .lt. p ) then

        arg = p * dlog ( x ) - x - alngam ( p + 1.0D+00, ifault )
        c = 1.0D+00
        gammad = 1.0D+00
        a = p

   40   continue

        a = a + 1.0D+00
        c = c * x / a
        gammad = gammad + c

        if ( c .gt. tol ) then
          go to 40
        end if

        arg = arg + dlog ( gammad )

        if ( elimit .le. arg ) then
          gammad = dexp ( arg )
        else
          gammad = 0.0D+00
        end if
c
c  Use a continued fraction expansion.
c
      else 

        arg = p * dlog ( x ) - x - alngam ( p, ifault )
        a = 1.0D+00 - p
        b = a + x + 1.0D+00
        c = 0.0D+00
        pn1 = 1.0D+00
        pn2 = x
        pn3 = x + 1.0D+00
        pn4 = x * b
        gammad = pn3 / pn4

   60   continue

        a = a + 1.0D+00
        b = b + 2.0D+00
        c = c + 1.0D+00
        an = a * c
        pn5 = b * pn3 - an * pn1
        pn6 = b * pn4 - an * pn2

        if ( pn6 .ne. 0.0D+00 ) then

          rn = pn5 / pn6

          if ( dabs ( gammad - rn ) .le. dmin1 ( tol, tol * rn ) ) then
            go to 80
          end if

          gammad = rn

        end if

        pn1 = pn3
        pn2 = pn4
        pn3 = pn5
        pn4 = pn6
c
c  Re-scale terms in continued fraction if terms are large.
c
        if ( dabs ( pn5 ) .ge. oflo ) then
          pn1 = pn1 / oflo
          pn2 = pn2 / oflo
          pn3 = pn3 / oflo
          pn4 = pn4 / oflo
        end if

        go to 60

   80   continue

        arg = arg + dlog ( gammad )

        if ( elimit .le. arg ) then
          gammad = 1.0D+00 - dexp ( arg )
        else
          gammad = 1.0D+00
        end if

      end if

      return
      end
      subroutine timestamp ( )

c*********************************************************************72
c
cc TIMESTAMP prints out the current YMDHMS date as a timestamp.
c
c  Discussion:
c
c    This FORTRAN77 version is made available for cases where the
c    FORTRAN90 version cannot be used.
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
