c  Source: https://github.com/johannesgerer/jburkardt-f77
c  File name: clean77/file2_new.f
c
      SUBROUTINE SETC (A,B,C)
C
      IF (A.GT.0.0) THEN
         IF (A.GT.B) THEN
            C = A
         ELSE
            C = B/A
         ENDIF
      ELSE
         C = -A/B
      ENDIF
      RETURN
C
      END
