      segment, t1
        real a1
      endsegment

      segment, t2
        pointeur a2.t1
      endsegment

      segment, t3
        pointeur a3.t2
      endsegment

      pointeur v1.t1
      pointeur v2.t2
      pointeur v3.t3

*     -- initialize segments
      segini, v1
      v1.a1 = 1.234

      segini, v2
      v2.a2 = v1

      segini, v3
      v3.a3 = v2

*     -- change the inner most segment

      v3.a3.a2.a1 = 1

*    ! generates Warning:	The invalid character "." is replaced by "(".      

      end
      