c begin "xseg.seg"
c     -- segment generique pour manipule la memoire de tout segment
      integer xsize
      segment,xseg
        integer xdata(xsize)  
      endsegment
c end "xseg.seg"
