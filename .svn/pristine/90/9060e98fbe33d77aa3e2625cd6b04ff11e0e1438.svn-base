/* Copyright (C) 1991-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */


/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */

/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */



/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */

/*
Thu Apr 21 18:44:49 1988  Philippe DESCHAMP  (deschamp at astree)

	* On remplace silencieusement, dans le scanner, les caracteres
	NULs dans les commentaires par la suite \0; ceci pour contourner
	un probleme dans SYNTAX: un commentaire n'est represente que par
	sa chaine de caractere, donc se termine au premier SXNUL.

Tue Apr 19 12:09:55 1988  Philippe DESCHAMP  (deschamp at astree)

	* Mise en un seul fichier de toutes les specifications existantes
	de paragrapheurs pour C. J'ai utilise diff(1), avec les options
	-DPPC2 et -DPPC3, puis edite le resultat de facon a ce que le
	preprocesseur C passe avec l'option -DPPC1 produise la
	specification du paragrapheur version 1, et de meme pour 2 et 3.
*/
/*	@(#)cgram.y	1.3	*/

%Lexical_Specification

Classes
	BLANC		= SP + HT ;
	HEXA		= DIGIT + A + B + C + D + E + F ;
	OCTAL		= "0".."7" ;

Abbreviations
	NOMBRE		= {DIGIT}+ ;
	ZERO		= "0"&True ;
	EXPOSANT	= ("E" @Upper_To_Lower | "e") ["+-"] NOMBRE ;
	X		= "X" @Upper_To_Lower | "x" ;
	L		= "l" @Lower_To_Upper | "L" ;
	BACKSLASH	= "\\"&True ;
	IGNORE		= BACKSLASH EOL ;
	NUL_IN_COMMENT	= @Put("\\") @Put ("0") -NUL&True ;
	CPP		= "#"&Is_First_Col {^EOL | NUL_IN_COMMENT | BACKSLASH Any} EOL ;
	COMMENT		= "/" "*" {Any | NUL_IN_COMMENT} "*" "/"&True | FF ;

Zombies
	%TYPE;

Tokens
	Comments	=
	   Union
	      ERASE	: {EOL | -BLANC}+ @Erase ;
	      KEEP	: {EOL | -BLANC} (COMMENT | @Put(EOL) CPP) @Mark
			  {BLANC | (EOL | COMMENT | CPP) @Mark} @Erase ;
	   End ;
			  Unbounded Context All But Comments ;
	%DIVOP		=
	   Union
	      DIV	: "/" ;
			  Context All But "*" ;
	      MOD	: "%" ;
	   End ;
	%EQUOP		= "!=" "=" ;
	%RELOP		= "<>" ["="] ;
	%SHIFTOP	= "<" "<" | ">" ">" ;
	%INCOP		= "-" "-" | "+" "+" ;
	"-"		= "-" @Erase ;
			  Priority Shift>Reduce ;
	"+"		= "+" @Erase ;
			  Priority Shift>Reduce ;
	%STROP		= "." | "-" ">" ;
	%UNOP		= "!~" ;
	"&"		= -"&" ;
			  Context All But "&" ;
	%CONST		=
	   Union
	      CHAR	: "'" {IGNORE} (^EOL | BACKSLASH (^EOL | OCTAL&True [OCTAL] [OCTAL])) {IGNORE} "'" ;
	      STRING	: QUOTE {^EOL | BACKSLASH Any} QUOTE&True ;
	      ICON	: (ZERO {OCTAL} | ZERO X {HEXA}+ | NOMBRE) [L] ;
	      FCON	: (NOMBRE | ZERO [NOMBRE]) (EXPOSANT | "." [NOMBRE] [EXPOSANT]) | "." NOMBRE [EXPOSANT] ;
	   End ;
			  Context All But %STROP ;
	%NAME		=
	   Union
	      ID	: {LOWER} (UPPER | "_" | LOWER DIGIT) {LETTER | DIGIT | "_"} ; @2 ;
			  Context All But %NAME ;
	      MOT_CLE	: {LOWER}+ ; @1 ;
			  Context All But %NAME, %CONST.ICON, %CONST.FCON ;
	   End ;
	%TYPE		= Any&False ; -- Not Generated

%%

%Terminal_Define

TYPE3	= %TYPE ;

%%

%Priorities

* at last count, there was one shift/reduce conflict,
* involving recognizing functions in declarations


%left ,
%right =
%right ? :
%left ||
%left "&&"
%left |
%left ^
%left "&"
%left %EQUOP
%left %RELOP
%left %SHIFTOP
%left + -
%left * %DIVOP
%right %UNOP
%right %INCOP sizeof
%left [ ( %STROP
%right while else
%nonassoc if
%%

%Syntactic_Specification

<C program>	= <ext_def_list> ~COL(1)~		~~ ;
	/* The next two rules to be used as filters by programs like Emacs */
<C program>	= <function_stmt> ~COL(1)~		~~ ;
<C program>	= ~COL(9)~ <statement2>	 ~COL(1)~	~~ ;

<ext_def_list>	=		~~ ;
<ext_def_list>	= <ext_def+1>	~~ ;
<ext_def_list>	= <ext_def+2>	~~ ;

<ext_def+1>	= <external_def>			~~ ;
<ext_def+1>	= <ext_def+1> ~SKIP(1)~ <external_def>	~~ ;
<ext_def+1>	= <ext_def+2> ~SKIP(4)~ <external_def>	~~ ;

<ext_def+2>	= <proc_def>				~~ ;
<ext_def+2>	= <ext_def+1> ~SKIP(4)~ <proc_def>	~~ ;
<ext_def+2>	= <ext_def+2> ~SKIP(4)~ <proc_def>	~~ ;

<external_def>	= <type_def>	~~ ;
<external_def>	= <data_def>	~~ ;

<type_def>	= <typedef> ";"		~~ ;

<typedef>	= <typedef> ,  <tdeclarator>				~~ ;
<typedef>	= typedef  <TYPE_list>  %TYPE				~~ ;
<typedef>	= typedef  <TYPE_list>  * <tdeclarator>			~~ ;
<typedef>	= <typedef()[]>						~~ ;
<typedef>	= typedef  <struct_dcl> ~SPACE~ ~TAB~ <tdeclarator>	~~ ;
<typedef>	= typedef  <enum_dcl> ~SPACE~ ~TAB~ <tdeclarator>	~~ ;

<typedef()[]>	= <typedef()[]> ( <> )				~~ ;
<typedef()[]>	= <typedef()[]> [ <.e> ]			~~ ;
<typedef()[]>	= typedef  <TYPE_list> ( <> )			~~ ;
<typedef()[]>	= typedef  <TYPE_list> [ <.e> ]			~~ ;
<typedef()[]>	= typedef  <TYPE_list>  ( <tdeclarator> )	~~ ;

<TYPE_list>	= <TYPE>		~~ ;
<TYPE_list>	= <TYPE_list>  <TYPE>	~~ ;

<tdeclarator>	= %TYPE				~~ ;
<tdeclarator>	= * <tdeclarator>		~~ ;
<tdeclarator>	= <tdeclarator> ( <> )		~~ ;
<tdeclarator>	= <tdeclarator> [ <.e> ]	~~ ;
<tdeclarator>	= ( <tdeclarator> ) 		~~ ;


<data_def>	= <attributes> ";"					~~ ;
<data_def>	= <init_dcl_list> ";"					~~ ;
<data_def>	= <attributes> ~SPACE~ ~TAB~ <init_dcl_list> ";"	~~ ;

<proc_def>	= <fdeclarator> <function_body>					~~ ;
<proc_def>	= <attributes> ~COL(1)~ <fdeclarator> <function_body>		~~ ;


<function_body>	= <arg_dcl_list> <function_stmt>	~~ ;

<function_stmt>	= ~COL(1)~ { ~SKIP~ ~COL(9)~ <dcl_stat_list> ~SKIP(2)~ ~COL(9)~ <stmt_list> ~COL(1)~ }	~~ ;

<arg_dcl_list>	= <arg_dcl_list> ~SPACE~ <declaration>	~~ ;
<arg_dcl_list>	=					~~ ;

<declaration>	= <attributes> ~SPACE~ <declarator_list> ";"		~~ ;
<declaration>	= <attributes> ";"					~~ ;
<declaration>	= <typedef> ";"						~~ ;


<dcl_stat_list>	= <dcl_stat_list> ~MARGIN~ <attributes> ";"				~~ ;
<dcl_stat_list>	= <dcl_stat_list> ~MARGIN~ <attributes> ~SPACE~ ~TAB~ <init_dcl_list> ";"	~~ ;
<dcl_stat_list>	= <dcl_stat_list> ~MARGIN~ <typedef> ";"				~~ ;
<dcl_stat_list>	= ~INH~									~~ ;

<attributes>	= <class>  <type>		~~ ;
<attributes>	= <type>  <class>		~~ ;
<attributes>	= <class>			~~ ;
<attributes>	= <type>			~~ ;
<attributes>	= <type>  <class>  <type>	~~ ;


<class>	= auto		~~ ;
<class>	= extern	~~ ;
<class>	= register	~~ ;
<class>	= static	~~ ;

<TYPE>	= %TYPE		~~ ;
<TYPE>	= char		~~ ;
<TYPE>	= double	~~ ;
<TYPE>	= float		~~ ;
<TYPE>	= int		~~ ;
<TYPE>	= long		~~ ;
<TYPE>	= short		~~ ;
<TYPE>	= unsigned	~~ ;
<TYPE>	= void		~~ ;

<type>	= <TYPE>			~~ ;
<type>	= <TYPE>  <TYPE>		~~ ;
<type>	= <TYPE>  <TYPE>  <TYPE>	~~ ;
<type>	= <struct_dcl>			~~ ;
<type>	= <enum_dcl>			~~ ;


<enum_dcl>	= <enum_head>  { <moe_list> <optcomma> }	~~ ;
<enum_dcl>	= enum  %NAME					~~ ;
<enum_dcl>	= enum  %TYPE					~~ ;


<enum_head>	= enum		~~ ;
<enum_head>	= enum  %NAME	~~ ;
<enum_head>	= enum  %TYPE	~~ ;


<moe_list>	= <moe>			~~ ;
<moe_list>	= <moe_list> ,  <moe>	~~ ;


<moe>	= %NAME			~~ ;
<moe>	= %NAME = <con_e>	~~ ;


<struct_dcl>	= <str_head>  { ~MARGIN+4~ <type_dcl_list> <optsemi> ~MARGIN~ }	~~ ;
<struct_dcl>	= struct  %NAME					~~ ;
<struct_dcl>	= struct  %TYPE					~~ ;
<struct_dcl>	= union  %NAME					~~ ;
<struct_dcl>	= union  %TYPE					~~ ;


<str_head>	= struct	~~ ;
<str_head>	= union		~~ ;
<str_head>	= struct  %NAME	~~ ;
<str_head>	= struct  %TYPE	~~ ;
<str_head>	= union  %NAME	~~ ;
<str_head>	= union  %TYPE	~~ ;


<type_dcl_list>	= <type_declaration>					~~ ;
<type_dcl_list>	= <type_dcl_list> ";" ~MARGIN~ <type_declaration>	~~ ;


<type_declaration>	= <type> ~SPACE~ ~TAB~ <declarator_list>	~~ ;
<type_declaration>	= <type>					~~ ;



<declarator_list>	= <declarator>				~~ ;
<declarator_list>	= <declarator_list> ,  <declarator>	~~ ;

<declarator>	= <fdeclarator>			~~ ;
<declarator>	= <nfdeclarator>		~~ ;
<declarator>	= <nfdeclarator>  :  <con_e>	~~ ;	%prec ,
<declarator>	= :  <con_e>			~~ ;	%prec ,


		/* int (a)();   is not a function --- sorry! */
<nfdeclarator>	= * <nfdeclarator>		~~ ;
<nfdeclarator>	= <nfdeclarator> ( <> )	~~ ;
<nfdeclarator>	= <nfdeclarator> [ <.e> ]	~~ ;
<nfdeclarator>	= %NAME  			~~ ;
<nfdeclarator>	= ( <nfdeclarator> ) 		~~ ;

<fdeclarator>	= * <fdeclarator>		~~ ;
<fdeclarator>	= <fdeclarator> ( <> )		~~ ;
<fdeclarator>	= <fdeclarator> [ <.e> ]	~~ ;
<fdeclarator>	= ( <fdeclarator> )		~~ ;
<fdeclarator>	= <name_lp>  <name_list>  )	~~ ;
<fdeclarator>	= <name_lp> )			~~ ;


<name_lp>	= %NAME (	~~ ;


<name_list>	= %NAME			~~ ;
<name_list>	= <name_list> ,  %NAME	~~ ;


<init_dcl_list>	= <init_declarator>			~~ ;	%prec ,
<init_dcl_list>	= <init_dcl_list> ,  <init_declarator>	~~ ;


<init_declarator>	= <nfdeclarator>		~~ ;
<init_declarator>	= <fdeclarator>			~~ ;
<init_declarator>	= <nfdeclarator>  =  <e>	~~ ;	%prec ,
<init_declarator>	= <nfdeclarator>  =  <> { <init_list> <optcomma> }	~~ ;


<init_list>	= <e>				~~ ;	%prec ,
<init_list>	= { <init_list> <optcomma> }	~~ ;
<init_list>	= <init_list> ,  <e>		~~ ;
<init_list>	= <init_list> , ~MARGIN~ <> { <init_list> <optcomma> }	~~ ;


<optcomma>	=	~~ ;
<optcomma>	= ,	~~ ;


<optsemi>	=	~~ ;
<optsemi>	= ";"	~~ ;


/*	STATEMENTS	*/

<stmt_list>	=		~~ ;
<stmt_list>	= <stmt+1>	~~ ;
<stmt_list>	= <stmt+2>	~~ ;

<stmt+1>	= <statement1>					~~ ;
<stmt+1>	= <statement3>					~~ ;
<stmt+1>	= <stmt+1> ~MARGIN~ <statement1>		~~ ;
<stmt+1>	= <stmt+2> ~SKIP(2)~ ~MARGIN~ <statement1>	~~ ;
<stmt+1>	= <stmt+1> ~SKIP(2)~ ~MARGIN~ <statement3>	~~ ;
<stmt+1>	= <stmt+2> ~SKIP(2)~ ~MARGIN~ <statement3>	~~ ;

<stmt+2>	= <statement2>					~~ ;
<stmt+2>	= <statement4>					~~ ;
<stmt+2>	= ~MARGIN+2~ <compoundstmt>			~~ ;
<stmt+2>	= <stmt+1> ~SKIP(2)~ ~MARGIN~ <statement2>	~~ ;
<stmt+2>	= <stmt+1> ~MARGIN~ <statement4>		~~ ;
<stmt+2>	= <stmt+1> ~SKIP(2)~ ~MARGIN+2~ <compoundstmt>	~~ ;
<stmt+2>	= <stmt+2> ~SKIP(2)~ ~MARGIN~ <statement2>	~~ ;
<stmt+2>	= <stmt+2> ~SKIP(2)~ ~MARGIN~ <statement4>	~~ ;
<stmt+2>	= <stmt+2> ~SKIP(2)~ ~MARGIN+2~ <compoundstmt>	~~ ;

<statement>	= <statement1>		~~ ;
<statement>	= <statement2>		~~ ;
<statement>	= <statement3>		~~ ;
<statement>	= <statement4>		~~ ;
<statement>	= <compoundstmt>	~~ ;

<compoundstmt>	=				~
    MARGIN-2~	{				~SKIP
 ~ ~MARGIN~	  <dcl_stat_list>		~SKIP(2)
 ~ ~MARGIN~	  <stmt_list>			~
    MARGIN-2~	}						~~ ;


<statement1>	= <e> ";"					~~ ;
<statement2>	= <ifprefix> ~MARGIN+4~ <statement>		~~ ; %prec ,
<statement2>	= <ifelprefix> ~SKIP~ ~MARGIN+4~ <statement>	~~ ;
<statement2>	= <whprefix> ~MARGIN+4~ <statement>		~~ ;
<statement2>	= <doprefix> ~SKIP~ ~MARGIN+4~ <statement> ~MARGIN~ while <(e)> ";"	~~ ;
<statement2>	= <forprefix> ~MARGIN+4~ <statement>		~~ ;
<statement2>	= <switchpart> ~MARGIN+4~ <statement>		~~ ;
<statement4>	= break <> ";"					~~ ;
<statement4>	= continue <> ";"				~~ ;
<statement4>	= return <> ";"					~~ ;
<statement4>	= return  <e> ";"				~~ ;
<statement4>	= goto  %NAME ";"				~~ ;
<statement1>	= ";"						~~ ;
<statement1>	= <label> <statement1>				~~ ;
<statement2>	= <label> <statement2>				~~ ;
<statement3>	= <label> <statement3>				~~ ;
<statement4>	= <label> <statement4>				~~ ;
<statement2>	= <label> ~MARGIN+2~ <compoundstmt>		~~ ;
<statement3>	= <case> <statement1>				~~ ;
<statement2>	= <case> <statement2>				~~ ;
<statement3>	= <case> <statement3>				~~ ;
<statement2>	= <case> <statement4>				~~ ;
<statement2>	= <case> ~MARGIN+2~ <compoundstmt>		~~ ;

<label>	= ~COL(1)~    %NAME :	     ~MARGIN-1~ ~SPACE~	~~ ;

<case>	= ~MARGIN-2~  case  <e> :	~MARGIN~	~~ ;
<case>	= ~MARGIN-2~  default <> :	~MARGIN~	~~ ;

<doprefix>	= do	~~ ;

<ifprefix>	= <if> <(e)>							~~ ;
<ifprefix>	= <ifprefix> ~MARGIN+4~ <statement> ~MARGIN~ else  <if> <(e)>	~~ ;

<ifelprefix>	= <ifprefix> ~MARGIN+4~ <statement> ~MARGIN~ else	~~ ;


<whprefix>	= while <(e)>	~~ ;

<forprefix>	= for <(3e)>	~~ ;

<switchpart>	= switch <(e)>	~~ ;


/*	EXPRESSIONS	*/

<con_e>	= <e>	~~ ;	%prec ,

<.e>	= <e>	~~ ;
<.e>	=	~~ ;

<(3e)>	= (  <.e> ";" < e> ";" < e>  )	~~ ;
< e>	= ~SPACE~ <e>	~~ ;
< e>	=		~~ ;

<elist>	= <e>			~~ ;	%prec ,
<elist>	= <elist> ,  <e>	~~ ;


<e>	= <e>  %RELOP  <e>	~~ ;
<e>	= <e> ,  <e>		~~ ;
<e>	= <e>  %DIVOP  <e>	~~ ;
<e>	= <e>  +  <e>		~~ ;
<e>	= <e>  -  <e>		~~ ;
<e>	= <e>  %SHIFTOP  <e>	~~ ;
<e>	= <e>  *  <e>		~~ ;
<e>	= <e>  %EQUOP  <e>	~~ ;
<e>	= <e>  "&"  <e>		~~ ;
<e>	= <e>  |  <e>		~~ ;
<e>	= <e>  ^  <e>		~~ ;
<e>	= <e>  "&&"  <e>	~~ ;
<e>	= <e>  ||  <e>		~~ ;
<e>	= <e>  * <> =  <e>	~~ ;
<e>	= <e>  %DIVOP =  <e>	~~ ;
<e>	= <e>  + <> =  <e>	~~ ;
<e>	= <e>  - <> =  <e>	~~ ;
<e>	= <e>  %SHIFTOP =  <e>	~~ ;
<e>	= <e>  "&" <> =  <e>	~~ ;
<e>	= <e>  | <> =  <e>	~~ ;
<e>	= <e>  ^ <> =  <e>	~~ ;
<e>	= <e>  ?  <e>  :  <e>	~~ ;
<e>	= <e>  =  <e>		~~ ;
<e>	= <term>		~~ ;

<term>	= <term> %INCOP			~~ ;
<term>	= * <term>			~~ ;
<term>	= "&" <term>			~~ ;
<term>	= - <term>			~~ ;
<term>	= %UNOP <term>			~~ ;
<term>	= %INCOP <term>			~~ ;
<term>	= <sizeof>  <term>		~~ ;	%prec sizeof
<term>	= ( <cast_type> )  <term>	~~ ;	%prec %INCOP
<term>	= <sizeof> ( <cast_type> )	~~ ;	%prec sizeof
<term>	= <term> [ <e> ]		~~ ;
<term>	= <funct_idn> )			~~ ;
<term>	= <funct_idn>  <elist>  )	~~ ;
<term>	= <term> %STROP %NAME		~~ ;
<term>	= <term> %STROP %TYPE		~~ ;
<term>	= %NAME				~~ ;
<term>	= %CONST			~~ ;
<term>	= ( <e> )			~~ ;


<cast_type>	= <type> <null_decl>	~~ ;


<null_decl>	=				~~ ;
<null_decl>	= ( <null_decl> ) ( <> )	~~ ;
<null_decl>	= * <null_decl>			~~ ;
<null_decl>	= <null_decl> [ <.e> ]		~~ ;
<null_decl>	= ( <null_decl> )		~~ ;


<funct_idn>	= <term> (	~~ ;

<if>		= if	~~ ;

<sizeof>	= sizeof		~~ ;

<(e)>	= (  <e>  )			~~ ;

<>	=	~~ ;	/* dummy non-terminal for comments positionning... */
%%

%Error_Recovery

Titles
	"",
	"Warning (ppc.anal):\t",
	"Error (ppc.anal):\t";

Scanner
	Local
		X 0 1 2 3 ; "A character \""%0"\" is inserted before \""$0"\".";
		1 2 3 4	; "The invalid character \""$0"\" is erased.";
		X 1 2 3 4 ; "The invalid character \""$0"\" is replaced by \""%0"\".";

		Dont_Delete = {};
		Dont_Insert = {#000..#037, "*", "/", "_"};

	Global
		Detection : -- parameter: character in error
			  "\"%s\" is erased.";
		KEYWORD	: "";
		Eol	: "\\n";
		Eof	: -- the "character"
			  "EOF";
		Halt	: "Lexical analysis stops.";

Parser
	Local
		0 S 2	; "Speling eror: \""$1"\" is replaced by \""%1"\".";
		S 1	; "Speling eror: \""$0"\" is replaced by \""%0"\" before \""$1"\".";
		X 1 2 3 4 ; "\""$0"\" is replaced by \""%0"\" before \""$1"\".";
		0 X 1 2 3 ; "\""%1"\" is inserted before \""$1"\".";
		0 X 2 3 4 ; "\""$1"\" is replaced by \""%1"\".";
		0 2 3 4	; "\""$1"\" is erased.";
		0 X X 1 2 3 4
			; "\""%1" "%2"\" is inserted before \""$1"\".";
		X 0 1 2 3 ; "\""%0"\" is inserted before \""$0" "$1"\".";
		1 2 3 4	; "\""$0"\" is erased before \""$1"\".";
		X 2 3 4	; "\""$0" "$1"\" is replaced by \""%0"\".";
		X X 1 2 3 ; "\""$0"\" before \""$1"\" is replaced by \""%0" "%1"\".";

		Dont_Delete = {"case", "default", "do", "else", "enum", "for", "if", "sizeof", "struct", "switch", "union", "while"};
		Dont_Insert = {"auto", "case", "char", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return", "short", "sizeof", "switch", "unsigned", "void", "while", "%CONST", "%DIVOP", "%EQUOP", "%INCOP", "%RELOP", "%SHIFTOP", "%STROP", "%UNOP", "-", "*", "&", "|", "^", "&&", "||", "?", "=", "{", "[", "*"};

	Forced_Insertion
		"\""%0"\" is inserted before \""$1"\"." ;

	Global
		Key_Terminals = {";", "{", "}", ",", "else"};
		Validation_Length = 2;
		Followers_Number <= 5
			: -- parameters: array (1:Followers_Number) of valid followers at detection point
			  "Only \"%s\"^( or \"%s\"^) is possible here.";
		Detection : -- parameters: none
			  "Cannot recover...";
		Restarting: -- parameters: array (1:Validation_Length) of valid followers at restarting point
			  "... Syntactic analysis restarts on \"%s\" \"%s\".";
		Eof	: -- the "token"
			  "EOF";
		Halt	: -- parameters: none
			  "... Analysis aborted.";

Abstract
	-- parameters: array (1:Titles_No) of number of messages
	"";

%%
