
%token A 
%token C
%token G
%token U

%%

RNA : RNA DoubleStrand  
    | DoubleStrand	
    | RNA Base		
    | Base		
    ;


DoubleStrand	   : A SingleStrand U
                   | A InternalLoop U
                   | A MultipleLoop U
                   | G SingleStrand C
                   | G InternalLoop C
                   | G MultipleLoop C
                   | G SingleStrand U
                   | G InternalLoop U
                   | G MultipleLoop U
                   | U SingleStrand A
                   | U InternalLoop A
                   | U MultipleLoop A
                   | C SingleStrand G
                   | C InternalLoop G
                   | C MultipleLoop G
                   | U SingleStrand G
                   | U InternalLoop G
                   | U MultipleLoop G
                   ;


MultipleLoop : MultipleLoop DoubleStrand
             | DoubleStrand
             | MultipleLoop Base
             | Base
             ;


Base : A
     | U
     | C
     | G
     ;

SingleStrand : Base		
             | SingleStrand Base
             ;
%%

