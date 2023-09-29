
Hubert Garavel -- 28/05/2023 21:41:07 

This grammar of FORTRAN 77 is based upon the ANSI standard syntax charts
Annex F of https://nvlpubs.nist.gov/nistpubs/Legacy/FIPS/fipspub69-1.pdf


Notes on F77 extracted from discussions with Pierre Boullier
------------------------------------------------------------

Zombie tokens
-------------

In SYNTAX, one can describe sub-languages of the main language (FORTRAN 77,
here) by specifying "entry points" in the BNF. For instance, the rules 2
and 3 of f77.bnf are:

<0:F77>		= !DO  =  <78:int_real_dp_expr> , ;

<0:F77>		= !FORMAT <59:format_specification> ;

where "!DO" and "!FORMAT" are called "zombie tokens". They are declared in
f77.lecl but are not defined by a regular expression. 

Note that the "!" symbol is not a negation, it is part of the token name 
(!DO and !FORMAT should be read as two identifiers). This is just a visual
convention adopted to distinguish these zombie tokens from normal tokens,
such as %label and %symbolic_name.

In a language such as FORTRAN, where keywords are not reserved, this mechanism
can be used to distinguish between a keyword and an identifier. For instance,
if one reads the sequence of letters « f o r m a t », f77.lecl recognizes
this sequence with the following regular expression: 

   %symbolic_name
		= id ; @1 ;
			Priority Shift > Reduce;
   where

   	id = (UPPER | LOWER @6) {UPPER | LOWER @6 | DIGIT} ;

The post-action @6 will decide whether « format » is a %symbolic_name or
the FORMAT keyword, which should be followed by a <59:format_specification>.

Therefore, after reading « format », one must examine the right context to
know if it matches <59:format_specification> and thus decide which token has
been recognized. Although LECL features an "Unbounded context" clause, it 
cannot be used in this situation, since the right context is not lexical
(i.e., described using regular expressions), but syntactic (i.e., described
using context-free grammars). Notice also that the language defined by
<59:format_specification> is infinite.

To do so, the post-action @6 can modify the flow of tokens read, by replacing
« format » by the zombie token !FORMAT and by recursively running SYNTAX,
using the standard function sxparse_in_la() of "sxparser.c", on the remaining
input text starting with the token !FORMAT. 

One question is to decide where to stop the analysis of the right-context
after this !FORMAT token, as all the input text following 
<59:format_specification> does not belong to the sublanguage and should be
considered as erroneous. The axiom rule
    <0:F77>		= !FORMAT <59:format_specification> ;
means (implicitly) that the text following « format » should terminate 
with the end of file. This issue is solved by simulating the presence of
an EOF token after <59:format_specification> to stop the analysis, so that
the entire FORTRAN input is not actually parsed when recognizing
<59:format_specification>. EOF is a particular token: once it has been
returned, any subsequent call to the token manager of SYNTAX will also return
EOF. This is useful for error recovery where one needs a fixed number of 
tokens in advance to apply the recovery models listed in the ".recor" file:
doing so, syntax errors occurring near the end of file can be handled using
the general error-recovery mechanism.

If the remainder is a <59:format_specification> (possibly modulo a few simple 
lexical or syntactic corrections), then @6 decides that it has read the 
« format » keyword and restarts a normal analysis following this token; 
otherwise, @6 decides that a  %symbolic_name has been read and continues 
the analysis normally. 

More fundamentally, this problem is typically a situation where unbounded
look-ahead is needed, and that we manage to solve using the LALR(1) part of 
SYNTAX that has only one symbol of look-ahead. The RLR approach introduced 
in P. Boullier's thesis might handle this automatically. The RCG approach
would certainly handle the matter, but the analysis time would no longer
be linear. But the combined use of standard LALR(1) and zombie tokens allows 
to solve the problem in linear time.



