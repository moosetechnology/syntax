c  Source: https://github.com/johannesgerer/jburkardt-f77
c  File name: clean77/file4.f
c
      SUBROUTINE DOBAD (X,Y)
      COMMON /XYZ/ A(10)
C
      DO 10 I = 1, 10
         IF (A(I).GT.0.0) RETURN
         A(I) = I
   10 X = A(I)
C
      DO 20 I = 1, 10
         IF (A(I).GT.10.0) GO TO 20
         A(I) = 2*I
   20 Y = A(I)
C
      RETURN
C
      END
