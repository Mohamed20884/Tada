%{
#include "tokens.h"
%}

DIGIT    [0-9]
IDENT	[a-zA-Z][A-Za-z0-9]*
         
%%

";" 			{ return SEMI;}
":=" 			{ return ASSIGN;}
"procedure" 	{ return PROCEDURE;}
"function"		{ return FUNCTION;}
","				{ return COMMA;}
"call_function" { return CALLFUNCTION;}
"is" 			{ return IS;}
"case"        	{ return CASE;}
"end case"    	{ return ENDCASE;}
"when"        	{ return WHEN;}
"exit"        	{ return EXIT;}
"=>"          	{ return ARROWRIGHT;}
"begin"			{ return TBEGIN;}
"end loop"		{ return ENDLOOP;}
"end if"		{ return ENDIF;}
"end"			{ return END;}
": Integer"		{ return DEF_INT;}
"in : Integer" { return INPUT_INT;}
"out : Integer" { return OUTPUT_INT;}
"if"			{ return IF;}
"then"			{ return THEN;}
"else"			{ return ELSE;}
"for" 			{ return FOR;}
"in"  			{ return IN;}
"loop" 			{ return LOOP;}
"+"  			{return PLUS;}
"-"				{ return MINUS;}
"*" 			{ return STAR;}
"/" 			{ return SLASH;}
"(" 			{ return LBRA;}
")" 			{ return RBRA;}
"<" 			{ return LT;}
"<=" 			{ return LTE;}
"==" 			{ return EQ;}
"!=" 			{ return NEQ;}
">" 			{ return GT;}
">=" 			{ return GTE;}
".."			{ return DOTS;}
{DIGIT}+ 		{ return INT;}
{IDENT}	 		{ return ID;}
<<EOF>>	 		{ return EOF;}

[ \t\n]+          /* eat up whitespace */


%%

int yywrap() { return EOF; }
