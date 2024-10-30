C Source: https://sites.esm.psu.edu/~ajm138/fortranexamples.html

      PROGRAM MAIN
      INTEGER N, X
      EXTERNAL SUB1
      COMMON /GLOBALS/ N
      X = 0
      PRINT *, 'Enter number of repeats'
      READ (*,*) N
      CALL SUB1(X,SUB1)
      END

      SUBROUTINE SUB1(X,DUMSUB)
      INTEGER N, X, Y
      EXTERNAL DUMSUB
      COMMON /GLOBALS/ N
      Y = 0
      IF(X .LT. N)THEN
        X = X + 1
        Y = X**2
        PRINT *, 'x = ', X, ', y = ', Y
        CALL DUMSUB(X,DUMSUB)
        PRINT *, 'x = ', X, ', y = ', Y
      END IF
      END
