c  Source: https://github.com/johannesgerer/jburkardt-f77
c  File name: toms661/toms661_prb.f
c
C      ALGORITHM 661, COLLECTED ALGORITHMS FROM ACM.
C      THIS WORK PUBLISHED IN TRANSACTIONS ON MATHEMATICAL SOFTWARE,
C      VOL. 14, NO. 2, P.151.
C
C        		   QS3TEST
C
C   THIS PROGRAM TESTS THE SCATTERED DATA INTERPOLATION
C PACKAGE QSHEP3D BY PRINTING THE MAXIMUM ERRORS ASSOCIATED
C WITH INTERPOLATED VALUES AND GRADIENTS ON A 5 BY 5 BY 5
C UNIFORM GRID IN THE UNIT CUBE.  THE DATA SET CONSISTS
C OF 64 NODES WITH DATA VALUES TAKEN FROM A QUADRATIC FUNC-
C TION FOR WHICH THE METHOD IS EXACT.  THE RATIO OF MAXIMUM
C INTERPOLATION ERROR RELATIVE TO THE MACHINE PRECISION IS
C ALSO PRINTED.  THIS SHOULD BE O(1).  THE INTERPOLATED
C VALUES FROM QS3VAL AND QS3GRD ARE COMPARED FOR AGREEMENT.
C
      INTEGER LCELL(3,3,3), LNEXT(64)
      REAL X(64), Y(64), Z(64), F(64), RSQ(64), A(9,64),
     .           XYZMIN(3), XYZDEL(3)
      REAL P(5)
C
C QSHEP3 PARAMETERS AND LOGICAL UNIT FOR OUTPUT
C
      DATA N/64/,  NQ/17/,  NW/32/,  NR/3/,  LOUT/6/
C
C QUADRATIC TEST FUNCTION AND PARTIAL DERIVATIVES
C
      FQ(XX,YY,ZZ) = ((XX + 2.*YY + 3.*ZZ)/6.)**2
      FX(XX,YY,ZZ) = (XX + 2.*YY + 3.*ZZ)/18.
      FY(XX,YY,ZZ) = (XX + 2.*YY + 3.*ZZ)/9.
      FZ(XX,YY,ZZ) = (XX + 2.*YY + 3.*ZZ)/6.
C
C GENERATE A 4 BY 4 BY 4 GRID OF NODES IN THE UNIT CUBE.
C
      L = 0
      DO 1 K = 1,4
        ZL = FLOAT(K-1)/3.
        DO 1 J = 1,4
          YL = FLOAT(J-1)/3.
          DO 1 I = 1,4
            L = L + 1
            X(L) = FLOAT(I-1)/3.
            Y(L) = YL
    1            Z(L) = ZL
C
C COMPUTE THE DATA VALUES.
C
      DO 2 L = 1,N
    2        F(L) = FQ(X(L),Y(L),Z(L))
C
C COMPUTE PARAMETERS DEFINING THE INTERPOLANT Q.
C
      CALL QSHEP3 (N,X,Y,Z,F,NQ,NW,NR, LCELL,LNEXT,XYZMIN,
     .             XYZDEL,RMAX,RSQ,A,IER)
      IF (IER .NE. 0) GO TO 6
C
C GENERATE A 5 BY 5 BY 5 UNIFORM GRID OF INTERPOLATION
C   POINTS (P(I),P(J),P(K)) IN THE UNIT CUBE.  THE EIGHT
C   CORNERS COINCIDE WITH NODES.
C
      DO 3 I = 1,5
    3        P(I) = FLOAT(I-1)/4.
C
C COMPUTE THE MACHINE PRECISION EPS.
C
      EPS = 1.
    4 EPS = EPS/2.
      EP1 = EPS + 1.
      IF (STORE(EP1) .GT. 1.) GO TO 4
      EPS = EPS*2.
C
C COMPUTE INTERPOLATION ERRORS AND TEST FOR AGREEMENT IN THE
C   Q VALUES RETURNED BY QS3VAL AND QS3GRD.
C
      EQ = 0.
      EQX = 0.
      EQY = 0.
      EQZ = 0.
      DO 5 K = 1,5
        PZ = P(K)
        DO 5 J = 1,5
          PY = P(J)
          DO 5 I = 1,5
            PX = P(I)
            Q1 = QS3VAL (PX,PY,PZ,N,X,Y,Z,F,NR,LCELL,LNEXT,
     .                   XYZMIN,XYZDEL,RMAX,RSQ,A)
            CALL QS3GRD (PX,PY,PZ,N,X,Y,Z,F,NR,LCELL,LNEXT,
     .                   XYZMIN,XYZDEL,RMAX,RSQ,A, Q,QX,QY,
     .                   QZ,IER)
            IF (IER .NE. 0) GO TO 7
            IF (ABS(Q1-Q) .GT. 3.*ABS(Q)*EPS) GO TO 8
            EQ = AMAX1(EQ,ABS(FQ(PX,PY,PZ)-Q))
            EQX = AMAX1(EQX,ABS(FX(PX,PY,PZ)-QX))
            EQY = AMAX1(EQY,ABS(FY(PX,PY,PZ)-QY))
    5            EQZ = AMAX1(EQZ,ABS(FZ(PX,PY,PZ)-QZ))
C
C PRINT ERRORS AND THE RATIO EQ/EPS.
C
      RQ = EQ/EPS
      WRITE (LOUT,100)
      WRITE (LOUT,110) EQ, RQ
      WRITE (LOUT,120) EQX
      WRITE (LOUT,130) EQY
      WRITE (LOUT,140) EQZ
      STOP
  100 FORMAT (///1H ,31HMAXIMUM ABSOLUTE ERRORS IN THE ,
     .              25HINTERPOLANT Q AND PARTIAL/
     .              1H ,32HDERIVATIVES (QX,QY,QZ) RELATIVE ,
     .              24HTO MACHINE PRECISION EPS//
     .              1H ,10X,8HFUNCTION,3X,9HMAX ERROR,3X,
     .              13HMAX ERROR/EPS/)
  110 FORMAT (1H ,13X,1HQ,7X,E9.3,7X,F4.2)
  120 FORMAT (1H ,13X,2HQX,6X,E9.3)
  130 FORMAT (1H ,13X,2HQY,6X,E9.3)
  140 FORMAT (1H ,13X,2HQZ,6X,E9.3)
C
C ERROR IN QSHEP3
C
    6 WRITE (LOUT,200) IER
      STOP
  200 FORMAT (///1H ,28H*** ERROR IN QSHEP3 -- IER =,I2,
     .              4H ***)
C
C ERROR IN QS3GRD
C
    7 WRITE (LOUT,210) IER
      STOP
  210 FORMAT (///1H ,28H*** ERROR IN QS3GRD -- IER =,I2,
     .              4H ***)
C
C VALUES RETURNED BY QS3VAL AND QS3GRD DIFFER BY A RELATIVE
C   AMOUNT GREATER THAN 3*EPS.
C
    8 WRITE (LOUT,220) Q1, Q
      STOP
  220 FORMAT (///1H ,33H*** ERROR -- INTERPOLATED VALUES ,
     .              37HQ1 (QS3VAL) AND Q2 (QS3GRD) DIFFER --//
     .              1H ,5X,5HQ1 = ,E21.14,5X,5HQ2 = ,E21.14)
      END
