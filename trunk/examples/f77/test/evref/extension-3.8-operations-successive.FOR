      REAL A, X, Z, RESULT, RESULTPAR
      X = 2.0
      A = 0.5
      Z = 2.0

      RESULT = (X ** (-A)) * Z
      RESULTPAR = X ** (-(A * Z))
      PRINT *, 'The result of X** -A * Z is:', RESULT
      PRINT *, 'The result of X ** (-(A * Z)) is:', RESULTPAR
      END
