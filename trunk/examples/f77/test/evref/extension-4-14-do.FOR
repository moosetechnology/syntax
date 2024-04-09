      subroutine sub (
     &  file, ds 
     &  nd, nc, nv,
     &  values )
      integer nd,nc,nv
      character*256  file
       integer   ds(nd)
      real         vr(nv)
       integer        id,ic, iv,go
       character*24 c
      open(15,file=file,IOSTAT=go)
      read(15,*,IOSTAT=go)c
      do while(go.eq.0)
        do ic=1,nc+1
          if(cs(ic).eq.c)then
            goto 11
          endif
        enddo
11    continue
       do id=1,nd+1
          if(ds(id).eq.det)then
            goto 13
          endif
        enddo
13    continue
        do iv=1,nv
          values(iv,id,ic) = vr(iv)
        enddo
      enddo
      END
