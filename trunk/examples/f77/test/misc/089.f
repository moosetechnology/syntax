c  Source: https://github.com/johannesgerer/jburkardt-f77
c  File name: toms463/toms463.f
c
      SUBROUTINE SCALE1 ( XMIN, XMAX, N, XMINP, XMAXP, DIST )

c*********************************************************************72
c
cc SCALE1
c
C  ANSI FORTRAN
C  GIVEN XMIN, XMAX AND N, SCALE1 FINDS A NEW RANGE XMINP AND
C  XMAXP DIVISIBLE INTO APPROXIMATELY N LINEAR INTERVALS
C  OF SIZE DIST.
C  VINT IS AN ARRAY OF ACCEPTABLE VALUES FOR DIST (TIMES
C  AN INTEGER POWER OF 10).
C  SQR IS AN ARRAY OF GEOMETRIC MEANS OF ADJACENT VALUES
C  OF VINT.  IT IS USED AS BREAK POINTS TO DETERMINE
C  WHICH VINT VALUE TO ASSIGN TO DIST.
c
      DIMENSION VINT(4), SQR(3)
      DATA VINT(1), VINT(2), VINT(3), VINT(4) / 1., 2., 5., 10. /
      DATA SQR(1), SQR(2), SQR(3) / 1.414214, 3.162278, 7.071068 /
c
C  CHECK WHETHER PROPER INPUT VALUES WERE SUPPLIED.
c
      IF ( XMIN .LT. XMAX .AND. N .GT. 0 ) GO TO 10
      WRITE ( 6, 99999 )
99999 FORMAT ( 34H IMPROPER INPUT SUPPLIED TO SCALE1 )
      RETURN
c
C  DEL ACCOUNTS FOR COMPUTER ROUND-OFF.
C  DEL SHOULD BE GREATER THAN THE ROUND-OFF EXPECTED FROM
C  A DIVISION AND FLOAT OPERATION.  IT SHOULD BE LESS THAN
C  THE MINIMUM INCREMENT OF THE PLOTTING DEVICE USED BY
C  THE MAIN PROGRAM (IN.) DIVIDED BY THE PLOT SIZE (IN.)
C  TIMES NUMBER OF INTERVALS N.
c
10    DEL = .00002
      FN = N
c
C  FIND APPROXIMATE INTERVAL SIZE A.
c
      A = ( XMAX - XMIN ) / FN
      AL = ALOG10 ( A )
      NAL = AL
      IF ( A .LT. 1. ) NAL = NAL - 1
c
C  A IS SCALED INTO VARIABLE NAMED B BETWEEN 1 AND 10.
c
      B = A / 10.**NAL
c
C  THE CLOSEST PERMISSIBLE VALUE FOR B IS FOUND.
c
      DO 20 I = 1, 3
        IF ( B .LT. SQR ( I ) ) GO TO 30
20    CONTINUE
      I = 4
c
C  THE INTERVAL SIZE IS COMPUTED.
c
30    DIST = VINT(I) * 10.**NAL
      FM1 = XMIN / DIST
      M1 = FM1
      IF ( FM1 .LT. 0. ) M1 = M1 - 1
      IF ( ABS ( FLOAT ( M1 ) + 1. - FM1 ) .LT. DEL ) M1 = M1 + 1
c
C  THE NEW MINIMUM AND MAXIMUM LIMITS ARE FOUND.
c
      XMINP = DIST * FLOAT ( M1 )
      FM2 = XMAX / DIST
      M2 = FM2 + 1.
      IF ( FM2 .LT. ( -1. ) ) M2 = M2 - 1
      IF ( ABS ( FM2 + 1. - FLOAT ( M2 ) ) .LT. DEL ) M2 = M2 - 1
      XMAXP = DIST * FLOAT ( M2 )
c
C  ADJUST LIMITS TO ACCOUNT FOR ROUND-OFF IF NECESSARY.
c
      IF ( XMINP .GT. XMIN ) XMINP = XMIN
      IF ( XMAXP .LT. XMAX ) XMAXP = XMAX
      RETURN
      END
      SUBROUTINE SCALE2 ( XMIN, XMAX, N, XMINP, XMAXP, DIST )

c*********************************************************************72
c
cc SCALE2
c
C  ANSI FORTRAN
C  GIVEN XMIN, XMAX AND N, SCALE2 FINDS A NEW RANGE XMINP AND
C  XMAXP DIVISIBLE INTO EXACTLY N LINEAR INTERVALS OF SIZE
C  DIST, WHERE N IS GREATER THAN 1.
c
      DIMENSION VINT(5)
      DATA VINT(1), VINT(2), VINT(3), VINT(4), VINT(5) / 1., 2.,
     &  5., 10., 20. /
c
C  CHECK WHETHER PROPER INPUT VALUES WERE SUPPLIED.
c
      IF ( XMIN .LT. XMAX .AND. N .GT. 0 ) GO TO 10
      WRITE ( 6, 99999 )
99999 FORMAT ( 34H IMPROPER INPUT SUPPLIED TO SCALE2 )
      RETURN
10    DEL = .00002
      FN = N
c
C  FIND APPROXIMATE INTERVAL SIZE A.
c
      A = ( XMAX - XMIN ) / FN
      AL = ALOG10 ( A )
      NAL = AL
      IF ( A .LT. 1. ) NAL = NAL - 1
c
C  A IS SCALED INTO VARIABLE NAMED B BETWEEN 1 AND 10.
c
      B = A / 10.**NAL
c
C  THE CLOSEST PERMISSIBLE VALUE FOR B IS FOUND.
c
      DO 20 I = 1, 3
        IF ( B .LT. ( VINT(I) + DEL ) ) GO TO 30
20    CONTINUE
      I = 4
c
C  THE INTERVAL SIZE IS COMPUTED.
c
30    DIST = VINT(I) * 10.**NAL
      FM1 = XMIN / DIST
      M1 = FM1
      IF ( FM1 .LT. 0. ) M1 = M1 - 1
      IF ( ABS ( FLOAT ( M1 ) + 1. - FM1 ) .LT. DEL ) M1 = M1 + 1
c
C  THE NEW MINIMUM AND MAXIMUM LIMITS ARE FOUND.
c
      XMINP = DIST * FLOAT ( M1 )
      FM2 = XMAX / DIST
      M2 = FM2 + 1.
      IF ( FM2 .LT. ( -1. ) ) M2 = M2 - 1
      IF ( ABS ( FM2 + 1. - FLOAT ( M2 ) ) .LT. DEL ) M2 = M2 - 1
      XMAXP = DIST * FLOAT ( M2 )
c
C  CHECK WHETHER A SECOND PASS IS REQUIRED.
c
      NP = M2 - M1
      IF ( NP .LE. N ) GO TO 40
      I = I + 1
      GO TO 30
40    NX = ( N - NP ) / 2
      XMINP = XMINP - FLOAT ( NX ) * DIST
      XMAXP = XMINP + FLOAT ( N ) * DIST
c
C  ADJUST LIMITS TO ACCOUNT FOR ROUND-OFF IF NECESSARY.
c
      IF ( XMINP .GT. XMIN ) XMINP = XMIN
      IF ( XMAXP .LT. XMAX ) XMAXP = XMAX
      RETURN
      END
      SUBROUTINE SCALE3 ( XMIN, XMAX, N, XMINP, XMAXP, DIST )

c*********************************************************************72
c
cc SCALE3
c
C  ANSI FORTRAN
C  GIVEN XMIN, XMAX AND N, WHERE N IS GREATER THAN 1, SCALE3
C  FINDS A NEW RANGE XMINP AND XMAXP DIVISIBLE INTO EXACTLY
C  N LOGARITHMIC INTERVALS, WHERE THE RATIO OF ADJACENT
C  UNIFORMLY SPACED SCALE VALUES IS DIST.
c
      DIMENSION VINT(11)
      DATA VINT(1), VINT(2), VINT(3), VINT(4), VINT(5), VINT(6),
     &  VINT(7), VINT(8), VINT(9), VINT(10), VINT(11) / 10., 9.,
     &  8., 7., 6., 5., 4., 3., 2., 1., .5 /
c
C  CHECK WHETHER PROPER INPUT VALUES WERE SUPPLIED.
c
      IF ( XMIN .LT. XMAX .AND. N .GT. 1 .AND. XMIN .GT. 0. ) GO TO 10
      WRITE ( 6, 99999 )
99999 FORMAT ( 34H IMPROPER INPUT SUPPLIED TO SCALE3 )
      RETURN
10    DEL = .00002
c
C  VALUES ARE TRANSLATED FROM THE LINEAR INTO LOGARITHMIC
C  REGION.
c
      XMINL = ALOG10 ( XMIN )
      XMAXL = ALOG10 ( XMAX )
      FN = N
c
C  FIND APPROXIMATE INTERVAL SIZE A.
c
      A = ( XMAXL - XMINL ) / FN
      AL = ALOG10 ( A )
      NAL = AL
      IF ( A .LT. 1. ) NAL = NAL - 1
c
C  A IS SCALED INTO VARIABLE NAMED B BETWEEN 1 AND 10.
c
      B = A / 10.**VAL
c
C  THE CLOSEST PERMISSIBLE VALUE FOR B IS FOUND.
c
      DO 20 I = 1, 9
        IF ( B .LT. ( 10. / VINT(I) + DEL ) ) GO TO 30
20    CONTINUE
      I = 10
c
C  THE INTERVAL SIZE IS COMPUTED.
c
30    DISTL = 10.**( NAL + 1 ) / VINT(I)
      FM1 = XMINL / DISTL
      M1 = FM1
      IF ( FM1 .LT. 0. ) M1 = M1 - 1
      IF ( ABS ( FLOAT ( M1 ) + 1. - FM1 ) .LT. DEL ) M1 = M1 + 1
c
C  THE NEW MINIMUM AND MAXIMUM LIMITS ARE FOUND.
c
      XMINP = DISTL * FLOAT ( M1 )
      FM2 = XMAXL / DISTL
      M2 = FM2 + 1.
      IF ( FM2 .LT. ( -1. ) ) M2 = M2 - 1
      IF ( ABS ( FM2 + 1. - FLOAT ( M2 ) ) .LT. DEL ) M2 = M2 - 1
      XMAXP = DISTL + FLOAT ( M2 )
      NP = M2 - M1
c
C  CHECK WHETHER ANOTHER PASS IS NECESSARY.
c
      IF ( NP .LE. N ) GO TO 40
      I = I + 1
      GO TO 30
40    NX = ( N - NP ) / 2
      XMINP = XMINP - FLOAT ( NX ) * DISTL
      XMAXP = XMINP + FLOAT ( N  ) * DISTL
c
C  VALUES ARE TRANSLATED FROM THE LOGARITHMIC INTO THE LINEAR
C  REGION.
c
      DIST = 10.**DISTL
      XMINP = 10.**XMINP
      XMAXP = 10.**XMAXP
c
C  ADJUST LIMITS TO ACCOUNT FOR ROUND-OFF IF NECESSARY.
c
      IF ( XMINP .GT. XMIN ) XMINP = XMIN
      IF ( XMAXP .LT. XMAX ) XMAXP = XMAX
      RETURN
      END
