c  Source: https://github.com/johannesgerer/jburkardt-f77
c  File name: steam_nbs_interact/steam_nbs_interact.f
c
C        P R O P E R T I E S   O F    S T E A M      -  N B S / N R C 
C 
C                                                         1/14/85 - RCS 
C 
C      THIS FILE ADAPTED FROM PAGES 302-312 OF THE NBS/NRC STEAM TABLES 
C      BY HAAR, GALLAGHER, AND KELL, AND PUBLISHED BY HEMISPHERE
C      PUBLISHING CORP. IN 1984.  THE PROGRAM HAS BEEN MODIFIED TO: 
C          1) COMPLY WITH MICROSOFT FORTRAN FOR IBM PC
C          2) ADD ROUTINE FOR SATURATED PROPERTIES = F(T) & = F(P)
C          3) IMPROVED CONVERGENCE ON G IN PCORR & TCORR TO 10-8
C          4) SET UREF AND SREF IN THERM TO GIVE ZERO TO BETTER THAN
C             1X10-12 FOR U AND S AT THE TRIPLE POINT 
C          5) COMPILED FOR USE WITH THE MATH COPROCESSOR (THE 8087
C             CHIP TO INCREASE SPEED BY ABOUT FACTOR OF 20 TO 1)
C          6) ACCEPT KEYBOARD OR FILE INPUT. INPUT FILE FORMAT
C             DEFINED IN EXTERNAL WRITEUP BUT LIKE KEYBOARD INPUT.
C          7) IMPROVE OUTPUT FORMAT, TEST FOR LIMITS AND PRINT
C             WARNINGS, UPDATE UNITS CONVERSION CONSTANTS ETC.
C          8) MODIFIED PS(T) ABOVE 373.15C TO PROVIDE CONVERGENCE TO
C             CRITICAL POINT ETC. 
C 
C 
      PROGRAM NBSSTM
C 
C 
C   THIS IS A MAIN PROGRAM WHICH WILL SERVE AS AN EXAMPLE FOR 
C   THE USE OF THE SUBROUTINES AND FUNCTIONS FOLLOWING, AND WHICH 
C   WILL PRINT OUT VALUES OF VARIOUS PROPERTIES CALCULATED FOR A
C   GIVEN INPUT POINT TO A LARGE NUMBER OF SIGNIFICANT FIGURES, 
C   SUITABLE FOR USE AS A CHECK ON THE OPERATION OF THE PROGRAM.
C   THE USER IS QUERIED AS TO THE INPUT/OUTPUT UNITS DESIRED. 
C   THE INTERNAL STEAM PROPERTY UNITS ARE:  
C         TEMPERATURE = DEG K 
C         PRESSURE    = MPA 
C         DENSITY     = G/CM3 
C         ENTHALPY    = KJ/KG 
C         ENTROPY     = KJ/KG K 
C 
      IMPLICIT REAL*8(A-H,O-Z)
      CHARACTER*8 NT,ND,NP,NH
      COMMON /UNITS/ IT,ID,IP,IH,FT,FD,FP,FH
      COMMON /CUNITS/ NT,ND,NP,NH
      COMMON /QQQQ/ Q0,Q5 
      COMMON /FCTS/ AD,GD,SD,UD,HD,CVD,CPD,DPDT,DVDT,CJTT,CJTH
      COMMON /ACONST/ WM,GASCON,TZ,AA,Z,DZ,Y,UREF,SREF
      COMMON /NCONST/ G(40),II(40),JJ(40),NC
      CHARACTER *2NS, NSS1, NSS2
      CHARACTER *14BLANK, FNAME 
      CHARACTER *1ANS, NN, YY 
      DATA NSS1,NSS2/ ' M', 'FT' /
      DATA BLANK / '                ' / 
      DATA NN, YY / 'N', 'Y' /
      DATA TCRIT / 647.1260000001D0 / 
      DATA PCRIT / 22.055D0 / 
C 
    1 WRITE(*,*) ' IS THE INPUT DATA ON A FILE (Y/N)?'
    2 IF(IFC .EQ. 5) CLOSE (IFC)
      IFC = 0 
      READ(*,'(A1)', END = 106) ANS 
      FNAME = BLANK 
      IF(ANS .EQ. YY) GO TO 3 
      IF(ANS .EQ. NN) GO TO 4 
      WRITE(*,*) ' IMPROPER INPUT - TYPE Y OR N UPPERCASE'
      GO TO 2 
C 
C   OPEN FILE FOR INPUT DATA
C 
    3 WRITE(*,*) ' SPECIFY INPUT FILE NAME' 
      READ(*,'(A14)') FNAME 
      OPEN(5, FILE = FNAME) 
    4 IFC = 5
      CALL UNIT(IFC)
      NS=NSS1 
      IF(ID.EQ.4) NS=NSS2 
    5 WRITE(*,11) 
    6 IOPT = 0
      X = 0 
      TT = 0
      READ(IFC,*,END=106) IOPT,X,TT 
      IF(IOPT .EQ. 0) GO TO 106 
      IF(IOPT .EQ. 5) GO TO 4 
      IF(IOPT .EQ. 6) GO TO 1 
      IF(IFC .NE. 0) WRITE(*,25) IOPT, X, TT
      T=TTT(TT) 
      IF(IOPT .LT. 4) GO TO 100 
      IF(IOPT .EQ. 4) GO TO 104 
      WRITE(*,*) ' INVALID OPTION - TRY AGAIN'
      GO TO 5 
  100 IF(T .LE. 0) GO TO 16 
      RT=GASCON*T 
      CALL BB(T)
      GO TO (101,102,103),IOPT
C 
C     CALCULATE AS A FUNCTION OF DENSITY AND TEMPERATURE
C 
  101 DD=X
      IF(X .LE. 0D0) GO TO 17 
      D=DD*FD 
      CALL QQ(T,D)
      ZDUM = BASE(D,T)
      PRES = FP*(RT*D*Z + Q0) 
      DQ=RT*(Z+Y*DZ)+Q5 
  111 CALL THERM(D,T) 
      U = UD*RT*FH
      C=DSQRT(DABS(CPD*DQ*1.D3/CVD))
      IF(ID.EQ.4) C=C*3.280833D0
      H = HD*RT*FH
      S = SD*GASCON*FH*FT 
      GG = RT*FH*GD 
      CP=CPD*GASCON*FH*FT 
      CV=CVD*GASCON*FH*FT 
      V = 1.0D0/DD
      DPDD = DQ*FD*FP 
      DPDT1=DPDT*FP*FT
      WRITE(*,*) ' '
      WRITE(*,23) TT, NT
      WRITE(*,21) PRES,NP,H,NH,DPDT1,DD,ND,U,NH,DVDT,V,GG,NH,DPDD,CP, 
     1 NH,NT,S,NH,NT,CJTT,CV,NH,NT,C,NS,CJTH
      IF (IOPT .NE. 1) GO TO 112
      IF (T .GE. TCRIT) GO TO 112 
      DLL = 0.0D0 
      DVV = 0.0D0 
      CALL PCORR(T, PSAT, DLL, DVV) 
      CALL QQ(T,DLL)
      Z = BASE(DLL,T) 
      PL = FP*(RT*DLL*Z + Q0) 
      CALL THERM(DLL,T) 
      GL = RT*FH*GD 
      CALL QQ(T,DVV)
      Z = BASE(DVV,T) 
      PV = FP*(RT*DVV*Z + Q0) 
      CALL THERM(DVV,T) 
      GV = RT*FH*GD 
      IF(DD .GE. DLL/FD) GO TO 112
      IF(DD .LE. DVV/FD) GO TO 112
      WRITE (*,*) ' **CAUTION - DENSITY, TEMP. DEFINE METASTABLE STATE' 
      WRITE (*,24) PL, NP, GL, NH, DLL/FD, ND, PV, NP, GV, NH,
     1 DVV/FD, ND 
  112 CONTINUE
      GO TO 5
C 
C     CALCULATE AS A FUNCTION OF PRESSURE AND TEMPERATURE 
C 
  102 PRES=X
      IF(X .LE. 0.0) GO TO 18 
      P=PRES/FP 
      DGSS=P/T/.4D0 
      PSAT=2.D4 
      DLL=0.D0
      DVV=0.D0
      IF(T.LT. TCRIT) CALL PCORR(T,PSAT,DLL,DVV)
      IF(P.GT.PSAT) DGSS = DLL
      CALL DFIND(D,P,DGSS,T,DQ) 
      DD = D/FD 
      GO TO 111 
C 
C     CALCULATE SATURATION PROPERTIES AS A FUNCTION OF T
C 
  103 IF (T .GE. TCRIT) GO TO 16
      DLL = 0.0 
      DVV = 0.0 
      CALL PCORR(T, PSAT, DLL, DVV) 
C 
C 
C     CALCULATE SATURATED LIQUID VALUES 
C 
  108 D = DLL 
      DD = D/FD 
      TT = TTI(T) 
      CALL QQ(T,D)
      Z = BASE(D,T) 
      PL = FP*(RT*D*Z + Q0) 
      CALL THERM(D,T) 
      U = UD*RT*FH
      DQ=RT*(Z+Y*DZ)+Q5 
      C=DSQRT(DABS(CPD*DQ*1.D3/CVD))
      IF(ID.EQ.4) C=C*3.280833D0
      H = HD*RT*FH
      S = SD*GASCON*FH*FT 
      GG = RT*FH*GD 
      CP = CPD*GASCON*FH*FT 
      CV = CVD*GASCON*FH*FT 
      VL = 1.0D0/DD 
      DPDD = DQ*FD*FP 
      DPDT1 = DPDT*FP*FT
      WRITE (*,*) ' ' 
      WRITE (*,20) TT, NT 
      WRITE (*,21) PL,NP,H,NH,DPDT1,DD,ND,U,NH,DVDT,
     1 VL,GG,NH,DPDD,CP,NH,NT,S,NH,NT,CJTT, 
     2 CV,NH,NT,C,NS,CJTH 
C 
C     CALCULATE SATURATED VAPOR VALUES
C 
      D = DVV 
      DD = D/FD 
      TT = TTI(T) 
      CALL QQ(T,D)
      Z = BASE(D,T) 
      PV = FP*(RT*D*Z + Q0) 
      CALL THERM(D,T) 
      U = UD*RT*FH
      DQ=RT*(Z+Y*DZ)+Q5 
      C = DSQRT(DABS(CPD*DQ*1.D3/CVD))
      IF (ID.EQ.4)  C = C*3.280833D0
      H = HD*RT*FH
      S = SD*GASCON*FH*FT 
      GG = RT*FH*GD 
      CP = CPD*GASCON*FH*FT 
      CV = CVD*GASCON*FH*FT 
      VG = 1.0D0/DD 
      DPDD = DQ*FD*FP 
      DPDT1 = DPDT*FP*FT
      WRITE (*,*) ' ' 
      WRITE (*,22) TT,NT
      WRITE (*,21) PV,NP,H,NH,DPDT1,DD,ND,U,NH,DVDT,
     1 VG,GG,NH,DPDD,CP,NH,NT,S,NH,NT,CJTT, 
     2 CV,NH,NT,C,NS,CJTH 
      GO TO 112 
C 
C     CALCULATE SATURATION PROPERTIES AS A FUNCTION OF P
C 
  104 P=X/FP
      IF(P .LE. 0D0 .OR. P .GT. PCRIT) GO TO 18 
      DLL=0.0 
      DVV= 0.0
      CALL TCORR(TSAT,P,DLL,DVV)
      T = TSAT
      RT = GASCON*T 
      CALL BB(T)
      GO TO 108 
C 
C 
  106 STOP 'END NBS/NRC STEAM PROPERTIES'
C 
C 
   11 FORMAT(//' ENTER OPTION NO., X, AND TEMPERATURE WHERE:' / 
     1 '       1,D,T    D = DENSITY'/ 
     2 '       2,P,T    P = PRESSURE'/
     3 '       3, ,T    CALCULATE SATURATED VALUES AS F(T)'/
     4 '       4,P, ,   CALCULATE SATURATED VALUES AS F(P)'/
     5 '       5, , ,   CHANGE UNITS'/
     6 '       6, , ,   CHANGE SOURCE OF INPUT'/
     7 '       0, , ,   TO QUIT AND RETURN TO DOS'/)
   20 FORMAT(' SATURATED LIQUID PROPERTIES',8X,'T = ',F9.4,' DEG ',A1,
     1 5X,'NBS/NRC STEAM TABLES') 
   21 FORMAT(' P =',F15.8,1X,A6,4X,'H =',F14.7,1X,A7,3X,'DP/DT =',
     1 F15.8/' D =',F15.8,1X,A6,4X,'U =',F14.7,1X,A7,3X,'DV/DT =',F15.8/ 
     2 ' V =',F15.8,11X,'G =',F14.7,1X,A7,3X,'DP/DD =',F15.8/ 
     3 ' CP =',F14.8,1X,A6,A1,3X,'S =',F14.7,1X,A6,A1,3X,'JT(T) ='
     4 ,F15.8/ ' CV =',F14.8,1X,A6,A1,3X,'VEL SND =',F8.1,1X,A2,
     5 '/SEC',4X,'JT(H) =',F15.8) 
   22 FORMAT(' SATURATED VAPOR PROPERTIES',9X,'T = ',F9.4,' DEG ',A1, 
     1 5X,'NBS/NRC STEAM TABLES') 
   23 FORMAT(' COMPR. WATER OR SUPERHEATED STEAM',2X,'T = ',F9.4, 
     1 ' DEG ',A1,5X,'NBS/NRC STEAM TABLES')
   24 FORMAT ( ' PL =',F14.8,1X,A6,4X,'GL=',F14.8,1X,A7,1X,'DL='
     1 ,F14.8,1X,A6,/' PV =',F14.8,1X,A6,4X,'GV=',F14.8,1X,A7, 
     2 1X,'DV=',F14.8,1X,A6)
   25 FORMAT( ' IOPT =',I5,2X,'X =',D25.18,2X,'T =',D25.18)
   16 WRITE(*,*) 'TEMPERATURE OUT OF RANGE - TRY AGAIN' 
      GO TO 5 
   17 WRITE(*,*) 'DENSITY OUT OF RANGE - TRY AGAIN' 
      GO TO 5 
   18 WRITE(*,*) 'PRESSURE OUT OF RANGE - TRY AGAIN'
      GO TO 5 
      END 
