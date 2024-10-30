c     test provided by Pierre Boullier
c     Exemple page 29
c
c     Ce petit programme permet de lire le fichier 1 et de le recopier
c     sur le fichier 2 en inversant l'ordre des enregistrements.
       SEGMENT, PILE
       POINTEUR PAR.PILE
       IMPLIED A,B(N)
       END SEGMENT
       POINTEUR P1.PILE
C
C     Initialisation de GEMAT
C
       CALL OOOSSG
C
C     Initialisation puis constitution de la pile
C
      PILE = 0
 10   READ (1, END=30) N
      SEGINI, P1
      P1.PAR = PILE
      PILE = P1
      READ (1) A, (B(I), I=1, N)
      SEGDES, PILE
      GO TO 10
c
c     exploitation et suppression de la pile
c
 30   IF (PILE.NE.0) THEN
         SEGACT, PILE
         N = B(/1)
         WRITE (2) N
         WRITE (2) A,(B(I),I=1,N)
         P1 = PAR
         SEGSUP, PILE
         PILE = P1
         GO TO 30
      END IF

      end
