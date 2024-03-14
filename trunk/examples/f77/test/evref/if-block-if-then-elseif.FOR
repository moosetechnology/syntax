      PROGRAM MAIN
       IF ( C .EQ. 'a' ) THEN 
              NA=NA+1 
              CALL APPEND 
       ELSE IF ( C .EQ. 'b' ) THEN 
              NB=NB+1 
              CALL BEFORE 
       END IF
      END