      subroutine stgglu(
c     == entrees ==
     & alist,sp,
c     == sorties ==
     & rs,status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : stgglu (langage Esope)
cb  Module     : STRING
cb  Auteur     : L.Brault
cb  Date       : 28-Octobre-1997
cb  Service    : EPNC 
cb  But        : coller ensemble des chaines de caracteres
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  alist  : "la" liste de chaines
cae  sp     : "le" separateur a placer entre les chaines
c
cas  rs     : conversion de "la" chaine
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
#include <string.seg>
#include <list.seg>
c
      pointeur alist.list
      pointeur sp.string
      pointeur rs.string
      integer  status
c
      pointeur xs.string
      pointeur ys.string
      pointeur zs.string
      integer  lxs
      integer  lys
      integer  lzs
      integer  i
      integer  j
c
      status=0
      zs=abs(sp)
      segact,zs
      lzs=zs.cstg(/1)
c
      segact,alist
c
      lys=0
      do i=1,alist.ilist
        xs=abs(alist.elist(i))
        segact,xs
          lxs=xs.cstg(/1)
        segdes,xs
        lys=lys+lxs
        if(lxs.gt.0)lys=lys+lzs
      enddo
      if(lys.gt.0)lys=lys-lzs
c
      lstg=lys
      segini,rs
      j=1
      do i=1,alist.ilist
        xs=abs(alist.elist(i))
        segact,xs
          lxs=xs.cstg(/1)
          if(lxs.gt.0)then
            rs.cstg(j:j+lxs-1)=xs.cstg
            j=j+lxs
            if((lzs.gt.0).and.(j.lt.lys))then
              rs.cstg(j:j+lzs-1)=zs.cstg
              j=j+lzs
            endif
          endif
        segdes,xs
      enddo
      segdes,rs
c
      segdes,alist
      segdes,zs
c
      return
      end
