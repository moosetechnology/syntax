      SUBROUTINE INVER (A,M,KK,ISIG,JK)
cm
      IMPLICIT NONE
      REAL*8 C, X, Z, zz, HOLD
      INTEGER M, KK, ISIG, Jk,N, NN, N2, J, I, NJCOL, L, LCOL, KLL, J1
      INTEGER KIL, KINJ, KLJ, KJIJ, KIJ, KKK, KKI, L1, JCOL, L2
      REAL*8 A(JK)
C
      ISIG = 1
      N = M
      NN = KK
      N2 = N + N
C
      DO 20 J=1,N
C
      NJCOL = (N + J - 1) * NN

      DO 20 I=1,N
C
      KINJ = NJCOL + I
      IF(I.EQ.J) GO TO 10
      A(KINJ) = 0.
      GO TO 20
   10 A(KINJ) = 1.
   20 CONTINUE
C
      L = 0
   30 L = L + 1
      LCOL = NN*L-NN
      KLL = LCOL + L
      IF(L - N) 40,70,1000
C
   40 J1 = L
      C=  ABS( A(KLL) )
      L1 = L + 1
C
      DO 50 I = L1,N
C
      KIL = LCOL + I
      X=  ABS( A(KIL) )
      IF (C.GE.X) GO TO 50
C
      J1 = I
C
      C  = X
   50 CONTINUE
C
      IF(J1.EQ.L) GO TO 70
C
      DO 60 J = L,N2
C
      JCOL = NN*J-NN
      KJIJ = JCOL + J1
      HOLD = A(KJIJ)
      KLJ = JCOL + L
      A(KJIJ) = A(KLJ)
      A(KLJ) = HOLD
   60 CONTINUE
C
   70 IF( ABS(A(KLL)) .GT. 0.00000001) GO TO 80
C
      ISIG = 4
      GO TO 1000
C
   80 L1 = 1
      L2 = L - 1
      IF(L2.GT.0) GO TO 100
   90 CONTINUE
      IF(L.EQ.N) GO TO 100
      L1 = L + 1
      L2 = N
C
  100 CONTINUE
C
      IF(N.LE.L2) GO TO 30
      GO TO 90
C
 1000 RETURN
C
       END
