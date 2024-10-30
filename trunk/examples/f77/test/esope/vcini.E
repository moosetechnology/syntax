      subroutine vcini(
     & ov,
     & pshp,type,typlen,size,pval,
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcini (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : initialiser un VECTOR avec des valeurs
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  pshp   : shape du VECTOR
cae  type   : type des elements du VECTOR
cae  typlen : longueur du type des elements du VECTOR
cae           N.B. typlen < 0 <=> a determiner d'apres les valeurs
cae  size   : nombre d'elements du VECTOR
cae           N.B. size < 0 <=> a determiner d'apres les valeurs
cae  pval   : valeur scalaire
cas  ov     : un VECTOR
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
c
      integer  mypnt
      external mypnt
c
      integer  valsiz
      external valsiz
c
      character*8  valtpe
      external     valtpe
c
#include <vector.obj>
#include <values.seg>
#include <string.seg>
c
      integer       ov
      pointeur      pshp.values
      character*8   type
      integer       typlen
      integer       size
      pointeur      pval.values
      integer       status
c
      integer       ierr
      character*8   vatp
      integer       vasz
      integer       ovsz
      integer       shsz
      integer       shpsiz
      integer       i
      integer       ipos
      integer       l
      integer       c
      integer       nl
      integer       nc
c
      status=0
c
c     -- verifier la concordance des types
      vatp=valtpe(pval,ierr)
      if(ierr .ne. 0) goto 999
c
      if(vatp .ne. type)then
        call edmess('vcini: VECTOR et VALUES pas meme type')
        goto 999
      endif
c
c     -- verifier la coherence des tailles
      vasz=valsiz(pval,ierr)
      if(ierr .ne. 0) goto 999
c
      if(size .ge. 0)then
        if(vasz .ne. 1)then
          call edmess('vcini: VALUES pas un singleton')
          goto 999
        endif
      endif
c
c     -- determiner la taille du VECTOR
      if(size .ge. 0)then
        ovsz=size
      else
        ovsz=vasz
      endif
c
c     -- ouvrir "shape"
      if(pshp .ne. 0)then
        segact,pshp
        shpsiz =pshp.vale(/1)
      else
        shpsiz =0
      endif
c
c     -- si shape fournie, alors verifier shape 
      if(shpsiz .ne. 0)then
        if(shpsiz .ne. 2)then
          call edmess('vcini: SHAPE pas de dimension 2')
          goto 999
        endif
      endif
c
c     -- si shape fournie, alors verifier coherence des tailles 
      if(shpsiz .ne. 0)then
        shsz=1
	do i=1,shpsiz
	  shsz=shsz*pshp.vale(i)
	enddo
c
        if(size .ge. 0  .and.  shsz .ne.size)then
          call edmess('vcini: SHAPE et SIZE pas compatibles')
          goto 999
	endif        
c
        if(size .lt. 0  .and.  shsz .ne.vasz)then
          call edmess('vcini: SHAPE et VALUES pas compatibles')
          goto 999
	endif        
      endif
c
c     -- creer un VECTOR vide non type
      call vc0new(ov,ierr)
      if(ierr.ne.0) goto 999
c
c     -- ouvrir le VECTOR vide non type
      call actstr(ov)
      tvecto=mypnt(ov,1)
      segact,tvecto
      nvcflo=vecflo(/1)
      nvcent=vecent(/1)
      nvclog=veclog(/1)
      nvccha=veccha(/2)
      cvccha=veccha(/1)
c
c     -- typer le VECTOR
      if(type .eq. 'FLOTTANT')then
        vectyp='FLOTTANT'
c
      elseif(type .eq. 'ENTIER')then
        vectyp='ENTIER'
c
      elseif(type .eq. 'LOGIQUE')then
        vectyp='LOGIQUE'
c
      elseif(type .eq. 'CHAINE')then
        vectyp='CHAINE'
	cvccha=max(0,typlen)
        segadj,tvecto
      endif
c
c     -- re-dimensionner le VECTOR 
      if(type .eq. 'FLOTTANT')then
        nvcflo=ovsz
c
      elseif(type .eq. 'ENTIER')then
        nvcent=ovsz
c
      elseif(type .eq. 'LOGIQUE')then
        nvclog=ovsz
c
      elseif(type .eq. 'CHAINE')then
        nvccha=ovsz
      endif
c
      segadj,tvecto
c
c     -- ouvrir le conteneur "values"
      segact,pval
      valnf =pval.valf(/1)
      valne =pval.vale(/1)
      valnl =pval.vall(/1)
      valnc =pval.valc(/2)
      valsmx=pval.valc(/1)
c
c     -- initialiser les elements du VECTOR
      if(size .ge. 0)then
c       // valeur unique a repeter
c
        if(type .eq. 'FLOTTANT')then
          do i=1,ovsz
            vecflo(i)=pval.valf(1)
          enddo
c
        elseif(type .eq. 'ENTIER')then
          do i=1,ovsz
            vecent(i)=pval.vale(1)
          enddo
c
        elseif(type .eq. 'LOGIQUE')then
          do i=1,ovsz
            veclog(i)=pval.vall(1)
          enddo
c
        elseif(type .eq. 'CHAINE')then
          if(typlen .lt. 0)then
            cvccha=valsmx
            segadj,tvecto
          endif
c
          do i=1,ovsz
            veccha(i)=pval.valc(1)
          enddo
        endif
      else
c       // valeurs multiples a copier
c
        if(shpsiz .eq. 0)then
c         // sans shape
          if(type .eq. 'FLOTTANT')then
            do i=1,ovsz
              vecflo(i)=pval.valf(i)
            enddo
c
          elseif(type .eq. 'ENTIER')then
            do i=1,ovsz
              vecent(i)=pval.vale(i)
            enddo
c
          elseif(type .eq. 'LOGIQUE')then
            do i=1,ovsz
              veclog(i)=pval.vall(i)
            enddo
c
          elseif(type .eq. 'CHAINE')then
            if(typlen .lt. 0)then
              cvccha=valsmx
              segadj,tvecto
            endif
c
            do i=1,ovsz
              veccha(i)=pval.valc(i)
            enddo
          endif
c
        else
c         // avec shape
          nl=pshp.vale(1)
          nc=pshp.vale(2)
          i =0
          do l=1,nl
	    do c=1,nc
	      ipos=l+(c-1)*nl
              i   =i+1
c
              if(type .eq. 'FLOTTANT')then
                vecflo(ipos)=pval.valf(i)
c
              elseif(type .eq. 'ENTIER')then
                vecent(ipos)=pval.vale(i)
c
              elseif(type .eq. 'LOGIQUE')then
                veclog(ipos)=pval.vall(i)
c
              elseif(type .eq. 'CHAINE')then
                if(typlen .lt. 0)then
                  cvccha=valsmx
                  segadj,tvecto
                endif
                veccha(ipos)=pval.valc(i)
              endif
c
	    enddo
	  enddo
        endif
      endif
c
c     -- fermer "shape"
      if(pshp .ne. 0) segdes,pshp*NOMOD
c
c     -- fermer le conteneur "values"
      segdes,pval*NOMOD
c
c     -- fermer le VECTOR
      segdes,tvecto*MOD
      call desstr(ov,'MOD')
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 999  continue      
      call edmess('vcini: routine en echec')
      status=1
      return
      end
