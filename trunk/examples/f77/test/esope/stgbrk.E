      subroutine stgbrk(
c     == entrees ==
     & alist,splist,
c     == sorties ==
     & rlist,status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : stgbrk (langage Esope)
cb  Module     : STRING
cb  Auteur     : L.Brault
cb  Date       : 28-Octobre-1997
cb  Service    : EPNC 
cb  But        : briser/decouper des chaines de caracteres
cb               d'apres une liste de separateurs
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  alist  : "la" liste de chaines
cae  splist : "la" liste de separateurs
c
cas  rlist  : la liste des morceaux/decoupes de "la" chaine
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
#include <string.seg>
#include <list.seg>
c
      pointeur alist.list
      pointeur splist.list
      pointeur rlist.list
      integer  status
c
      pointeur xs.string
      pointeur sp.string
      pointeur ms.string
      integer  lxs
      integer  lsp
      integer  lmin
      integer  i,ia
      integer  idx,kdx
      integer  first,last
      integer  mfirst,mlast
c
      status=0
      nlist=10
      segini,rlist
      rlist.ilist=0
c
      segact,splist
      segact,alist
c
c     //repeter "decouper chaque chaine"
      do ia=1,alist.ilist
        xs=abs(alist.elist(ia))
c
        segact,xs
        lxs=xs.cstg(/1)
c
        if(lxs.eq.0)then
c         == la chaine a "decouper" est vide
c         >> ne rien renvoyer <<
        else
c         == la chaine a "decouper" n'est pas vide 
          first=1
          last=lxs
c
c         //repeter "decouper morceau"
 100      continue
c
c           //repeter "sauter separateur-tete"
c             -- chercher separateur-entete
c             -- ... donnant le plus petit separateur
 110          i=0
              lmin=-1
 120          i=i+1
              if(i.le.(splist.ilist))then
                sp=abs(splist.elist(i))
                segact,sp
                lsp=sp.cstg(/1)
                if(lsp.eq.0)then
                  idx=1
                else
                  idx=index(xs.cstg(first:last),
     +                      sp.cstg)
                endif
                if(idx.eq.1) then
c                 -- initialiser
                  if(lmin.eq.-1)lmin=lsp
c                 -- favoriser tout separateur-non-vide
                  if(lmin.eq.0)lmin=lsp
c                 -- prendre le plus petit separateur-non-vide
                  if((lsp.lt.lmin).and.(lsp.gt.0))lmin=lsp
                endif
                segdes,sp
                goto 120
              endif
c
c             //si trouve, sauter separateur-tete
              if(lmin.ne.-1) then
c               -- sauter separateur-entete
                first=first+lmin
c               //si possible, sauter prochain separateur-tete
                if((first.le.last).and.(lmin.ge.1))goto 110
              endif
c           //fin repeter "sauter separateur-tete"
c
c           //si possible, chercher morceau
            if(first.le.last)then
c             -- chercher separateur-morceau
c             -- ... donnant le plus petit morceau
              i=0
              kdx=0
              lmin=-1
 130          i=i+1
              if(i.le.(splist.ilist))then
                sp=abs(splist.elist(i))
                segact,sp
                lsp=sp.cstg(/1)
                if(lsp.eq.0)then
                  idx=1
                else
                  idx=index(xs.cstg(first:last),
     +                      sp.cstg)
                endif
                if(idx.ne.0) then
                  if(kdx.eq.0)then
                    kdx=idx
                    lmin=lsp
                  endif
                  if(idx.lt.kdx)then
                    kdx=idx
                    lmin=lsp
                  endif
                  if((idx.eq.kdx).and.(lsp.lt.lmin))then
                    kdx=idx
                    lmin=lsp
                  endif
                endif
                segdes,sp
                goto 130
              endif
c
c             -- determiner limites morceau
              if(kdx.ne.0) then
                mfirst=first
                if(kdx.eq.1)then
                  mlast=first
                else
                  mlast=first+kdx-2
                endif
              else
                lmin=0
                mfirst=first
                mlast=last
              endif
c
c             -- ranger morceau 
              lstg=mlast-mfirst+1
              segini,ms
                ms.cstg=xs.cstg(mfirst:mlast)
              segdes,ms
              rlist.ilist=rlist.ilist+1
              if((rlist.ilist).gt.nlist)then
                nlist=nlist+10
                segadj,rlist
              endif
              rlist.elist(rlist.ilist)=ms          
c
c             //si possible, decouper prochain morceau
              first=mlast+lmin+1
              if(first.le.last)goto 100
            endif
c
c         //fin repeter "decouper morceau"
        endif
c
        segdes,xs
      enddo
c     //fin repeter "decouper chaque chaine"
c
      segdes,alist
      segdes,splist
      segdes,rlist
      return
      end
