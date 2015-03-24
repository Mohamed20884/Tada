#include <stdio.h>
#include <stdlib.h>

#include "tokens.h"
#include "nodes.h"


extern int symb;
extern int yylex(void);
extern NODE * program(void);
extern FILE* yyin;
extern int ch;
extern char * showSymb(int);
extern prettytree(NODE *,int);

#define MAXREG 10
#define E1 MAXREG+1
#define E2 MAXREG+2

char * registers[MAXREG];
int rp;
int rb;

int labno;

isComp(int c)
{  switch(c)
   {  case LT: case LTE: case EQ:
      case GTE: case GT: case NEQ: return 1;
      default: return 0;
   }
}

char * notComp(int c)
{  switch(c)
   {  case LT: return "GE";
      case LTE: return "GT";
      case GTE: return "LT";
      case GT: return "LE";
      case EQ: return "NE";
      case NEQ: return "EQ";
      default: printf("code: unknown operator %s\n",showSymb(c));
               exit(0);
   }
}

char * showCode(int c)
{  switch(c)
   {  case PLUS: return "ADD";
      case MINUS: return "SUB";
      case STAR: return "MUL";
      case SLASH: return "UDIV";
      default: printf("code: unknown operator %s\n",showSymb(c));
               exit(0);
   }
}

showSource(NODE * t)
{  printf("; ");
   prettytree(t,0);
   putchar('\n');
}

checkvar(char * id)
{  int i;
   for(i=rp-1;i>=rb;i--)
    if(strcmp(id,registers[i])==0)
     return i+1;
   return -1;
}

findvar(char * id)
{  int i;
   for(i=rp-1;i>=0;i--)
    if(strcmp(id,registers[i])==0)
     return i+1;
   return -1;
}


codeerror(NODE * tree,char * message)
{  prettytree(tree,0);
   printf(" %s\n",message);
   exit(0);
}

codeprog(NODE * t)
{  printf("\tAREA ASMain,CODE\n");
   printf("__main\tPROC\n");
   printf("\tEXPORT __main\n");
   codeblock(t);
   printf("PLOOP\tB PLOOP\n");
   printf("\tENDP\n");
   printf("\tEND\n");  
}

codevar(NODE * t)
{  if(rp==MAXREG)
    codeerror(NULL,"too many variables");
   if(checkvar(t->f.b.n1->f.id)!=-1)
    codeerror(t,"declared already");
   registers[rp] = t->f.b.n1->f.id;
   printf("%s\tRN %d\n",registers[rp],rp+1);
   rp++;
}

codeassign(NODE * t)
{  int reg, ereg;
   reg = findvar(t->f.b.n1->f.id);
   if(reg==-1)
    codeerror(t->f.b.n1,"not declared");
    // if it is just an identifier then we need to handle this special case.
   if(t->f.b.n2->tag==ID)
   {  ereg = findvar(t->f.b.n2->f.id);
      if(ereg==-1)
       codeerror(t->f.b.n2,"not declared");
      printf("\tMOV R%d,R%d\n",reg,ereg);
   }
   else 
    codeexp(reg,t->f.b.n2);
   
}


codeif(NODE * t)
{  int ln;
   ln = labno;
   labno++;
   codeexp(0,t->f.b.n1);
   if(t->f.b.n2->tag==ELSE)
   {  printf("\tB%s IFALSE%d\n",notComp(t->f.b.n1->tag),ln);
      codetree(t->f.b.n2->f.b.n1);
      printf("\tB IFEND%d\n",ln);
      printf("IFALSE%d\n",ln);
      codetree(t->f.b.n2->f.b.n2);
   }
   else
   {  printf("\tB%s IFEND%d\n",notComp(t->f.b.n1->tag),ln);
      codetree(t->f.b.n2);
   }
   printf("IFEND%d\n",ln);
}



codeblock(NODE * t)
{  int rb1;
   rb1 = rb;
   rb = rp;
   codetree(t->f.b.n1);
   codetree(t->f.b.n2);
   rp = rb;
   rb = rb1;
}
  
codetree(NODE * t)
{  if(t==NULL)
    return;
   switch(t->tag)
   {  
      case SEMI: codetree(t->f.b.n1);
                 codetree(t->f.b.n2);
                 return;
      case ASSIGN: codeassign(t);
                   showSource(t);
                   return; 
      case IF: codeif(t);
               return;
      case PROCEDURE:
                  return;
      case ID: return;
      case IS: codeblock(t->f.b.n1);
      case TBEGIN: codeblock(t->f.b.n2);
                  return;
      default: codeerror(t,"unknown construct");
               exit(0);
   }
}

int need_push(NODE * t){
  switch(t->tag)
   {  case ID: return 0;
      case INT: return 0;
   }
   return 1;
}

int codeexp(int RD,NODE * e)
{  int reg;
    int RE1;
    int RE2;
   switch(e->tag)
   {  case ID: reg = findvar(e->f.id);
               if(reg==-1)
                codeerror(e,"not declared");
               return reg;
      case INT: printf("\tMOV R%d,#0x%x\n",RD,e->f.value);
                return RD;
   }
   RE1 = codeexp(E1,e->f.b.n1);
   // only PUSH if actually register changed
   if(RE1 == E1 && need_push(e->f.b.n2)){
     printf("\tPUSH {R%d}\n",E1);
     RE2 = codeexp(E2,e->f.b.n2);
     printf("\tPOP {R%d}\n",E1);
   }else{
      RE2 = codeexp(E2,e->f.b.n2);
   }
   if(isComp(e->tag))
     printf("\tCMP R%d,R%d\n",RE1,RE2);
   else
     printf("\t%s R%d,R%d,R%d\n",showCode(e->tag),RD,RE1,RE2);
   showSource(e);
   return RD;
}

main(int c,char ** argv)
{

   NODE * tree;
   if((yyin=fopen(argv[1],"r"))==NULL){  
      printf("can't open %s\n",argv[1]);
      exit(0);
   }
   symb = yylex();
   tree = program();
   rb = 0;
   rp = 0;
   labno = 0;
   codeprog(tree);
   fclose(yyin);
}
