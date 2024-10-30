c     test provided by Pierre Boullier
c     Exemple page 21
       SEGMENT, P
       REAL A,B(N1),C(N2,N3),D(N2)
       END SEGMENT

       SEGMENT, SS
       POINTEUR S(N).P
       END SEGMENT

       SEGACT, S(1)
       N1=S(1).B(/1)
       N2=S(1).C(/1)
       N3=S(1).C(/2)
       SEGINI, S(K)
       S(K).A=S(1).A
       DO 10 I=1,N1
 10       S(K).B(I)=S(1).B(I)
       DO 20 I=1,N2
          S(K).D(I)=S(1).D(I)
          DO 20 J=1,N3
 20          S(K).C(I,J)=S(1).C(I,J)
       SEGDES, S(1)
c     La possibilite' de duplication permet d'ecrire :
       SEGINI, S(K)=S(1)

c     En reprenant le meme exemple que celui de la duplication de segment :
c     activer S(K) en reinitialisant les donnees avec S(1), on peut ecrire :
       SEGACT, S(K)=S(1)

       end
      
