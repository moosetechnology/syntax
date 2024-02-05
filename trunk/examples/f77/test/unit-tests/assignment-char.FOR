       CHARACTER BELL*1, C2*2, C3*3, C5*5, C6*6 
       REAL Z
       C2 = 'z' 
       C3 = 'uvwxyz' 
       C5 = 'vwxyz' 
       C5(1:2) = 'AB' 
       C6 = C5 // C2 
       BELL = CHAR(7)
       END