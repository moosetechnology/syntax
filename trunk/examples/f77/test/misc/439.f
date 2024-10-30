c  Source: https://github.com/johannesgerer/jburkardt-f77
c  File name: clean77/file3_new.f
c
      COMMON /XYZ/ A(30,50),B(30,50)
C
      DO 20 I = 1, 30
         DO 20 J = 1, 50
            DO 10 K = 1, 3
               A(I,J) = A(I,J)*FUNCT(K)
   10       CONTINUE
   20 CONTINUE
C
      DO 30 I = 1, 30
         DO 30 J = 1, 50
            IF (B(I,J).LT.0.0) GO TO 30
            B(I,J) = A(I,J)
   30       B(I,J) = B(I,J)*FUNCT2(83.7)
C
C     RETURN
C
      END
