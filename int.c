#include <stdio.h>
#include <stdlib.h>

#include "tokens.h"
#include "nodes.h"

#define TRACE 0
#define TRACESTORE 0
#define MAXSTORE 100

extern int symb;

extern printSymb();
extern int yylex(void);
extern FILE* yyin;
extern NODE * program(void);
extern prettytree(NODE *,int);
extern char * showSymb(int);

struct variable {char * id;int value;};
struct variable store[MAXSTORE];
int sb,sp;

trace(NODE * t,char * message)
{  if(TRACE)
   {  printf("%s\n",message);
      prettytree(t,0);
      putchar('\n');
   }
}

interror(NODE * tree,char * message)
{  prettytree(tree,0);
   printf(" %s\n",message);
   exit(0);
}

showstore()
{  int i;
   printf("sb: %d, sp: %d\n",sb,sp);
   for(i=sp-1;i>=0;i--)
    printf("%d. %s: %d\n",i,store[i].id,store[i].value);
}

tracestore()
{  if(TRACESTORE)
    showstore();
}

findvar(char * id)
{  int i;
   for(i=sp-1;i>=0;i--)
    if(strcmp(store[i].id,id)==0)
     return i;
   return -1;
}

checkvar(char * id)
{  int i;
   for(i=sp-1;i>=sb;i--)
    if(strcmp(store[i].id,id)==0)
     return i;
   return -1;
}

intprog(NODE * t)
{  int sb1;
   trace(t,"program");
   if(t->f.b.n1==NULL)
    inttree(t->f.b.n2);
   else
   {  sb1 = sb;
      sb = sp;
      tracestore();
      inttree(t->f.b.n1);
      inttree(t->f.b.n2);
      sp = sb;
      sb = sb1;
      tracestore();
   }
}

intvar(NODE * t)
{  trace(t,"var");
   if(sp==MAXSTORE)
    interror(NULL,"too many variables");
   if(checkvar(t->f.b.n1->f.id)!=-1)
    interror(t,"declared already");
   store[sp].id = t->f.b.n1->f.id;
   sp++;
   tracestore();
}

intassign(NODE * t)
{  int addr;
   trace(t,"assign");
   addr = findvar(t->f.b.n1->f.id);
   if(addr==-1)
    interror(t->f.b.n1,"not declared");
   store[addr].value = intexp(t->f.b.n2);
   tracestore();
}

intif(NODE * t)
{  int c;
   trace(t,"if");
   c = intexp(t->f.b.n1);
   if(t->f.b.n2->tag==ELSE)
   {  if(c)
       inttree(t->f.b.n2->f.b.n1);
      else
       inttree(t->f.b.n2->f.b.n2);
   }
   else
   if(c)
    inttree(t->f.b.n2);
}

intwhile(NODE * t)
{  trace(t,"while");
   while(intexp(t->f.b.n1))
   inttree(t->f.b.n2);
}

intblock(NODE * t)
{  intprog(t->f.b.n1);
}

intinput(NODE * t)
{  int addr;
   trace(t,"input");
   addr = findvar(t->f.b.n1->f.id);
   if(addr==-1)
    interror(t->f.b.n1,"not declared");
   printf("INPUT> ");
   scanf("%d",&store[addr].value);
   tracestore();
}

intprint(NODE * t)
{  trace(t,"print");
   printf("%d\n",intexp(t->f.b.n1));
}

inttree(NODE * t)
{  switch(t->tag)
   {  case SEMI: inttree(t->f.b.n1);
                 inttree(t->f.b.n2);
                 return;
      case VAR: intvar(t);
                return;
      case ASSIGN: intassign(t);
                   return; 
      case IF: intif(t);
               return;
      case WHILE: intwhile(t);
                  return;
      case TBEGIN: intblock(t);
                  return;
      case INPUT: intinput(t);
                  return;
      case PRINT: intprint(t);
                  return;
      default: printf("unknown node: %s\n",showSymb(t->tag));
               exit(0);
   }
}

intexp(NODE * e)
{  int addr;
   int e1,e2;
   trace(e,"exp");
   switch(e->tag)
   {  case ID: addr = findvar(e->f.id);
               if(addr==-1)
                interror(e,"not declared");
               return store[addr].value;
      case INT: return e->f.value;
   }
   e1 = intexp(e->f.b.n1);
   if(e->tag==MINUS && e->f.b.n2==NULL)
    return -e1;
   e2 = intexp(e->f.b.n2);
   switch(e->tag)
   {  case PLUS: return e1+e2;
      case MINUS: return e1-e2;
      case STAR: return e1*e2;
      case SLASH: if(e2==0)
                   interror(e,"divide by 0");
      case LT: return e1<e2;
      case LTE: return e1<=e2;
      case EQ: return e1==e2;
      case NEQ: return e1!=e2;
      case GTE: return e1>=e2;
      case GT: return e1>e2;
      default: interror(e,"unknown operator");
   }
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
   sb = 0;
   sp = 0;
   intprog(tree);
   fclose(yyin);
}
