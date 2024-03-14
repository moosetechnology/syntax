c        :
      FUNCTION IACCUM1 (IA,N) 
      DIMENSION IA(N) 
C 
      IACCUM1=0 
C 
      IF (N.LE.0) RETURN
      DO 10 I=1,N 
      IACCUM1=IACCUM1+IA(I) 
   10 CONTINUE
C 
      RETURN
      END
C 
      FUNCTION IACCUM2 (IA,IB,N)
      DIMENSION IA(N),IB(N) 
C 
      IACCUM2=0 
C 
      IF (N.LE.0) RETURN
      DO 10 I=1,N 
      IACCUM2=IACCUM2+IA(I)*IB(I) 
   10 CONTINUE
C 
      RETURN
      END
C 
      FUNCTION IACCUM3 (IA,IB,IC,N) 
      DIMENSION IA(N),IB(N),IC(N) 
C 
      IACCUM3=0 
C 
      IF (N.LE.0) RETURN
      DO 10 I=1,N 
      IACCUM3=IACCUM3+IA(I)*IB(I)*IC(I) 
   10 CONTINUE
C 
      RETURN
      END
C
      REAL*8 FUNCTION ACCUM1 (A,N) 
      IMPLICIT REAL*8 (A-H,O-Z)
C
      DIMENSION A(N)
C 
      ACCUM1=0.0
C 
      IF (N.LE.0) RETURN
      DO 10 I=1,N 
      ACCUM1=ACCUM1+A(I)
   10 CONTINUE
C 
      RETURN
      END
C
      REAL*8 FUNCTION ACCUM2 (A,B,N) 
      IMPLICIT REAL*8 (A-H,O-Z)
C
      DIMENSION A(N),B(N) 
C 
      ACCUM2=0.0
C 
      IF (N.LE.0) RETURN
      DO 10 I=1,N 
      ACCUM2=ACCUM2+A(I)*B(I) 
   10 CONTINUE
C 
      RETURN
      END
C
      REAL*8 FUNCTION ACCUM3 (A,B,C,N) 
      IMPLICIT REAL*8 (A-H,O-Z)
C
      DIMENSION A(N),B(N),C(N)
C 
      ACCUM3=0.0
C 
      IF (N.LE.0) RETURN
      DO 10 I=1,N 
      ACCUM3=ACCUM3+A(I)*B(I)*C(I)
   10 CONTINUE
C 
      RETURN
      END
