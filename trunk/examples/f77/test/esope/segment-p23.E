c     test provided by Pierre Boullier
c     Exemple page 23
       SEGMENT, AA
       POINTEUR A(N).PA
       END SEGMENT
       SEGMENT, BB
       POINTEUR B(N).PB
       END SEGMENT
       SEGMENT, XX
       POINTEUR X(M).PX
       END SEGMENT
       SEGMENT, YY
       POINTEUR Y(M).PY
       END SEGMENT
       SEGMENT, ZZ
       POINTEUR Z(M).PZ
       END SEGMENT
       CALL OOOMRU (1)
       DO 20 I=1,N
          SEGACT, A(I)
          SEGINI, B(I)
c     ...........
          SEGDES, A(I)
          DO 10 J=1,M
             SEGACT, X(J), Y(J)
c            ...................
             SEGDES, X(J)*MRU
             IF (I.EQ.1) THEN
                SEGACT, Z(J)
c     ...........
                SEGDES, Z(J)
             END IF
             SEGDES, Y(J)*MRU
 10       CONTINUE
          SEGDES, B(I)
 20    CONTINUE
       CALL OOOMRU (0)

       end
      
