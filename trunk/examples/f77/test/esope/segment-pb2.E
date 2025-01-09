C test provided by Pierre Boullier
C segment "contemporain" avec ',' ...
       segment, exempl
        logical aa, bb(m)
        integer cc, dd(m)
        real ee, ff(m)
        complex gg, hh(m)
        double precision ii, jj(m)
        character kk*5, ll(m)*(n)
        implied mm, nn(m)
        pointeur oo.p, qq(m).r
        logical ss, tt(n, m)
       end segment

C segment "contemporain" sans ','
       segment exempl
        logical aa, bb(m)
        integer cc, dd(m)
        real ee, ff(m)
        complex gg, hh(m)
        double precision ii, jj(m)
        character kk*5, ll(m)*(n)
        implied mm, nn(m)
        pointeur oo.p, qq(m).r
        logical ss, tt(n, m)
       end segment

C segment "moyen-âgeux"
       segment/exempl/(
     1  aa*L, bb(m)*L1, L8*L8,
     2  cc*I, dd(m)*I2, I8*I8,
     3  ee*R, ff(m)*D, R8*R8, Q*Q,
     4  gg*C, hh(m)*CD, CQ*CQ,
C Est-ce permis ?
C     5  kk*5, ll(m)*(n)
     6  H*H
     7 )

C Exemples de segXXX avec ',' ...
       segact, f1, f2
       segdes, f1, s(1)
       segsup, f1, f2
       if (f1.EQ.0) segini, f1
       segadj, s
       segprt, s
       segina chara=m3cp, chaine
       segina chara=m3cp, bin
       segina chara=m3cp, xdr
       segind chara=m3cp, chaine
       segind chara=m3cp, bin
       segind chara=m3cp, xdr

C ... et sans
       segact f1, f2
       segdes f1, s(1)
       segsup f1, f2
       if (f1.EQ.0) segini f1
       segadj s
       segprt s

C       4.2 : Macronotation pour SEGXXX (XXX== INI SUP ACT DES ou ADJ), avec ou sans ','
C       segact, XYZ(*)
C ICI ambiguité, "segsupXYZ" est reconnu comme étant le nom d'un tableau et l'"*" comme
C la <16e:last_upper_dimension_bound>!! Comment la différencier d'une Macronotation ?
       segsup XYZ(*)

C p. 21-4.5 : DUPLICATION d'un segment, avec ou sans ','
       segini, s(k)=s(1)
       segini s(k)=s(1)

C p. 22-4.6 : Activation avec initialisation, avec ou sans ','
       segact, s(k)=s(1)
       segact s(k)=s(1)

C p. 23-5.1 : Le paramètre NOMOD
       segdes, s*NOMOD
       segdes s*NOMOD

C       5.2 : Le paramètre MRU (Most Recently Used), LRU (Last Recently Used)
       segdes, X(j)*MRU
       segdes X(j)*MRU

C p. 24.5.3 Utilisation de NOMOD, MRU
       segdes, s*(NOMOD, MRU)
       segdes s*(MRU, NOMOD)
