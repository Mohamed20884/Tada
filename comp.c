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
extern int * ParameterCount;
extern int * paramters;
extern int * functionCount;

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
{  
   codeblock(t);
   
}

codeprogram(NODE * t)
{
    printf("\tAREA ASMain,CODE\n");
    printf("__main\tPROC\n");
    printf("\tEXPORT __main\n");
	codetree(t);
    printf("\tPLOOP	B PLOOP\n");
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

codefor(NODE * t)
{  int ln;
   ln = labno;
   labno++;
   //codeexp(0,t->f.b.n1);
   //char * i;
   //i = t->f.b.n1->f.b.n2->f.value;
   
   codevar(t->f.b.n1);
   
   codetree(t->f.b.n1);
   //codeassign(t->f.b.n1);
   //codeassign(t->f.b.n2);
   
   printf("FLOOP%d\n",ln);
   codeexp(0,t->f.b.n2);
   //printf("\tMOV R%d,#0x1\n",0);	//,t->f.b.n2->f.b.n1->f.value);
   //printf("\tMOV R%d,#0x10\n",10);;
   //printf("\tCMP R%d,R%d\n",0,10);
   if(t->f.b.n2->tag==LT)
   printf("\tBGE FEND%d\n",ln); 
   if(t->f.b.n2->tag==GT)
   printf("\tBLE FEND%d\n",ln); 
   codetree(t->f.b.n3);
   
   
   //printf("\tADD R%d,#0x%x\n",10,1);
   printf("\tB FLOOP%d\n",ln);
   printf("FEND%d\n",ln);
      //codetree(t->f.b.n2->f.b.n2);
   
}

/*int codeexp(int RD,NODE * e)
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
}*/


codeblock(NODE * t)
{  int rb1;
   rb1 = rb;
   rb = rp;
   codetree(t->f.b.n1);
   codetree(t->f.b.n2);
   codetree(t->f.b.n3);
   codefunction(t->f.b.n4);
   rp = rb;
   rb = rb1;
}

codefunctioncall(NODE * t)
{
	int i = 0;
	NODE * temp;
	temp = t->f.b.n2;
	while(temp!=NULL)
	{
		printf("\tPUSH{R%d}\n",findvar(temp->f.b.n1->f.b.n1->f.id));
		temp = temp->f.b.n2;
		
	}
   //if(findvar(t->f.b.n2->f.b.n2->f.b.n2->f.b.n1->f.b.n1->f.id)!=-1)
   //printf("\tPUSH{R%d}\n",findvar(t->f.b.n2->f.b.n2->f.b.n2->f.b.n1->f.b.n1->f.id));
   printf("\tBL %s\n",t->f.b.n1->f.id);
   
   //codeblock(t);

	//printf("test %s", t->f.b.n1->f.id);
	//codevar(t->f.b.n1->f.id);

}


codefunction(NODE * t)
{
   int count = 0; 
   int a[3];
   int i = 0;
   int rb1;
   rb1 = rb;
   rb = rp;
   NODE * test = t;
   NODE * temp = t->f.b.n2;
   while(test != NULL)
   {
	   count += 1;
	   test = test->f.b.n2;
   }
   count -= 1;
   //count
   printf("%s\n",t->f.b.n1->f.id); 
   codetree(t->f.b.n1);
   codetree(t->f.b.n2);
   codetree(t->f.b.n3);
   while(temp!=NULL)
   {
	   a[i] = findvar(temp->f.b.n1->f.b.n1->f.id);
	   temp = temp->f.b.n2;
	   i++;
   }
   //codetree(t->f.b.n3);
   int k = i-1;
   for (k=i-1; k>=0;k--)
   {
	   printf("\tPOP{R%d}\n",a[k]);
   }
   printf("\tPUSH{LR}\n");
   codetree(t->f.b.n4);
   printf("\tPOP{LR}\n");
   printf("\tBX LR\n");
   rp = rb;
   rb = rb1;
   
	
}



codetree(NODE * t)
{  if(t==NULL)
    return;
   switch(t->tag)
   {  
	  case PROCEDURE: codetree(t->f.b.n1);
					  codetree(t->f.b.n2);
					  codetree(t->f.b.n3);
					  codetree(t->f.b.n4);
	  case PROCEDURES: codetree(t->f.b.n1);
					 return;	
	  case CALLFUNCTION:  codefunctioncall(t);
					  return;
	  case FUNCTION: codefunction(t); return;
      //~ case SEMI: codetree(t->f.b.n1);
                 //~ codetree(t->f.b.n2);
                 //~ return;
      case COMMANDS:
					codetree(t->f.b.n1);
					codetree(t->f.b.n2);
					 
                 return;
      case ASSIGN: codeassign(t);
                   showSource(t);
                   return; 
      case IF: codeif(t);
               return;
      case FOR: codefor(t);
				return;
      case DEFS://showTree(t,0) ;
				if(t->f.b.n1 != NULL){
				 codetree(t->f.b.n1);}
				if(t->f.b.n2 != NULL){
				 codetree(t->f.b.n2);}
				 
				return;
	  case DEF: codevar(t);
				return;
	  case PARAMS://showTree(t,0) ;
				if(t->f.b.n1 != NULL){
				 codetree(t->f.b.n1);}
				if(t->f.b.n2 != NULL){
				 codetree(t->f.b.n2);}
				 
				return;
	  case PARAM: codevar(t);
				return;
	  case DEF_INT: 
				return;
      case ID: 
				return;
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
   showTree(tree, 0);
   rb = 0;
   rp = 0;
   labno = 0;
   codeprogram(tree);
   fclose(yyin);
}
