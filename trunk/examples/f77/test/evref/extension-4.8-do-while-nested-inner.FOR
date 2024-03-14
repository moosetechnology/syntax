       INTEGER A(4,4), C, R 

       C = 4 
       R = 1 

       DO WHILE ( C .GT. R ) 
         A(C,R) = 1 
         C = C - 1 

         DO 10 WHILE ( C .NE. R ) 
           A(C,R) = A(C,R) + 1 
           C = C+1 
10       CONTINUE

       END DO
       END