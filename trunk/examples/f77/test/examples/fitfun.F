      SUBROUTINE FITFUN(FONCT,NERR,NCOEF,NVAR,NPOINT,NW,COEF,X,Y,W,
     &  EPS0,EPS1,VARF,VARF1,NIT,INTP,NINV,NCIT)
c
c -------+---------+---------+---------+---------+---------+---------+--
c
cb  subroutine : firfun
cb  auteur     : A. Dall'Osso
cb  service    : NFPSDN
cb  date       : 04/02/03
cb  source     : A. Dall'Osso, CORRELE. Un sous-programme pour l'execution
cb               d'ajustements lineaires et non lineaires avec la methode
cb               des moindres carres. EPN/DM.087 (fevrier 1984).
cb  but        :
cb    Determination des coefficients d'une correlation.
c
c -------+---------+---------+---------+---------+---------+---------+--
c
c Description:
c ------------
cd Voir note EPN/DM.087 citee ci dessus.
c
c Entrees:
c --------
cae FONCT   : Nom de la subroutine externe qui calcule les derivee de la
cae           correlation par rapport aux coefficients. Elle doit etre
cae           declaree en "external" par le programme appelant.
cae NCOEF   : Nombre de coefficients de la correlation.
cae NVAR    : Nombre de variables independentes.
cae NPOINT  : Nombre de points observes (les points sur lequels est effectue
cae           l'ajustement (fit)).
cae NW      : Type de ponderation a effectur sur l'erreur :
cae             0 : Aucune ponderation (tout les points ont le meme poid);
cae             1 : Le poids est defini par le tableau W en entree;
cae             2 : Le poids est defini par 1/Y^2 observe;
cae             3 : Le poids est defini par 1/Y^2 calcule;
cae X(I,J)  : Valeur de la I-eme variable independente correspondant au J-eme
cae           point observe (J=1,...,NPOINT).
cae Y(J)    : Valeur de la variable dependente correspondant au J-eme point
cae           observe.
cae W(J)    : Poid associe au  J-eme point observe (utile si NW=1).
cae EPS0    : Critere de convergence sur le calcul iteratif dans le cas ou la
cae           correlation est non lineaire.
cae EPS1    : Critere d'acceptation de la nouvelle solution en cours d'iteration.
cae           Si la variation depasse EPS1 elle n'est pas retenue et la solution
cae           de l'iteration precedente est extrapolee.
cae NIT     : Nombre maximal d'iterations.
cae INTP    : Option d'impression (incrementale) :
cae             0 : Aucune impression;
cae             1 : Impression des coefficients et de la precision de la
cae                 correlation dans chaque point;
cae             2 : Impression des coefficients en cours d'iteration.
cae NINV    : Option d'impression de la matrice inverse du fit (oui SI NINV>0).
c
c Entrees/Sorties:
c ----------------
cam COEF(I) : Coefficients de la correlation (I=1,...,NCOEF). Ils sont en entree
cam           dans le cas ou la correlation est non lineaire.
c
c Sorties:
c --------
cas NERR    : Compte rendu de retour: NERR=1 => pas d'erreur.
cas VARF    : Variance de la correlation sur le nuage de points observes.
cas VARF1   : Variance ponderee de la correlation sur le nuage de points
cas           observes.
cas NCIT    : Nombre d'iterations executees (dans le cas ou la correlation est 
cas           non lineaire).
c
c Scratch:
c --------
cac Aucun
c
c Dimension:
c ----------
cad Aucun
c
cm  Auteur  : A. Dall'Osso
cm  Date    : 27/01/04
cm  Service : NFPSDN
cm  Source  : Task 1648: Evolutions procedure GenCMA (FADE 1916)
cm  Objet   : Numerotation a gauche du tableau recapitulatif.
c
cg Voir note EPN/DM.087.
cf Voir note EPN/DM.087.
c
      IMPLICIT REAL*8 (A-H,O-Z)
      EXTERNAL FONCT
      DIMENSION COEF(NCOEF),X(NVAR,NPOINT),Y(NPOINT),W(NPOINT)
      REAL*8 EPS0,EPS1,VARF,VARF1
      INTEGER NIT,INTP,NINV,NCIT
C
C     SOUS-PROGRAMME D'AJUSTEMENT LINEAIRE ET NON LINEAIRE
C     PAR LA METHODE DES MOINDRES CARRES
C
#include <matfit.cmn>
C
      DIMENSION TB(10),DERIVE(10)
      EQUIVALENCE (TB,B),(BB,DERIVE)
C
      CHARACTER*1 BLANC,ETOILE
      CHARACTER*8 NONCHG,ICAR,LINTST
      LOGICAL LINEAR
C
      DATA BLANC,ETOILE / ' ','*'  /
      DATA NOUT / 6 /
      DATA NONCHG / 'INCHANGE' /
C
C     LES TABLEAUX SONT DIMENSIONNES DE FACON A PERMETTRE UN
C     NOMBRE MAXIMAL DE VARIABLES (NVAR) ET DE COEFFICIENTS
C     (NCOEF) EGAL A 10.  SI ON LE MODIFIE CHANGER EN CONSEQUENCE
C     LES DIMENSIONS DANS DETER ET DANS L APPEL A INVER
C
C         ON POSE NERR=1 ( PAS D'ERREUR )
C
      NERR=1
C
      IF (NIT  .LE. 0)  NIT  = 50
      IF (EPS0 .LE. 0.) EPS0 = 0.01
      IF (EPS1 .LE. 0.) EPS1 = 0.1
C
C  ON TESTE S'IL S'AGIT D'UN CAS LINEAIRE OU PAS
C
      LINTST=NONCHG
      CALL FONCT(NCOEF,NVAR,X,COEF,  LINTST  ,DERIVE)
      LINEAR=LINTST.EQ.NONCHG
C
      IF(LINEAR .OR. INTP.LT.2) GO TO 10
      WRITE (NOUT,1000) (BLANC,I,I=1,NCOEF)
 1000 FORMAT(1H0,10(A4,5HCOEF(I2,2H) ))
      WRITE(NOUT,1050)
 1050 FORMAT (1X)
C
   10 CONTINUE
C
C     CALCUL DES COEFFICIENTS DU BEST-FIT -----------------
C
      DO 210 N=1,NIT
      NCIT=N
C
      VNUL=0.0
      CALL INID(B,VNUL,10)
      CALL INID(A,VNUL,100)
C
      DO 40 JAY=1,NPOINT
      YC=0.
      CALL FONCT( NCOEF,NVAR,X(1,JAY),COEF,YC,DERIVE )
C  ETANT DONNEE QUE DANS LE CAS LINEAIRE FONCT LAISSE YC=0 L'INSTRUCTION
C  SUIVANTE FAIT ELLE MEME LA DISTINCTION ENTRE LES CAS LINEAIRE ET NON
      F1=Y(JAY)-YC
      WI=1.0
      IF(LINEAR.AND.NW.EQ.3) YC=ACCUM2(COEF,DERIVE,NCOEF)
      IF(NW.GT.0) CALL POIDS ( NW,Y(JAY),YC,W(JAY),WI )
C
      DO 30 L=1,NCOEF
      B(L)=B(L)+DERIVE(L)*F1*WI
      DO 30 M=L,NCOEF
   30 A(L,M)=A(L,M)+DERIVE(L)*DERIVE(M)*WI
   40 CONTINUE
C
C      LA MATRICE A EST SYMETRIQUE DONC ON PEUT BASCULER
C      AUTOUR DE LA DIAGONALE
      DO 50 M=2,NCOEF
      K=M-1
      DO 50 I=1,K
   50 A(M,I)=A(I,M)
C     ON SAUVE LA MATRICE A PAR CE QUE DETER LA DETRUIT
      DO 60 L=1,NCOEF
      DO 60 M=1,NCOEF
   60 C(L,M)= A(L,M)
C     CALCUL DU DETERMINANT
      CALL DETER (A,NCOEF,DET)
C            TEST DE SINGULARITE
      IF(ABS(DET) .GT. 1.0E-30) GO TO 70
      NERR=2
      WRITE(NOUT,1100)
 1100 FORMAT('FITFUN --- ERREUR - MATRICE SINGULIERE')
      RETURN
C
   70 CONTINUE
      CALL INVER (C,NCOEF,10,NERR,200)
      IF (NERR.NE.4) GO TO 80
      WRITE(NOUT,1200)
 1200 FORMAT('FITFUN --- ERREUR - MATRICE MAL CONDITIONNEE')
      RETURN
C
   80 CONTINUE
      DO 90 J=1,NCOEF
      BB(J)= 0.0
      DO 90 I=1,NCOEF
   90 BB(J)= C(J,I)*B(I) + BB(J)
      DO 100 I=1,NCOEF
 100  B(I)= BB(I)
C
      IF (.NOT.LINEAR) GO TO 120
C               LINEAIRE
      DO 110 I=1,NCOEF
  110 COEF(I)=B(I)
      GO TO 240
C               NON-LINEAIRE
C          TEST DE CONVERGENCE
  120 CONTINUE
      IF(EPS1.NE.0.0) GO TO 140
C              SANS CONVERGENCE
      DO 130 I=1,NCOEF
  130 TB(I)=COEF(I)+B(I)
      GO TO 170
C              AVEC CONVERGENCE
  140 CONTINUE
      DO 160 I=1,NCOEF
      ARG=EPS1*COEF(I)
      IF( ABS(ARG) .GE. ABS(B(I))) GO TO 150
      TB(I)=COEF(I)+ SIGN(ARG,B(I))
      GO TO 160
  150 TB(I)=COEF(I)+B(I)
  160 CONTINUE
C               TEST DU CRITERE DE CONVERGENCE
  170 CONTINUE
      DO 180 I=1,NCOEF
      CONV= ABS(COEF(I)/TB(I)-1.0)
      IF(CONV.GT.EPS0) GO TO 190
  180 CONTINUE
      GO TO 220
  190 CONTINUE
      DO 200 I=1,NCOEF
  200 COEF(I)=TB(I)
      IF(INTP.LT.2) GO TO 210
C                IMPRESSION INTERMEDIAIRE DES COEFFICIENTS
C
      WRITE (NOUT,1300) (COEF(I),I=1,NCOEF)
 1300 FORMAT (1X,10E13.6)
C
  210 CONTINUE
      NERR=3
      WRITE(NOUT,1400)
 1400 FORMAT('FITFUN --- ERREUR - NON CONVERGENCE')
      RETURN
C
  220 CONTINUE
      DO 230 I=1,NCOEF
  230 COEF(I)=TB(I)
C
C      CALCULS DE VARIANCE --------------------------------
C
  240 CONTINUE
      SUMW=0.0
      VARF=0.0
      ERMAX=0.
      DO 250 JAY=1,NPOINT
      CALL FONCT( NCOEF,NVAR,X(1,JAY),COEF,YC,DERIVE )
      IF(LINEAR) YC=ACCUM2 (COEF,DERIVE,NCOEF)
      F1=Y(JAY)-YC
      WI=1.0
      IF(NW.GT.0) CALL POIDS ( NW,Y(JAY),YC,W(JAY),WI )
      VARF=VARF+WI*F1**2
      F1=ABS(F1)
      IF(F1.LT.ERMAX) GO TO 250
      ERMAX=F1
      JMAX=JAY
  250 SUMW=SUMW+WI
C
      VARF=VARF/(FLOAT (NPOINT)-FLOAT (NCOEF))
      VARF1=VARF/SUMW
C
      IF (INTP.LT.1) GO TO 290
C
      WRITE(NOUT,1500) NCIT,VARF,VARF1
 1500 FORMAT(/' Ajustement obtenu avec',I4,' iterations'
     +2X,5HVARF=, E10.4,2X,6HVARF1=E10.4//
     +19X,'Coefficient',5X,'Dev. std'/)
      DO 260 I=1,NCOEF
      STDE=SQRT (C(I,I)*VARF)
      WRITE(NOUT,1600) I,COEF(I),STDE
 1600 FORMAT(11X,I5,2E14.6)
  260 CONTINUE
C
C     IMPRESSIONS ---------------------------------------
C
      WRITE (NOUT,1700) (BLANC,L,L=1,NVAR)
 1700 FORMAT(//9X,6HY(obs),5X,7HY(calc),6X,6HErr. %,6X,6HVar(Y),
     + 7(A7,2HX(,I1,2H) ))
      WRITE(NOUT,1050)
      DO 280 JAY=1,NPOINT
      CALL FONCT( NCOEF,NVAR,X(1,JAY),COEF,YC,DERIVE )
      IF(LINEAR) YC=ACCUM2 (COEF,DERIVE,NCOEF)
      F1=Y(JAY)-YC
      VY=0.0
      YF1=Y(JAY)
      IF(NW.EQ.3) YF1=YC
      PCER=0.
      IF(YF1.NE.0.0) PCER=100.*F1/YF1
      DO 270 I=1,NCOEF
      DO 270 K=1,NCOEF
  270 VY=VY+DERIVE(I)*DERIVE(K)*C(I,K)
      VY=VY*VARF
      ICAR=BLANC
      IF(JMAX.EQ.JAY) ICAR=ETOILE
      WRITE(NOUT,1800) JAY, Y(JAY),ICAR,YC,PCER,VY,(X(L,JAY),L=1,NVAR)
 1800 FORMAT(1X,I4,E12.4,A1,E11.4,9E12.4)
  280 CONTINUE
C
  290 CONTINUE
      IF(NINV.LE.0) GO TO 320
      WRITE (NOUT,1900)
 1900 FORMAT (1H0 13X,15HMATRICE INVERSE)
      DO 300 I=1,NCOEF
C
  300 WRITE (NOUT,2000) (C(I,J),J=1,NCOEF)
 2000 FORMAT (1H0,  8E12.4)
  320 CONTINUE
C
      RETURN
      END
