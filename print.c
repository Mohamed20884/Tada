#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"

extern int symb;
extern char * yytext;

char * showSymb(int symb)
{  switch(symb)
   {  case  PROCEDURE: return "PROCEDURE";
	  case  PROCEDURES: return "PROCEDURES"; 
	  case  FUNCTION: return "FUNCTION";
	  case  CALLFUNCTION: return "CALLFUNCTION";
	  case  COMMA: return ",";
	  case CASE: return "CASE";
	  case ENDCASE: return "ENDCASE";
	  case WHEN: return "WHEN";
	  case EXIT: return "EXIT";
	  case  PARAM: return "PARAM";
	  case  PARAMS: return "PARAMS";
      case  IS: return "IS";
      case  INPUT_INT: return "INPUT_INT";
      case  OUTPUT_INT: return "OUTPUT_INT";
      case  DEF_INT: return "DEF_INT";
      case  TBEGIN: return "BEGIN";
      case  ELSE: return "ELSE";
      case  END: return "END";
      case  COMMANDS: return "COMMANDS";
      case  ID: return "ID";
      case  DEF: return "DEF";
      case  DEFS: return "DEFS";
      case  IF: return "IF";
      case  ENDIF: return "END IF";
      case  FOR: return "FOR";
      case  IN: return "IN";
      case  DOTS: return "..";
      case  LOOP: return "LOOP";
      case  ENDLOOP: return "END LOOP";
      case  INT: return "INT";
      case  THEN: return "THEN";         
      case  PLUS: return "+";
      case  MINUS: return "-";
      case  STAR: return "*";
      case  SLASH: return "/";
      case  LBRA: return "(";
      case  RBRA: return ")";
      case  LT: return "<";
      case  LTE: return "<=";
      case  EQ: return "=";
      case  NEQ: return "!=";
      case  GTE: return ">=";
      case  GT: return ">";
      case  ASSIGN: return ":=";
      case ARROWRIGHT: return "=>";
      case SEMI: return ";";
      case EOF: return "EOF";
      default: printf("bad symbol: %d",symb);
   }
}

printSymb()
{  char * s;
   printf("%s ",showSymb(symb));
   if(symb==ID || symb==INT){
    if(yytext == NULL)
      printf("Error: yytext is null");
    else
      printf("%s\n",yytext);
   } else
    printf("\n");
}
