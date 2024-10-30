C Source: https://heasarc.gsfc.nasa.gov/docs/heasarc/ofwg/docs/general/checksum/node29.html

        subroutine checksum(buffer,length,sum32)

C       Calculate a 32-bit 1's complement checksum of the input buffer, adding
C       it to the value of sum32.  This algorithm assumes that the buffer
C       length is a multiple of 4 bytes.

C       a double precision value (which has at least 48 bits of precision)
C       is used to accumulate the checksum because standard Fortran does not 
C       support an unsigned integer datatype.

C       buffer  - integer buffer to be summed
C       length  - number of bytes in the buffer (must be multiple of 4)
C       sum32   - double precision checksum value (The calculated checksum
C                 is added to the input value of sum32 to produce the 
C                 output value of sum32)

        integer buffer(*),length,i,hibits 
        double precision sum32,word32
        parameter (word32=4.294967296D+09)
C                 (word32 is equal to 2**32)

C       LENGTH must be less than 2**15, otherwise precision may be lost
C       in the sum
        if (length .gt. 32768)then
            print *, 'Error: size of block to sum is too large'
            return
        end if

        do i=1,length/4
            if (buffer(i) .ge. 0)then
                sum32=sum32+buffer(i)
            else
C               sign bit is set, so add the equivalent unsigned value
                sum32=sum32+(word32+buffer(i))
            end if
        end do

C       fold any overflow bits beyond 32 back into the word
10      hibits=sum32/word32
        if (hibits .gt. 0)then
            sum32=sum32-(hibits*word32)+hibits
            go to 10
        end if

        end


