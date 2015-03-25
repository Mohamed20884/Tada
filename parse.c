#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "nodes.h"

int symb;

extern char* showSymb(int);
extern int yylex(void);
extern FILE* yyin;

extern void prettytree(NODE*,int);
extern char * yytext;

lex()
{  
   printSymb();
   symb = yylex();
   //printf("\nyytext=%s\n", yytext);
}

NODE * newInt(int v)
{  NODE * n;
   n = (NODE *)malloc(sizeof(NODE));
   n->tag = INT;
   n->f.value = v;
   return n;
}


NODE * newId(char * i)
{  NODE * n;
   char * cur = strdup(i);
   n = (NODE *)malloc(sizeof(NODE));
   n->tag = ID;
   n->f.id = cur;
   return n;
}

NODE * newNode(int tag)
{  NODE * n;
   n = (NODE *)malloc(sizeof(NODE));
   n->tag = tag;
   n->f.b.n1 = NULL;
   n->f.b.n2 = NULL;
   return n;
}

showTree(NODE * tree,int depth)
{  int i;
   if(tree==NULL)
    return;
   for(i=0;i<depth;i++)
    putchar('-');
   switch(tree->tag)
   {  case ID: printf("%s\n",tree->f.id);
               return;
      case INT: printf("%d\n",tree->f.value);
               return;
      default: printf("%s\n",showSymb(tree->tag));
               showTree(tree->f.b.n1,depth+1);
               showTree(tree->f.b.n2,depth+1);
   }
}

error(char * rule,char * message)
{  printf("%s: found %s\n",rule,showSymb(symb));
   printf("%s: %s\n",rule,message);
   exit(0);
}

NODE * program()
{  extern NODE * defs();
   extern NODE * commands();
   NODE * p;
   p = newNode(SEMI);
   char * name ;
   if(symb!=PROCEDURE)
      error("procedure", "expected procedure");
   
   lex();
   if(symb==ID){
      p->f.b.n1 = newId(yytext);
      name=(char *)malloc(strlen(yytext) * sizeof(char));
      name=yytext;
   }else{
      error("name", "name expected");
   }
   lex();
   if(symb!=IS)
      error("is", "is expected");
   
   lex();
   p->f.b.n2 = defs();
   if(symb!=TBEGIN)
      error("begin", "begin expected");
      
   lex();
   p->f.b.n3 = commands();
   if(symb!=END)
      error("end", "end expected");
  
   lex();
   if(symb!=ID)
	   error("ID", "name expected");
   if(strcmp(yytext, name)){
	   
   }else{
      error("name", "correct name expected");
   }
   lex();
   if(symb==SEMI){
   }else{
		error("SEMI", "; expected");
   }
   return p; 
}

NODE * defs()
{  extern NODE * def();
   NODE * d = newNode(SEMI);
   d = def();
   if(symb==SEMI)
   {  lex();
      if(symb==ID)
      {  NODE * d1;
         d1 = d;
         d = newNode(SEMI);
         d->f.b.n1 = d1;
         d->f.b.n2 = defs();
      }
   }
   else
    error("declarations","; expected");
   return d;
}

NODE * def()
{  NODE * d;
   if(symb!=ID)
    error("declaration","ID expected");
   lex();
   if(symb!=DEF_INT)
    error("declaration","declaration expected");
   d = newNode(ID);
   d->f.b.n1 = newId(yytext);
   lex();
   return d;
}

NODE * commands()
{  extern NODE * command();
   NODE * c;
   c = command();
   if(symb==SEMI)
   {  NODE * c1;
      c1 = c;
      c = newNode(SEMI);
      c->f.b.n1 = c1;
      lex();
      c->f.b.n2 = commands();
   }
   return c;
}

NODE * command()
{  extern NODE * assign();
   extern NODE * ifComm();
   extern NODE * forComm();
   switch(symb)
   {  case ID: return assign();
      case IF: lex();
               return ifComm();
      case FOR: lex();
                  return forComm();
      default: error("command","IF/FOR/identifier expected\n");
   }
}

// NODE * repeatComm()
// {  extern NODE * condexp(); 
//    NODE *c;
//    NODE *r;
//    c = command();
//    if (symb != UNTIL)
//     error("repeat","UNTIL expected\n");
//    r = newNode(REPEAT);
//    r->f.b.n1 = c;
//    lex();   
//    r->f.b.n2 = condexp();
//    return r;
// }

NODE * assign()
{  extern NODE * expr();
   NODE * a;
   NODE * i = newId(yytext);
   lex();
   if(symb!=ASSIGN)
    error("assign",":= expected\n");
   a = newNode(ASSIGN);
   a->f.b.n1 = i;
   lex();
   a->f.b.n2 = expr();
   return a;
}

NODE * ifComm()
{  extern NODE * condexp();
   NODE * c;
   NODE * t;
   c = newNode(IF);
   c->f.b.n1 = condexp();
   if(symb!=THEN)
    error("if","then expected\n");
   lex();
   t = command();
   lex();
   if(symb==ELSE)
   {  lex();
      c->f.b.n2 = newNode(ELSE);
      c->f.b.n2->f.b.n1 = t;
      c->f.b.n2->f.b.n2 = command();
   }
   else
    c->f.b.n2 = t;
   if(symb!=SEMI)
      error("SEMI", "; expected");
   lex();
   if(symb!=ENDIF)
      error("ENDIF", "end if expected");
   lex();
   return c;
}

 NODE * forComm()
 {  extern NODE * commands();
    NODE * f;
    NODE * n;
    f = newNode(FOR);
    if(symb!=ID)
      error("ID", "identifier expected");
    f->f.b.n1 = newId(yytext);
    lex();
    if(symb!=IN)
      error("IN", "IN expected");
    lex();
    if(symb!=INT)
      error("INT", "INT expected");
    lex();
    if(symb!=DOTS)
      error("DOTS", "DOTS expected");
    lex();
    if(symb!=INT)
      error("INT", "INT expected");
    lex();
    if(symb!=LOOP)
      error("LOOP", "LOOP expected");
    f->f.b.n1 = commands();
    if(symb!=ENDLOOP)
      error("ENDLOOP", "end loop expected");
    lex();
    return f;
 }

// NODE * whileComm()
// {  extern NODE * condexp();
//    NODE * w;
//    NODE * c;
//    c = condexp();
//    if(symb!=DO)
//     error("while","DO expected\n");
//    lex();
//    w = newNode(WHILE);
//    w->f.b.n1 = c;
//    w->f.b.n2 = command();
//    return w;
// }

// NODE * block()
// {  NODE * b;
//    b = newNode(TBEGIN);
//    b->f.b.n1 = program();
//    if(symb!=END)
//     error("block","END expected\n");
//    lex();
//    return b;
// }

// NODE * input()
// {  NODE * i;
//    i = newNode(INPUT);
//    if(symb!=ID)
//     error("input","identifier expected\n");
//    i->f.b.n1 = newId(yytext);
//    lex();
//    return i;
// }

// NODE * print()
// {  extern NODE * expr();
//    NODE * p;
//    p = newNode(PRINT);
//    p->f.b.n1 = expr();
//    return p;
// }

NODE * condexp()
{  extern NODE * expr();
   NODE * e;
   NODE * c;
   e = expr();
   switch(symb)
   {  case LT:
      case LTE:
      case EQ:
      case NEQ:
      case GTE:
      case GT: c = newNode(symb);
               c->f.b.n1 = e;
               lex();
               c->f.b.n2 = expr();
               return c;
      default: error("condexp","comparison operator expected\n");
   }
}

NODE * expr()
{  extern NODE * term();
   NODE * e;
   NODE * e1;
   e = term();
   while(symb==PLUS || symb==MINUS)
   {  e1 = e;
      e = newNode(symb);
      lex();
      e->f.b.n1 = e1;
      e->f.b.n2 = term();
   }
   return e;
}

NODE * term()
{  extern NODE * factor();
   NODE * t;
   NODE * t1;
   t = factor();
   while(symb==STAR || symb==SLASH)
   {  t1 = t;
      t = newNode(symb);
      lex();
      t->f.b.n1 = t1;
      t->f.b.n2 = term();
   }
   return t;
} 
 
NODE * factor()
{  extern NODE * base();
   NODE * f;
   if(symb==MINUS)
   {  lex();
      f = newNode(MINUS);
      f->f.b.n1 = base();
      return f;
   }
   return base();
}
  
NODE * base()
{  NODE * b;
   switch(symb)
   {  case ID: b = newId(yytext);
               break;
      case INT: b = newInt(atoi(yytext));
                break;
      case LBRA: lex();
                 b = expr();
                 if(symb!=RBRA)
                  error("base","( expected\n");
                 break;
      default: error("base","(, identifier or integer expected\n");
   }
   lex();
   return b;
}
