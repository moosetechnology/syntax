      PROGRAM MAIN
       IF ( PRESSURE .GT 1000.0 ) THEN 
              IF ( N .LT. 0.0 ) THEN 
                     X = 0.0 
                     Y = 0.0 
              ELSE 
                     Z = 0.0 
              END IF 
       ELSE IF ( TEMPERATURE .GT. 547.0 ) THEN 
              Z = 1.0 
       ELSE 
              X = 1.0 
              Y = 1.0 
       END IF
      END