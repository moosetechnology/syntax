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


DoubleStrand : A SingleStrand U
             | G SingleStrand C
             | G SingleStrand U    
             | U SingleStrand A
             | C SingleStrand G
             | U SingleStrand G
             | A InternalLoop U
             | G InternalLoop C
             | G InternalLoop U
             | U InternalLoop A
             | C InternalLoop G
             | U InternalLoop G
	     | A MultipleLoop U
	     | G MultipleLoop C
	     | G MultipleLoop U
	     | U MultipleLoop A
	     | C MultipleLoop G
	     | U MultipleLoop G
	     ;


InternalLoop : DoubleStrand
	     | InternalLoop Base
	     | Base InternalLoop
	     ;

MultipleLoop : MultipleLoop DoubleStrand
             | DoubleStrand
             | MultipleLoop Base
             | Base
             ;

Base 	     : A
             | U
             | C
             | G
             ;

SingleStrand : SingleStrand Base
             | Base		
             ;

%%


Source texts:

 ARN_186:
GACGUAGACAACCCUGUGUUUAGCCUGCGUUUUGUGCCAUCCUAAUGCUUUACUAGAUCACUGAGCCACCUCCCAAGGACUACACCUAGCGGUAUUUCGUACAUUAACUAGGAUCCUUUUCCACAUGGGACGUAGACAACCCUGUGUUUAGCCUGCGUUUUGUGCCAUCCUAAUGCUUUACUAGAU


 ARN_683:
GACGUAGACAACCCUGUGUUUAGCCUGCGUUUUGUGCCAUCCUAAUGCUUUACUAGAUCACUGAGCCACCUCCCAAGGACUACACCUAGCGGUAUUUCGUACAUUAACUAGGAUCCUUUUCCACAUGGGACGUAGACAACCCUGUGUUUAGCCUGCGUUUUGUGCCAUCCUAAUGCUUUACUAGAUGACGUAGACAACCCUGUGUUUAGCCUGCGUUUUGUGCCAUCCUAAUGCUUUACUAGAUCACUGAGCCACCUCCCAAGGACUACACCUAGCGGUAUUUCGUACAUUAACUAGGAUCCUUUUCCACAUGGGACGUAGACAACCCUGUGUUUAGCCUGCGUUUUGUGCCAUCCUAAUGCUUUACUAGAUGACGUAGACAACCCUGUGUUUAGCCUGCGUUUUGUGCCAUCCUAAUGCUUUACUAGAUCACUGAGCCACCUCCCAAGGACUACACCUAGCGGUAUUUCGUACAUUAACUAGGAUCCUUUUCCACAUGGGACGUAGACAACCCUGUGUUUAGCCUGCGUUUUGUGCCAUCCUAAUGCUUUACUAGAUGACGUAGACAACCCUGUGUUUAGCCUGCGUUUUGUGCCAUCCUAAUGCUUUACUAGAUCACUGAGCCACCUCCCAAGGACUACACCUAGCGGUAUUUCGUACAUUAACUAGGAUCCUUUUCCACA
