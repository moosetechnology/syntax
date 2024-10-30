      subroutine vced2(
c     == entrees ==
     & ov,pshp,title,format,delim,
c     == sorties ==
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vced2 (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : editer en 2D un objet VECTOR
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  ov     : un objet VECTOR
cae  pshp   : shape du VECTOR
cae  title  : titre de la carte 2D
cae  format : format de chaque element du VECTOR
cae  delim  : caracteres delimiteurs
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
c
      integer  mypnt
      external mypnt
c
      integer  length
      external length
c
#include <vector.obj>
#include <values.seg>
#include <string.seg>
c
      integer         ov
      pointeur        pshp.values
      character*(128) title
      character*(16)  format
      pointeur        delim.string
      integer         status
c
      integer         shpsiz
      integer         size
      integer         nl
      integer         nc
      integer         l
      integer         c
      integer         i
      integer         ibegin
      integer         iend
      integer         istep
      character*(128) fmt
      pointeur        dlm.string
      integer  ierr
c
      status=0
c
c     -- check version of vector v
      call vc0ckv(ov,ierr)
      if (ierr.ne.0) goto 999
c
c     -- ouvrir le VECTOR
      call actstr(ov)
      tvecto=mypnt(ov,1)
      segact,tvecto
      nvcflo=vecflo(/1)
      nvcent=vecent(/1)
      nvclog=veclog(/1)
      nvccha=veccha(/2)
      cvccha=veccha(/1)
      size=max(nvcflo,nvcent,nvclog,nvccha)
c
c     -- ouvrir "shape"
      if(pshp .eq. 0)then
        call edmess('vced2: shape non fournie')
        goto 999
      endif
c
      segact,pshp
      shpsiz =pshp.vale(/1)
c
c     -- verifier la forme
      if(shpsiz .ne. 2)then
        call edmess('vced2: shape pas de dimension 2')
        goto 999
      endif
c
      if(pshp.vale(1)*pshp.vale(2) .ne. size)then
        call edmess('vced2: shape et size incompatibles')
        goto 999
      endif
c
c     -- editer le titre
      write(*,'(1x)')
      write(*,'(1x,a)') title(:length(title,len(title)))
c
c     -- delimiteurs : prendre ceux par defaut ?
      if(delim .eq. 0)then
c       // ceux par defaut
        lstg=1
        segini,dlm
	dlm.cstg=' '
      else
c       // ceux fournis
        segini,dlm=delim
        lstg=dlm.cstg(/1)
      endif
c
c     -- editer les elements du VECTOR
      nl=pshp.vale(1)
      nc=pshp.vale(2)
c
      fmt=''
      write(fmt,'(i3)') nc
      fmt='(1x'//','''//dlm.cstg//''','//fmt(1:3)//
     &       '('//format(:length(format,len(format)))//
     &            ','''//dlm.cstg//''''//
     &       ')'//
     &    ')'
c
      write(*,'(1x)')
      do l=1,nl
c
        ibegin = l 
        iend   = l+(nc-1)*nl 
        istep  = nl 
c
        if(vectyp .eq. 'FLOTTANT')then
          write(*,fmt) (vecflo(i),i=ibegin,iend,istep)
c
        elseif(vectyp .eq. 'ENTIER')then
          write(*,fmt) (vecent(i),i=ibegin,iend,istep)
c
        elseif(vectyp .eq. 'LOGIQUE')then
          write(*,fmt) (veclog(i),i=ibegin,iend,istep)
c
        elseif(vectyp .eq. 'CHAINE')then
          write(*,fmt) (veccha(i),i=ibegin,iend,istep)
        endif
c
      enddo
      write(*,'(1x)')
c
c     -- menage
      segsup,dlm
c
c     -- fermer "shape"
      segdes,pshp*NOMOD
c
c     -- fermer le VECTOR
      segdes,tvecto*NOMOD
      call desstr(ov,'NOMOD')
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 999  continue      
      call edmess('vced2: routine en echec')
      status=1
      return
      end
