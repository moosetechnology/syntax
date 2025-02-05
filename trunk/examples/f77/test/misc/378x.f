c  Source: https://github.com/johannesgerer/jburkardt-f77
c  File name: toms493/toms493.f
c
      SUBROUTINE RPOLY ( OP, DEGREE, ZEROR, ZEROI, FAIL )

c*********************************************************************72
C
CC RPOLY finds the zeros of a real polynomial.
c
C OP  - DOUBLE PRECISION VECTOR OF COEFFICIENTS IN
C       ORDER OF DECREASING POWERS.
C
C DEGREE   - INTEGER DEGREE OF POLYNOMIAL.q
C
C ZEROR, ZEROI - OUTPUT DOUBLE PRECISION VECTORS OF
C                REAL AND IMAGINARY PARTS OF THE
C                ZEROS.
C
C FAIL  - OUTPUT LOGICAL PARAMETER, TRUE ONLY IF
C         LEADING COEFFICIENT IS ZERO OR IF RPOLY
C         HAS FOUND FEWER THAN DEGREE ZEROS.
C         IN THE LATTER CASE DEGREE IS RESET TO
C         THE NUMBER OF ZEROS FOUND.
C
C TO CHANGE THE SIZE OF POLYNOMIALS WHICH CAN BE
C SOLVED, RESET THE DIMENSIONS OF THE ARRAYS IN THE
C COMMON AREA AND IN THE FOLLOWING DECLARATIONS.
C THE SUBROUTINE USES SINGLE PRECISION CALCULATIONS
C FOR SCALING, BOUNDS AND ERROR CALCULATIONS. ALL
C CALCULATIONS FOR THE ITERATIONS ARE DONE IN DOUBLE
C PRECISION.
C
      COMMON /GLOBAL/ P, QP, K, QK, SVK, SR, SI, U,
     * V, A, B, C, D, A1, A2, A3, A6, A7, E, F, G,
     * H, SZR, SZI, LZR, LZI, ETA, ARE, MRE, N, NN

      real r4_epsilon
      real r4_huge
      DOUBLE PRECISION P(101), QP(101), K(101),
     * QK(101), SVK(101), SR, SI, U, V, A, B, C, D,
     * A1, A2, A3, A6, A7, E, F, G, H, SZR, SZI,
     * LZR, LZI
      REAL ETA, ARE, MRE
      INTEGER N, NN
      DOUBLE PRECISION OP(101), TEMP(101),
     * ZEROR(100), ZEROI(100), T, AA, BB, CC, DABS,
     * FACTOR
      REAL PT(101), LO, MAX, MIN, XX, YY, COSR,
     * SINR, XXX, X, SC, BND, XM, FF, DF, DX, INFIN,
     * SMALNO, BASE
      INTEGER DEGREE, CNT, NZ, I, J, JJ, NM1
      LOGICAL FAIL, ZEROK
C
C THE FOLLOWING STATEMENTS SET MACHINE CONSTANTS USED
C IN VARIOUS PARTS OF THE PROGRAM. THE MEANING OF THE
C FOUR CONSTANTS ARE...
C ETA     THE MAXIMUM RELATIVE REPRESENTATION ERROR
C         WHICH CAN BE DESCRIBED AS THE SMALLEST
C         POSITIVE FLOATING POINT NUMBER SUCH THAT
C         1.D0+ETA IS GREATER THAN 1.
C INFINY  THE LARGEST FLOATING-POINT NUMBER.
C SMALNO  THE SMALLEST POSITIVE FLOATING-POINT NUMBER
C         IF THE EXPONENT RANGE DIFFERS IN SINGLE AND
C         DOUBLE PRECISION THEN SMALNO AND INFIN
C         SHOULD INDICATE THE SMALLER RANGE.
C BASE    THE BASE OF THE FLOATING-POINT NUMBER
C         SYSTEM USED.
C
      BASE = 2.0
      ETA = r4_epsilon ( )
      INFIN = r4_huge ( )
      SMALNO = 1.0E-37
C
C ARE AND MRE REFER TO THE UNIT ERROR IN + AND *
C RESPECTIVELY. THEY ARE ASSUMED TO BE THE SAME AS
C ETA.
c
      ARE = ETA
      MRE = ETA
      LO = SMALNO/ETA
c
C INITIALIZATION OF CONSTANTS FOR SHIFT ROTATION
c
      XX = .70710678
      YY = -XX
      COSR = -.069756474
      SINR = .99756405
      FAIL = .FALSE.
      N = DEGREE
      NN = N + 1
c
C ALGORITHM FAILS IF THE LEADING COEFFICIENT IS ZERO.
c
      IF ( OP(1) .EQ. 0.0D0) then
        FAIL = .TRUE.
        DEGREE = 0
        RETURN
      end if
c
C REMOVE THE ZEROS AT THE ORIGIN IF ANY
c
10    continue

      IF ( OP(NN) .eq. 0.0D0 ) then
        J = DEGREE - N + 1
        ZEROR(J) = 0.D0
        ZEROI(J) = 0.D0
        NN = NN - 1
        N = N - 1
        GO TO 10
      end if
c
C MAKE A COPY OF THE COEFFICIENTS
c
      DO I=1,NN
        P(I) = OP(I)
      end do
c
C START THE ALGORITHM FOR ONE ZERO
c
   40 continue

      IF ( N .LT. 1 ) then
        RETURN
      else IF ( N.eq.1) then
        ZEROR(DEGREE) = -P(2)/P(1)
        ZEROI(DEGREE) = 0.0D0
        return
      else if ( n .eq. 2 ) then
        CALL QUAD(P(1), P(2), P(3), ZEROR(DEGREE-1),
     *  ZEROI(DEGREE-1), ZEROR(DEGREE), ZEROI(DEGREE))
        return
      end if
c
C FIND LARGEST AND SMALLEST MODULI OF COEFFICIENTS.
c
      MAX = 0.0
      MIN = INFIN
      DO I=1,NN
        X = ABS(SNGL(P(I)))
        IF (X.GT.MAX) MAX = X
        IF (X.NE.0. .AND. X.LT.MIN) MIN = X
      end do
c
C SCALE IF THERE ARE LARGE OR VERY SMALL COEFFICIENTS
C COMPUTES A SCALE FACTOR TO MULTIPLY THE
C COEFFICIENTS OF THE POLYNOMIAL. THE SCALING IS DONE
C TO AVOID OVERFLOW AND TO AVOID UNDETECTED UNDERFLOW
C INTERFERING WITH THE CONVERGENCE CRITERION.
C THE FACTOR IS A POWER OF THE BASE
c
      SC = LO / MIN
      IF (SC.GT.1.0) GO TO 80
      IF (MAX.LT.10.) GO TO 110

      IF (SC.EQ.0.) then
        SC = SMALNO
      end if

      GO TO 90
   80 IF (INFIN/SC.LT.MAX) GO TO 110
   90 L = ALOG(SC)/ALOG(BASE) + 0.5
      FACTOR = (BASE*1.0D0)**L

      IF (FACTOR.ne.1.D0) then
        DO I=1,NN
          P(I) = FACTOR*P(I)
        end do
      end if
c
C COMPUTE LOWER BOUND ON MODULI OF ZEROS.
c
110   continue

      DO I=1,NN
        PT(I) = ABS(SNGL(P(I)))
      end do
      PT(NN) = -PT(NN)
c
C COMPUTE UPPER ESTIMATE OF BOUND
c
      X = EXP((ALOG(-PT(NN))-ALOG(PT(1)))/ dble ( N ))
      IF (PT(N).EQ.0.) GO TO 130
c
C IF NEWTON STEP AT THE ORIGIN IS BETTER, USE IT.
c
      XM = -PT(NN)/PT(N)
      IF (XM.LT.X) X = XM
c
C CHOP THE INTERVAL (0,X) UNTIL FF .LE. 0
c
  130 CONTINUE

      XM = X * 0.1
      FF = PT(1)
      DO I=2,NN
        FF = FF*XM + PT(I)
      end do

      IF (FF.LE.0.) GO TO 150
      X = XM
      GO TO 130
  150 DX = X
c
C DO NEWTON ITERATION UNTIL X CONVERGES TO TWO
C DECIMAL PLACES
c
  160 IF (ABS(DX/X).LE..005) GO TO 180
      FF = PT(1)
      DF = FF
      DO I=2,N
        FF = FF*X + PT(I)
        DF = DF*X + FF
      end do
      FF = FF*X + PT(NN)
      DX = FF/DF
      X = X - DX
      GO TO 160
  180 BND = X
c
C COMPUTE THE DERIVATIVE AS THE INITIAL K POLYNOMIAL
C AND DO 5 STEPS WITH NO SHIFT
c
      NM1 = N - 1

      DO I=2,N
        K(I) = dble ( NN - I ) * P(I) / dble ( N )
      end do

      K(1) = P(1)
      AA = P(NN)
      BB = P(N)
      ZEROK = K(N).EQ.0.D0

      DO JJ=1,5

        CC = K(N)
c
C USE SCALED FORM OF RECURRENCE IF VALUE OF K AT 0 IS NONZERO
c
        IF ( .not. ZEROK) then

          T = -AA/CC
          DO I=1,NM1
            J = NN - I
            K(J) = T*K(J-1) + P(J)
          end do

          K(1) = P(1)
          ZEROK = DABS(K(N)).LE.DABS(BB)*ETA*10.

c
C USE UNSCALED FORM OF RECURRENCE
c
        else

          DO I=1,NM1
            J = NN - I
            K(J) = K(J-1)
          end do

          K(1) = 0.D0
          ZEROK = K(N).EQ.0.D0

        end if

      end do
c
C SAVE K FOR RESTARTS WITH NEW SHIFTS
c
      DO I=1,N
        TEMP(I) = K(I)
      end do
c
C LOOP TO SELECT THE QUADRATIC CORRESPONDING TO EACH NEW SHIFT
c
      DO 280 CNT=1,20
c
C QUADRATIC CORRESPONDS TO A DOUBLE SHIFT TO A
C NON-REAL POINT AND ITS COMPLEX CONJUGATE. THE POINT
C HAS MODULUS BND AND AMPLITUDE ROTATED BY 94 DEGREES
C FROM THE PREVIOUS SHIFT
c
        XXX = COSR*XX - SINR*YY
        YY = SINR*XX + COSR*YY
        XX = XXX
        SR = BND*XX
        SI = BND*YY
        U = -2.0D0*SR
        V = BND
c
C SECOND STAGE CALCULATION, FIXED QUADRATIC
c
        CALL FXSHFR(20*CNT, NZ)
        IF (NZ.EQ.0) GO TO 260
c
C THE SECOND STAGE JUMPS DIRECTLY TO ONE OF THE THIRD
C STAGE ITERATIONS AND RETURNS HERE IF SUCCESSFUL.
C DEFLATE THE POLYNOMIAL, STORE THE ZERO OR ZEROS AND
C RETURN TO THE MAIN ALGORITHM.
c
        J = DEGREE - N + 1
        ZEROR(J) = SZR
        ZEROI(J) = SZI
        NN = NN - NZ
        N = NN - 1
        DO I=1,NN
          P(I) = QP(I)
        end do

        IF (NZ.EQ.1) GO TO 40
        ZEROR(J+1) = LZR
        ZEROI(J+1) = LZI
        GO TO 40
c
C IF THE ITERATION IS UNSUCCESSFUL ANOTHER QUADRATIC
C IS CHOSEN AFTER RESTORING K
c
  260   DO I=1,N
          K(I) = TEMP(I)
        end do

  280 CONTINUE
c
C RETURN WITH FAILURE IF NO CONVERGENCE WITH 20
C SHIFTS
c
      FAIL = .TRUE.
      DEGREE = DEGREE - N

      RETURN
      END
      SUBROUTINE FXSHFR(L2, NZ)

c*********************************************************************72
c
cc FXSHFR computes up to L2 fixed shift K-polynomials.
c
C   In the linear or quadratic cases, a convergence test is made.
c
c   INITIATES ONE OF THE VARIABLE SHIFT
C ITERATIONS AND RETURNS WITH THE NUMBER OF ZEROS
C FOUND.
C L2 - LIMIT OF FIXED SHIFT STEPS
C NZ - NUMBER OF ZEROS FOUND
C
      COMMON /GLOBAL/ P, QP, K, QK, SVK, SR, SI, U,
     * V, A, B, C, D, A1, A2, A3, A6, A7, E, F, G,
     * H, SZR, SZI, LZR, LZI, ETA, ARE, MRE, N, NN
      DOUBLE PRECISION P(101), QP(101), K(101),
     * QK(101), SVK(101), SR, SI, U, V, A, B, C, D,
     * A1, A2, A3, A6, A7, E, F, G, H, SZR, SZI,
     * LZR, LZI
      REAL ETA, ARE, MRE
      INTEGER N, NN
      DOUBLE PRECISION SVU, SVV, UI, VI, S
      REAL BETAS, BETAV, OSS, OVV, SS, VV, TS, TV,
     * OTS, OTV, TVV, TSS
      INTEGER L2, NZ, TYPE, I, J, IFLAG
      LOGICAL VPASS, SPASS, VTRY, STRY
      NZ = 0
      BETAV = .25
      BETAS = .25
      OSS = SR
      OVV = V
C EVALUATE POLYNOMIAL BY SYNTHETIC DIVISION
      CALL QUADSD(NN, U, V, P, QP, A, B)
      CALL CALCSC(TYPE)
      DO 80 J=1,L2
C CALCULATE NEXT K POLYNOMIAL AND ESTIMATE V
        CALL NEXTK(TYPE)
        CALL CALCSC(TYPE)
        CALL NEWEST(TYPE, UI, VI)
        VV = VI
C ESTIMATE S
        SS = 0.
        IF (K(N).NE.0.D0) SS = -P(NN)/K(N)
        TV = 1.
        TS = 1.
        IF (J.EQ.1 .OR. TYPE.EQ.3) GO TO 70
C COMPUTE RELATIVE MEASURES OF CONVERGENCE OF S AND V
C SEQUENCES
        IF (VV.NE.0.) TV = ABS((VV-OVV)/VV)
        IF (SS.NE.0.) TS = ABS((SS-OSS)/SS)
C IF DECREASING, MULTIPLY TWO MOST RECENT
C CONVERGENCE MEASURES
        TVV = 1.
        IF (TV.LT.OTV) TVV = TV*OTV
        TSS = 1.
        IF (TS.LT.OTS) TSS = TS*OTS
C COMPARE WITH CONVERGENCE CRITERIA
        VPASS = TVV.LT.BETAV
        SPASS = TSS.LT.BETAS
        IF (.NOT.(SPASS .OR. VPASS)) GO TO 70
C AT LEAST ONE SEQUENCE HAS PASSED THE CONVERGENCE
C TEST. STORE VARIABLES BEFORE ITERATING
        SVU = U
        SVV = V
        DO 10 I=1,N
          SVK(I) = K(I)
   10   CONTINUE
        S = SS
C CHOOSE ITERATION ACCORDING TO THE FASTEST
C CONVERGING SEQUENCE
        VTRY = .FALSE.
        STRY = .FALSE.
        IF (SPASS .AND. ((.NOT.VPASS) .OR.
     *   TSS.LT.TVV)) GO TO 40
   20   CALL QUADIT(UI, VI, NZ)
        IF (NZ.GT.0) RETURN
C QUADRATIC ITERATION HAS FAILED. FLAG THAT IT HAS
C BEEN TRIED AND DECREASE THE CONVERGENCE CRITERION.
        VTRY = .TRUE.
        BETAV = BETAV*.25
C TRY LINEAR ITERATION IF IT HAS NOT BEEN TRIED AND
C THE S SEQUENCE IS CONVERGING
        IF (STRY .OR. (.NOT.SPASS)) GO TO 50
        DO 30 I=1,N
          K(I) = SVK(I)
   30   CONTINUE
   40   CALL REALIT(S, NZ, IFLAG)
        IF (NZ.GT.0) RETURN
C LINEAR ITERATION HAS FAILED. FLAG THAT IT HAS BEEN
C TRIED AND DECREASE THE CONVERGENCE CRITERION
        STRY = .TRUE.
        BETAS = BETAS*.25
        IF (IFLAG.EQ.0) GO TO 50
C IF LINEAR ITERATION SIGNALS AN ALMOST DOUBLE REAL
C ZERO ATTEMPT QUADRATIC INTERATION
        UI = -(S+S)
        VI = S*S
        GO TO 20
C RESTORE VARIABLES
   50   U = SVU
        V = SVV
        DO 60 I=1,N
          K(I) = SVK(I)
   60   CONTINUE
C TRY QUADRATIC ITERATION IF IT HAS NOT BEEN TRIED
C AND THE V SEQUENCE IS CONVERGING
        IF (VPASS .AND. (.NOT.VTRY)) GO TO 20
C RECOMPUTE QP AND SCALAR VALUES TO CONTINUE THE
C SECOND STAGE
        CALL QUADSD(NN, U, V, P, QP, A, B)
        CALL CALCSC(TYPE)
   70   OVV = VV
        OSS = SS
        OTV = TV
        OTS = TS
   80 CONTINUE
      RETURN
      END
      SUBROUTINE QUADIT(UU, VV, NZ)

c*********************************************************************72
c
C VARIABLE-SHIFT K-POLYNOMIAL ITERATION FOR A
C QUADRATIC FACTOR CONVERGES ONLY IF THE ZEROS ARE
C EQUIMODULAR OR NEARLY SO.
C UU,VV - COEFFICIENTS OF STARTING QUADRATIC
C NZ - NUMBER OF ZERO FOUND
      COMMON /GLOBAL/ P, QP, K, QK, SVK, SR, SI, U,
     * V, A, B, C, D, A1, A2, A3, A6, A7, E, F, G,
     * H, SZR, SZI, LZR, LZI, ETA, ARE, MRE, N, NN
      DOUBLE PRECISION P(101), QP(101), K(101),
     * QK(101), SVK(101), SR, SI, U, V, A, B, C, D,
     * A1, A2, A3, A6, A7, E, F, G, H, SZR, SZI,
     * LZR, LZI
      REAL ETA, ARE, MRE
      INTEGER N, NN
      DOUBLE PRECISION UI, VI, UU, VV, DABS
      REAL MS, MP, OMP, EE, RELSTP, T, ZM
      INTEGER NZ, TYPE, I, J
      LOGICAL TRIED
      NZ = 0
      TRIED = .FALSE.
      U = UU
      V = VV
      J = 0
C MAIN LOOP
   10 CALL QUAD(1.D0, U, V, SZR, SZI, LZR, LZI)
C RETURN IF ROOTS OF THE QUADRATIC ARE REAL AND NOT
C CLOSE TO MULTIPLE OR NEARLY EQUAL AND  OF OPPOSITE
C SIGN
      IF (DABS(DABS(SZR)-DABS(LZR)).GT..01D0*
     * DABS(LZR)) RETURN
C EVALUATE POLYNOMIAL BY QUADRATIC SYNTHETIC DIVISION
      CALL QUADSD(NN, U, V, P, QP, A, B)
      MP = DABS(A-SZR*B) + DABS(SZI*B)
C COMPUTE A RIGOROUS  BOUND ON THE ROUNDING ERROR IN
C EVALUTING P
      ZM = SQRT(ABS(SNGL(V)))
      EE = 2.*ABS(SNGL(QP(1)))
      T = -SZR*B
      DO 20 I=2,N
        EE = EE*ZM + ABS(SNGL(QP(I)))
   20 CONTINUE
      EE = EE*ZM + ABS(SNGL(A)+T)
      EE = (5.*MRE+4.*ARE)*EE - (5.*MRE+2.*ARE)*
     * (ABS(SNGL(A)+T)+ABS(SNGL(B))*ZM) +
     * 2.*ARE*ABS(T)
C ITERATION HAS CONVERGED SUFFICIENTLY IF THE
C POLYNOMIAL VALUE IS LESS THAN 20 TIMES THIS BOUND
      IF (MP.GT.20.*EE) GO TO 30
      NZ = 2
      RETURN
   30 J = J + 1
C STOP ITERATION AFTER 20 STEPS
      IF (J.GT.20) RETURN
      IF (J.LT.2) GO TO 50
      IF (RELSTP.GT..01 .OR. MP.LT.OMP .OR. TRIED)
     * GO TO 50
C A CLUSTER APPEARS TO BE STALLING THE CONVERGENCE.
C FIVE FIXED SHIFT STEPS ARE TAKEN WITH A U,V CLOSE
C TO THE CLUSTER
      IF (RELSTP.LT.ETA) RELSTP = ETA
      RELSTP = SQRT(RELSTP)
      U = U - U*RELSTP
      V = V + V*RELSTP
      CALL QUADSD(NN, U, V, P, QP, A, B)
      DO 40 I=1,5
        CALL CALCSC(TYPE)
        CALL NEXTK(TYPE)
   40 CONTINUE
      TRIED = .TRUE.
      J = 0
   50 OMP = MP
C CALCULATE NEXT K POLYNOMIAL AND NEW U AND V
      CALL CALCSC(TYPE)
      CALL NEXTK(TYPE)
      CALL CALCSC(TYPE)
      CALL NEWEST(TYPE, UI, VI)
C IF VI IS ZERO THE ITERATION IS NOT CONVERGING
      IF (VI.EQ.0.D0) RETURN
      RELSTP = DABS((VI-V)/VI)
      U = UI
      V = VI
      GO TO 10
      END
      SUBROUTINE REALIT(SSS, NZ, IFLAG)

c*********************************************************************72
c
C VARIABLE-SHIFT H POLYNOMIAL ITERATION FOR A REAL ZERO.
C
C SSS   - STARTING ITERATE
C NZ    - NUMBER OF ZERO FOUND
C IFLAG - FLAG TO INDICATE A PAIR OF ZEROS NEAR REAL
C         AXIS.
      COMMON /GLOBAL/ P, QP, K, QK, SVK, SR, SI, U,
     * V, A, B, C, D, A1, A2, A3, A6, A7, E, F, G,
     * H, SZR, SZI, LZR, LZI, ETA, ARE, MRE, N, NN
      DOUBLE PRECISION P(101), QP(101), K(101),
     * QK(101), SVK(101), SR, SI, U, V, A, B, C, D,
     * A1, A2, A3, A6, A7, E, F, G, H, SZR, SZI,
     * LZR, LZI
      REAL ETA, ARE, MRE
      INTEGER N, NN
      DOUBLE PRECISION PV, KV, T, S, SSS, DABS
      REAL MS, MP, OMP, EE
      INTEGER NZ, IFLAG, I, J, NM1
      NM1 = N - 1
      NZ = 0
      S = SSS
      IFLAG = 0
      J = 0
C MAIN LOOP
   10 PV = P(1)
C EVALUATE P AT S
      QP(1) = PV
      DO 20 I=2,NN
        PV = PV*S + P(I)
        QP(I) = PV
   20 CONTINUE
      MP = DABS(PV)
C COMPUTE A RIGOROUS BOUND ON THE ERROR IN EVALUATING
C P
      MS = DABS(S)
      EE = (MRE/(ARE+MRE))*ABS(SNGL(QP(1)))
      DO 30 I=2,NN
        EE = EE*MS + ABS(SNGL(QP(I)))
   30 CONTINUE
C ITERATION HAS CONVERGED SUFFICIENTLY IF THE
C POLYNOMIAL VALUE IS LESS THAN 20 TIMES THIS BOUND
      IF (MP.GT.20.*((ARE+MRE)*EE-MRE*MP)) GO TO 40
      NZ = 1
      SZR = S
      SZI = 0.D0
      RETURN
   40 J = J + 1
C STOP ITERATION AFTER 10 STEPS
      IF (J.GT.10) RETURN
      IF (J.LT.2) GO TO 50
      IF (DABS(T).GT..001*DABS(S-T) .OR. MP.LE.OMP)
     * GO TO 50
C A CLUSTER OF ZEROS NEAR THE REAL AXIS HAS BEEN
C ENCOUNTERED RETURN WITH IFLAG SET TO INITIATE A
C QUADRATIC ITERATION
      IFLAG = 1
      SSS = S
      RETURN
C RETURN IF THE POLYNOMIAL VALUE HAS INCREASED
C SIGNIFICANTLY
   50 OMP = MP
C COMPUTE T, THE NEXT POLYNOMIAL, AND THE NEW ITERATE
      KV = K(1)
      QK(1) = KV
      DO 60 I=2,N
        KV = KV*S + K(I)
        QK(I) = KV
   60 CONTINUE
      IF (DABS(KV).LE.DABS(K(N))*10.*ETA) GO TO 80
C USE THE SCALED FORM OF THE RECURRENCE IF THE VALUE
C OF K AT S IS NONZERO
      T = -PV/KV
      K(1) = QP(1)
      DO 70 I=2,N
        K(I) = T*QK(I-1) + QP(I)
   70 CONTINUE
      GO TO 100
C USE UNSCALED FORM
   80 K(1) = 0.0D0
      DO 90 I=2,N
        K(I) = QK(I-1)
   90 CONTINUE
  100 KV = K(1)
      DO 110 I=2,N
        KV = KV*S + K(I)
  110 CONTINUE
      T = 0.D0
      IF (DABS(KV).GT.DABS(K(N))*10.*ETA) T = -PV/KV
      S = S + T
      GO TO 10
      END
      SUBROUTINE CALCSC(TYPE)

c*********************************************************************72
c
C THIS ROUTINE CALCULATES SCALAR QUANTITIES USED TO
C COMPUTE THE NEXT K POLYNOMIAL AND NEW ESTIMATES OF
C THE QUADRATIC COEFFICIENTS.
C TYPE - INTEGER VARIABLE SET HERE INDICATING HOW THE
C CALCULATIONS ARE NORMALIZED TO AVOID OVERFLOW
      COMMON /GLOBAL/ P, QP, K, QK, SVK, SR, SI, U,
     * V, A, B, C, D, A1, A2, A3, A6, A7, E, F, G,
     * H, SZR, SZI, LZR, LZI, ETA, ARE, MRE, N, NN
      DOUBLE PRECISION P(101), QP(101), K(101),
     * QK(101), SVK(101), SR, SI, U, V, A, B, C, D,
     * A1, A2, A3, A6, A7, E, F, G, H, SZR, SZI,
     * LZR, LZI
      REAL ETA, ARE, MRE
      INTEGER N, NN
      DOUBLE PRECISION DABS
      INTEGER TYPE
C SYNTHETIC DIVISION OF K BY THE QUADRATIC 1,U,V
      CALL QUADSD(N, U, V, K, QK, C, D)
      IF (DABS(C).GT.DABS(K(N))*100.*ETA) GO TO 10
      IF (DABS(D).GT.DABS(K(N-1))*100.*ETA) GO TO 10
      TYPE = 3
C TYPE=3 INDICATES THE QUADRATIC IS ALMOST A FACTOR
C OF K
      RETURN
   10 IF (DABS(D).LT.DABS(C)) GO TO 20
      TYPE = 2
C TYPE=2 INDICATES THAT ALL FORMULAS ARE DIVIDED BY D
      E = A/D
      F = C/D
      G = U*B
      H = V*B
      A3 = (A+G)*E + H*(B/D)
      A1 = B*F - A
      A7 = (F+U)*A + H
      RETURN
   20 TYPE = 1
C TYPE=1 INDICATES THAT ALL FORMULAS ARE DIVIDED BY C
      E = A/C
      F = D/C
      G = U*E
      H = V*B
      A3 = A*E + (H/C+G)*B
      A1 = B - A*(D/C)
      A7 = A + G*D + H*F
      RETURN
      END
      SUBROUTINE NEXTK(TYPE)

c*********************************************************************72
c
C COMPUTES THE NEXT K POLYNOMIALS USING SCALARS
C COMPUTED IN CALCSC
      COMMON /GLOBAL/ P, QP, K, QK, SVK, SR, SI, U,
     * V, A, B, C, D, A1, A2, A3, A6, A7, E, F, G,
     * H, SZR, SZI, LZR, LZI, ETA, ARE, MRE, N, NN
      DOUBLE PRECISION P(101), QP(101), K(101),
     * QK(101), SVK(101), SR, SI, U, V, A, B, C, D,
     * A1, A2, A3, A6, A7, E, F, G, H, SZR, SZI,
     * LZR, LZI
      REAL ETA, ARE, MRE
      INTEGER N, NN
      DOUBLE PRECISION TEMP, DABS
      INTEGER TYPE
      IF (TYPE.EQ.3) GO TO 40
      TEMP = A
      IF (TYPE.EQ.1) TEMP = B
      IF (DABS(A1).GT.DABS(TEMP)*ETA*10.) GO TO 20
C IF A1 IS NEARLY ZERO THEN USE A SPECIAL FORM OF THE
C RECURRENCE
      K(1) = 0.D0
      K(2) = -A7*QP(1)
      DO 10 I=3,N
        K(I) = A3*QK(I-2) - A7*QP(I-1)
   10 CONTINUE
      RETURN
C USE SCALED FORM OF THE RECURRENCE
   20 A7 = A7/A1
      A3 = A3/A1
      K(1) = QP(1)
      K(2) = QP(2) - A7*QP(1)
      DO 30 I=3,N
        K(I) = A3*QK(I-2) - A7*QP(I-1) + QP(I)
   30 CONTINUE
      RETURN
C USE UNSCALED FORM OF THE RECURRENCE IF TYPE IS 3
   40 K(1) = 0.D0
      K(2) = 0.D0
      DO 50 I=3,N
        K(I) = QK(I-2)
   50 CONTINUE
      RETURN
      END
      SUBROUTINE NEWEST(TYPE, UU, VV)

c*********************************************************************72
c
C COMPUTE NEW ESTIMATES OF THE QUADRATIC COEFFICIENTS
C USING THE SCALARS COMPUTED IN CALCSC.
      COMMON /GLOBAL/ P, QP, K, QK, SVK, SR, SI, U,
     * V, A, B, C, D, A1, A2, A3, A6, A7, E, F, G,
     * H, SZR, SZI, LZR, LZI, ETA, ARE, MRE, N, NN
      DOUBLE PRECISION P(101), QP(101), K(101),
     * QK(101), SVK(101), SR, SI, U, V, A, B, C, D,
     * A1, A2, A3, A6, A7, E, F, G, H, SZR, SZI,
     * LZR, LZI
      REAL ETA, ARE, MRE
      INTEGER N, NN
      DOUBLE PRECISION A4, A5, B1, B2, C1, C2, C3,
     * C4, TEMP, UU, VV
      INTEGER TYPE
C USE FORMULAS APPROPRIATE TO SETTING OF TYPE.
      IF (TYPE.EQ.3) GO TO 30
      IF (TYPE.EQ.2) GO TO 10
      A4 = A + U*B + H*F
      A5 = C + (U+V*F)*D
      GO TO 20
   10 A4 = (A+G)*F + H
      A5 = (F+U)*C + V*D
C EVALUATE NEW QUADRATIC COEFFICIENTS.
   20 B1 = -K(N)/P(NN)
      B2 = -(K(N-1)+B1*P(N))/P(NN)
      C1 = V*B2*A1
      C2 = B1*A7
      C3 = B1*B1*A3
      C4 = C1 - C2 - C3
      TEMP = A5 + B1*A4 - C4
      IF (TEMP.EQ.0.D0) GO TO 30
      UU = U - (U*(C3+C2)+V*(B1*A1+B2*A7))/TEMP
      VV = V*(1.+C4/TEMP)
      RETURN
C IF TYPE=3 THE QUADRATIC IS ZEROED
   30 UU = 0.D0
      VV = 0.D0
      RETURN
      END
      SUBROUTINE QUADSD(NN, U, V, P, Q, A, B)

c*********************************************************************72
c
C DIVIDES P BY THE QUADRATIC  1,U,V  PLACING THE
C QUOTIENT IN Q AND THE REMAINDER IN A,B
      DOUBLE PRECISION P(NN), Q(NN), U, V, A, B, C
      INTEGER I
      B = P(1)
      Q(1) = B
      A = P(2) - U*B
      Q(2) = A
      DO 10 I=3,NN
        C = P(I) - U*A - V*B
        Q(I) = C
        B = A
        A = C
   10 CONTINUE
      RETURN
      END
      SUBROUTINE QUAD(A, B1, C, SR, SI, LR, LI)

c*********************************************************************72
c
C CALCULATE THE ZEROS OF THE QUADRATIC A*Z**2+B1*Z+C.
C THE QUADRATIC FORMULA, MODIFIED TO AVOID
C OVERFLOW, IS USED TO FIND THE LARGER ZERO IF THE
C ZEROS ARE REAL AND BOTH ZEROS ARE COMPLEX.
C THE SMALLER REAL ZERO IS FOUND DIRECTLY FROM THE
C PRODUCT OF THE ZEROS C/A.
      DOUBLE PRECISION A, B1, C, SR, SI, LR, LI, B,
     * D, E, DABS, DSQRT
      IF (A.NE.0.D0) GO TO 20
      SR = 0.D0
      IF (B1.NE.0.D0) SR = -C/B1
      LR = 0.D0
   10 SI = 0.D0
      LI = 0.D0
      RETURN
   20 IF (C.NE.0.D0) GO TO 30
      SR = 0.D0
      LR = -B1/A
      GO TO 10
C COMPUTE DISCRIMINANT AVOIDING OVERFLOW
   30 B = B1/2.D0
      IF (DABS(B).LT.DABS(C)) GO TO 40
      E = 1.D0 - (A/B)*(C/B)
      D = DSQRT(DABS(E))*DABS(B)
      GO TO 50
   40 E = A
      IF (C.LT.0.D0) E = -A
      E = B*(B/DABS(C)) - E
      D = DSQRT(DABS(E))*DSQRT(DABS(C))
   50 IF (E.LT.0.D0) GO TO 60
C REAL ZEROS
      IF (B.GE.0.D0) D = -D
      LR = (-B+D)/A
      SR = 0.D0
      IF (LR.NE.0.D0) SR = (C/LR)/A
      GO TO 10
C COMPLEX CONJUGATE ZEROS
   60 SR = -B/A
      LR = SR
      SI = DABS(D/A)
      LI = -SI
      RETURN
      END
      function r4_huge ( )

c*********************************************************************72
c
cc R4_HUGE returns a "huge" R4.
c
c  Modified:
c
c    13 April 2004
c
c  Author:
c
c    John Burkardt
c
c  Parameters:
c
c    Output, real R4_HUGE, a huge number.
c
      implicit none

      real r4_huge

      r4_huge = 1.0E+30

      return
      end
      function r4_epsilon ( )

c*********************************************************************72
c
cc R4_EPSILON returns the R4 roundoff unit.
c
c  Discussion:
c
c    The roundoff unit is a number R which is a power of 2 with the
c    property that, to the precision of the computer's arithmetic,
c      1 .lt. 1 + R
c    but
c      1 = ( 1 + R / 2 )
c
c    FORTRAN90 provides the superior library routine
c
c      EPSILON ( X )
c
c  Modified:
c
c    06 March 2006
c
c  Author:
c
c    John Burkardt
c
c  Parameters:
c
c    Output, real R4_EPSILON, the R4 roundoff unit.
c
      implicit none

      real r4
      real r4_epsilon
      real r4_test

      r4 = 1.0E+00
      r4_test = 1.0E+00 + ( r4 / 2.0E+00 )

10    continue

      if ( 1.0E+00 .lt. r4_test ) then
        r4 = r4 / 2.0E+00
        r4_test = 1.0E+00 + ( r4 / 2.0E+00 )
        go to 10
      end if

      r4_epsilon = r4

      return
      end
