#include <stdio.h>
#include <stdlib.h>

#include "tokens.h"
#include "nodes.h"

extern char * showSymb(int);

spaces(int n)
{  while(n>0)
   {  putchar(' ');
      n--;
   }
}

prettyseq(NODE * t,int d)
{  if(t->f.b.n1!=NULL)
   {  prettytree(t->f.b.n1,d);
      printf(";\n");
   }
   prettytree(t->f.b.n2,d);
}

prettyvar(NODE * t,int d)
{  spaces(d);
   printf("VAR %s",t->f.b.n1->f.id);
}

prettyassign(NODE * t,int d)
{  spaces(d);
   printf("%s := ",t->f.b.n1->f.id);
   prettytree(t->f.b.n2);
}

prettyif(NODE * t,int d)
{  spaces(d);
   printf("IF ");
   prettytree(t->f.b.n1);
   printf(" THEN\n");
   if(t->f.b.n2->tag==ELSE)
   {  prettytree(t->f.b.n2->f.b.n1,d);
      putchar('\n');
      spaces(d);
      printf("ELSE\n");
       prettytree(t->f.b.n2->f.b.n2,d);
   }
   else
    prettytree(t->f.b.n2,d);
}

prettywhile(NODE * t,int d)
{  spaces(d);
   printf("WHILE ");
   prettytree(t->f.b.n1,d);
   printf(" DO\n");
   spaces(d);
   prettytree(t->f.b.n2,d);
}

prettyrepeat(NODE * t,int d)
{  spaces(d);
   printf("REPEAT\n");
   spaces(d);
   prettytree(t->f.b.n1,d);
   printf("\n");
   spaces(d);
   printf("UNTIL ");
   prettytree(t->f.b.n2,d);
   printf("\n");
   
}

prettyblock(NODE * t,int d)
{  spaces(d);
   printf("BEGIN\n");
   prettytree(t->f.b.n1,d+5);
   putchar('\n');
   spaces(d);
   printf("END");
}

prettyinput(NODE * t,int d)
{  spaces(d);
   printf("INPUT ");
   prettytree(t->f.b.n1);
}

prettyprint(NODE * t,int d)
{  spaces(d);
   printf("PRINT ");
   prettytree(t->f.b.n1,d);
}

prettydef(NODE * t, int d)
{  spaces(d);
   printf("DEFS ");
   prettytree(t->f.b.n1,d);
}

prettyfor(NODE * t,int d)
{  spaces(d);
   printf("FOR ");
   prettytree(t->f.b.n1);
   printf(" THEN\n");
   if(t->f.b.n2->tag==ELSE)
   {  prettytree(t->f.b.n2->f.b.n1,d);
      putchar('\n');
      spaces(d);
      printf("ELSE\n");
       prettytree(t->f.b.n2->f.b.n2,d);
   }
   else
    prettytree(t->f.b.n2,d);
}

prettycommands(NODE * t, int d)
{
	spaces(d);
	printf("COMMANDS ");
	switch(t->tag)
	{
		
		
	}
}


prettytree(NODE * t,int d)
{  switch(t->tag)
   {  case SEMI: prettyseq(t,d);
                 return;
      case DEFS: prettydef(t,d);
				 return;
      case ASSIGN: prettyassign(t,d);
                   return; 
      case COMMANDS: prettycommands(t,d);
               return;
      case ID: printf("%s",t->f.id);
               return;
      case INT: printf("%d",t->f.value);
                return;
      case MINUS: if(t->f.b.n2==NULL)
                  {  putchar('-');
                     prettybra(t->f.b.n1,0);
                     return;
                  }
      case PLUS: case STAR: case SLASH: case LT:
      case LTE: case EQ: case NEQ: case GTE:
      case GT:
               prettybra(t->f.b.n1);
               printf("%s",showSymb(t->tag));
               prettybra(t->f.b.n2);
               return;
      default: printf("unknown node: %s\n",showSymb(t->tag));
               exit(0);
   }
}

prettybra(NODE * e)
{  switch(e->tag)
   {  case ID:
      case INT: prettytree(e,0);
                return;
      default: putchar('(');
               prettytree(e,0);
               putchar(')');
   }
}
