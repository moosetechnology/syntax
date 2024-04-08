       subroutine sub( n,
     &                   indx,
     &                   indxs, ierr )
      integer n
      integer indx(ni,n)
      integer indxs(ni,ncomp)
      integer ierr
      integer icomp, i, index
         do i=1,n
          do while((indx(i,icomp).ne.isx(index,icomp))
     &           .and.(index.lt.n))
              index = index + 1
            enddo
            if (index.eq.n) then
              ierr = 1
              index = 0
            endif
            indxs(i,icomp) = index
      enddo
      END
