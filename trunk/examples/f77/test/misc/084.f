c  Source: https://github.com/johannesgerer/jburkardt-f77
c  File name: toms450/toms450.f
c
      SUBROUTINE ROMIN(N, X, FUNCT, STEP, MONITR )

c*********************************************************************72
c
C  THIS SUBROUTINE MINIMIZES A FUNCTION OF N VARIABLES
C  USING THE METHOD OF ROSENBROCK.  THE PARAMETERS ARE
C  DESCRIBED AS FOLLOWS:
C    N IS THE NUMBER OF INDEPENDENT VARIABLES.
C    X(N) IS AN ESTIMATE OF THE SOLUTION ( ON ENTRY -
C      AN INITIAL ESTIMATE, ON EXIT - THE BEST ESTIMATE
C      OF THE SOLUTION FOUND.)
C    FUNCT(N,X,F) IS A ROUTINE PROVIDED BY THE USER TO
C      CALCULATE THE VALUE F OF THE MINIMIZED FUNCTION
C      AT ANY POINT X.
C    STEP IS AN INITIAL STEP LENGTH FOR ALL COORDINATE
C      DIRECTIONS AT THE START OF THE PROCESS.
C    MONITR(N,X,F,R,S,CON,NR) IS A ROUTINE PROVIDED BY
C      THE USER FOR DIAGNOSTIC AND CONVERGENCE PURPOSES.
C    R IS THE ACTUAL NUMBER OF FUNCTION EVALUATIONS (FOR THE
C      INITIAL ESTIMATE R = 0.)
C    B IS THE VALUE OF THE EUCLIDEAN NORM OF THE VECTOR
C      REPRESENTING THE TOTAL PROGRESS MADE SINCE THE
C      AXES WERE LAST ROTATED.
C    CON IS A LOGICAL VARIABLE.  AT THE START OF THE
C      SUBROUTINE ROMIN CON=.FALSE.  IF THE CONVERGENCE
C      CRITERIA OF THE ROUTINE MONITOR ARE SATISFIED
C      CON MUST BE SET TO .TRUE. TO STOP THE PROCESS.
C    NR IS THE MONITOR INDEX.
C  INITIALIZE CON, D(I) AND R.
C  E(I) IS A SET OF STEPS TO BE TAKEN IN THE CORRESPONDING
C  COORDINATE DIRECTIONS.
c
      INTEGER N, IP
      REAL STEP
      DIMENSION X(N)
      LOGICAL CON
      INTEGER I, J, K, L, P, R
      REAL F0, F1, B, BETY
      DIMENSION A(30), D(30), V(30,30), ALPHA(30,30), BETA(30),
     &  E(30), AV(30)

      CON = .FALSE.
      DO 10 I=1,N
        E(I) = STEP
10    CONTINUE
      R = 0
C  V(I,J) IS AN NXN MATRIX DEFINING A SET OF N MUTUALLY
C  ORTHOGONAL COORDINATE DIRECTIONS.  V(I,J) IS THE UNIT
C  MATRIX AT THE START OF THE PROCESS.
      DO 30 I=1,N
        DO 20 J=1,N
          V(I,J) = 0.0
          IF (I.EQ.J) V(I,J) = 1.0
20      CONTINUE
30    CONTINUE
      CALL FUNCT(N, X, F0)
C  START OF THE ITERATION LOOP
40    DO 50 I=1,N
        A(I) = 2.0
        D(I) = 0.0
50    CONTINUE
C  EVALUATE F AT THE NEW POINT X.
60    DO 130 I=1,N
        DO 70 J=1,N
          X(J) = X(J) + E(I)*V(I,J)
70      CONTINUE
        R = R + 1
        CALL FUNCT(N, X, F1 )
        CALL MONITR(N, X, F1, R, 0.0, CON, 1)
        IF ( CON ) GO TO 290
        IF (F1-F0) 80, 90, 90
C  THE NEW VALUE OF THE FUNCTION IS LESS THAN THE OLD ONE.
80      D(I) = D(I) + E(I)
        E(I) = 3.0 * E(I)
        F0 = F1
        IF (A(I).GT.1.5) A(I) = 1.0
        GO TO 110
C  THE NEW VALUE OF THE FUNCTION IS GREATER THAN OR EQUAL
C  TO THE OLD ONE.
90      DO 100 J=1,N
          X(J) = X(J) - E(I)*V(I,J)
100     CONTINUE
        E(I) = -0.5 * E(I)
        IF (A(I).LT.1.5) A(I) = 0.0
110     DO 120 J=1,N
          IF (A(J).GE.0.5) GO TO 130
120     CONTINUE
        GO TO 140
130   CONTINUE
      GO TO 60
C  GRAM-SCHMIDT ORTHOGONALIZATION PROCESS.
140   DO 160 K=1,N
        DO 150 L=1,N
          ALPHA(K,L) = 0.0
150     CONTINUE
160   CONTINUE
      DO 190 I=1,N
        DO 180 J=1,N
          DO 170 L=I,N
            ALPHA(I,J) = ALPHA(I,J) + D(L) * V(L,J)
170       CONTINUE
180     CONTINUE
190   CONTINUE
      B = 0.0
      DO 200 J=1,N
        B = B + ALPHA(1,J)**2
200   CONTINUE
      B = SQRT(B)
C  CALCULATE THE NEW SET OF ORTHONORMAL COORDINATE
C  DIRECTIONS ( THE NEW MATRIX V(I,J) ).
      DO 210 J=1,N
        V(1,J) = ALPHA(1,J) / B
210   CONTINUE
      DO 280 P=2,N
        BETY = 0.0
        IP = P - 1
        DO 220 M=1,N
          BETA(M) = 0.0
220     CONTINUE
        DO 250 J=1,N
          DO 240 K=1,IP
            AV(K) = 0.0
            DO 230 L=1,N
              AV(K) = AV(K) + ALPHA(P,L) * V(K,L)
230         CONTINUE
            BETA(J) = BETA(J) - AV(K) * V(K,J)
240       CONTINUE
250     CONTINUE
        DO 260 J=1,N
          BETA(J) = BETA(J) + ALPHA(P,J)
          BETY = BETY + BETA(J)**2
260     CONTINUE
        BETY = SQRT(BETY)
        DO 270 J=1,N
          V(P,J) = BETA(J) / BETY
270     CONTINUE
280   CONTINUE
C  END OF GRAM-SCHMIDT PROCESS.
      CALL MONITR(N, X, F0, R, B, CON, 2)
      IF ( CON ) GO TO 290
C  GO TO THE NEXT ITERATION
      GO TO 40
290   RETURN
      END
