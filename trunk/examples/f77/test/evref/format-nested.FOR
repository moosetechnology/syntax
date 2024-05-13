C     Define a complex format with nested formats in parentheses
      WRITE(*,100) I, X, Y, STR
100   FORMAT(I3, 3X, (2F10.3), A)

      END