/****************************************************/
/* File: cgen.c                                     */
/* The code generator implementation                */
/* for the TINY compiler                            */
/* (generates code for the TM machine)              */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "cgen.h"

/* tmpOffset is the memory offset for temps
   It is decremented each time a temp is
   stored, and incremeted when loaded again
*/
static int tmpOffset = 0;

int countreg = 1;
int countL = 1;
int invert = 0;
int tab = 0;

/* prototype for internal recursive code generator */
static void cGen (TreeNode * tree);

static void tabfunc ();

/* Procedure genStmt generates code at a statement node */
static void genStmt( TreeNode * tree)
{ TreeNode * p1, * p2, * p3;
  int savedLoc1,savedLoc2,currentLoc;
  int loc;
  
  switch (tree->kind.stmt) {
      case FuncDeclK:
         tab+=2;
         fprintf(code, "%s:\n",tree->attr.name); tabfunc();
         cGen(tree->child[0]);
         cGen(tree->child[1]);
         tab-=2;
         fprintf(code, "\n\n");
         break;

      case VarDeclK:
         cGen(tree->child[0]);
         break;

      case AssignK:
         switch (tree->child[1]->nodekind) {
            case ExpK:

               cGen(tree->child[0]);
               cGen(tree->child[1]);


               fprintf(code, "%s", tree->child[0]->attr.name);

               if(tree->child[0]->kind.exp == IdK){
                  p3 = tree->child[0]->child[0];
                  if(p3 != NULL){
                     if(p3->kind.exp == ConstK) fprintf(code, "[%d]", p3->attr.val);
                     else if (p3->kind.exp == IdK ) fprintf(code, "[%s]", p3->attr.name);
                     else fprintf(code, "[ELSE]");
                  }
               }

               fprintf(code, " =");

               if(tree->child[1]->kind.exp == ConstK) fprintf(code, " %d", tree->child[1]->attr.val);
               else if (tree->child[1]->kind.exp == IdK) fprintf(code, " %s", tree->child[1]->attr.name);
               else if (tree->child[1]->kind.exp == OpK) fprintf(code, " t%d", countreg-1);
               

               if(tree->child[1]->kind.exp == IdK){
                  p3 = tree->child[1]->child[0];
                  if(p3 != NULL){
                     if(p3->kind.exp == ConstK) fprintf(code, "[%d]", p3->attr.val);
                     else if (p3->kind.exp == IdK ) fprintf(code, "[%s]", p3->attr.name);
                     else fprintf(code, "[ELSE]");
                  }
               }
               break;

            case StmtK:
               cGen(tree->child[1]);
               fprintf(code, "%s = t%d", tree->child[0]->attr.name, countreg-1);
               break;
            default:
               break;
         }

         fprintf(code, "\n"); tabfunc();
         //tab--;
         break;

      case RetK:
         cGen(tree->child[0]);

         fprintf(code, "return ");

         if(tree->child[0]->kind.exp == ConstK) fprintf(code, " %d", tree->child[0]->attr.val);
         else if (tree->child[0]->kind.exp == IdK ) fprintf(code, " %s", tree->child[0]->attr.name);
         else fprintf(code, " t%d", countreg-1);


         fprintf(code, "\n"); tabfunc();
         break;

      case ActivK:
         fprintf(code,"");
         TreeNode * param;
         int countparam = 0;
         param = tree->child[0];

         while(param != NULL){
            countparam++;
            if(param->kind.exp == ConstK) {fprintf(code, "param %d\n", param->attr.val); tabfunc();}
            else if (param->kind.exp == IdK) {fprintf(code, "param %s\n", param->attr.name); tabfunc();}
            else if (param->kind.exp == OpK) {
               cGen(param);
               fprintf(code, "param t%d\n", countreg-1); tabfunc();
            }
            else if (param->kind.stmt == ActivK) {
               cGen(param);
               fprintf(code, "param t%d\n", countreg-1); tabfunc();
            }
            param = param->sibling;                  
         }
         countreg++;
         fprintf(code, "t%d = ", countreg-1);
         fprintf(code, "call %s, %d\n", tree->attr.name, countparam); tabfunc();
         break;

      case IfK :
         fprintf(code,"");
         int countLlocalif = countL;
         countL+=2;
         cGen(tree->child[0]);
         fprintf(code,"if_true t%d goto L%d\n", countreg-1, countLlocalif); tabfunc();
         cGen(tree->child[2]);
         fprintf(code, "goto L%d\n", countLlocalif+1); tabfunc();
         fprintf(code, "\n\tL%d:\n", countLlocalif); tabfunc();
         cGen(tree->child[1]);
         fprintf(code, "\n\tL%d:\n", countLlocalif+1); tabfunc();
         break; /* if_k */

      case RepeatK:
         invert = 1;
         fprintf(code,"");
         int countLlocalrepeat = countL;
         countL+=2;
         fprintf(code, "\n\tL%d:\n", countLlocalrepeat); tabfunc();
         cGen(tree->child[0]);
         fprintf(code,"if_true t%d goto L%d\n", countreg-1, countLlocalrepeat+1); tabfunc();
         cGen(tree->child[1]);
         fprintf(code, "goto L%d\n", countLlocalrepeat); tabfunc();
         fprintf(code, "\n\tL%d:\n", countLlocalrepeat+1); tabfunc();
         break; /* repeat */

      default:
         cGen(tree->child[0]);
         break;
    }
} /* genStmt */

/* Procedure genExp generates code at an expression node */
static void genExp( TreeNode * tree)
{ int loc;
  TreeNode * p1, * p2, * p3;
  switch (tree->kind.exp) {

    case ConstK :
      break; /* ConstK */
    
    case IdK :
      break; /* IdK */

    case OpK :
         p1 = tree->child[0];
         p2 = tree->child[1];

         genExp(p1);
         genExp(p2);

         fprintf(code,"t%d =", countreg);

         if(p1->kind.exp == ConstK) fprintf(code, " %d", p1->attr.val);
         else if (p1->kind.exp == IdK ) fprintf(code, " %s", p1->attr.name);
         else {
            if(p2->kind.exp == OpK) fprintf(code, " t%d", countreg-2);
            else fprintf(code, " t%d", countreg-1);
         }

         if(p1->kind.exp == IdK){
            p3 = p1->child[0];
            if(p3 != NULL) {
               if(p3->kind.exp == ConstK) fprintf(code, "[%d]", p3->attr.val);
               else if (p3->kind.exp == IdK ) fprintf(code, "[%s]", p3->attr.name);
               else fprintf(code, "[ELSE]");
            }
         }

         switch (tree->attr.op)
         {
            //case EQ: fprintf(code," ="); break;
            case LT: if(invert==0) fprintf(code," <"); else fprintf(code," >="); break;
            case GT: if(invert==0) fprintf(code," >"); else fprintf(code," <="); break;
            case GEQ: if(invert==0) fprintf(code," >="); else fprintf(code," <"); break;
            case LEQ: if(invert==0) fprintf(code," <="); else fprintf(code," >"); break;
            case EQEQ: if(invert==0) fprintf(code," =="); else fprintf(code," !="); break;
            case INEQ: if(invert==0) fprintf(code," !="); else fprintf(code," =="); break;
            case PLUS: fprintf(code," +"); break;
            case COMMA: fprintf(code," ,"); break;
            case MINUS: fprintf(code," -"); break;
            case TIMES: fprintf(code," *"); break;
            case OVER: fprintf(code," /"); break;
            // case LPAREN: fprintf(code," ("); break;
            // case RPAREN: fprintf(code," )"); break;
            // case LBRACKETS: fprintf(code," ["); break;
            // case RBRACKETS: fprintf(code," ]"); break;
            // case LCBRACES: fprintf(code," {"); break;
            // case RCBRACES: fprintf(code," }"); break;
            // case SEMI: fprintf(code," ;"); break;
            default:
               break;
         }


         
         if(p2->kind.exp == ConstK) fprintf(code, " %d", p2->attr.val);
         else if (p2->kind.exp == IdK ) fprintf(code, " %s", p2->attr.name);
         else fprintf(code, " t%d", countreg-1);

         if(p2->kind.exp == IdK){
            p3 = p2->child[0];
            if(p3 != NULL) {
               if(p3->kind.exp == ConstK) fprintf(code, "[%d]", p3->attr.val);
               else if (p3->kind.exp == IdK ) fprintf(code, "[%s]", p3->attr.name);
               else fprintf(code, "[ELSE]");
            }
         }


         fprintf(code, " \n"); tabfunc();

         countreg++;
         // cGen(p1);
         // //if(p1->kind.exp == 0) fprintf(code, "OPPPPP");
         // //else fprintf(code, "COOONST");
         // /* gen code to push left operand */
         // //emitRM("ST",ac,tmpOffset--,mp,"op: push left");
         // switch (tree->attr.op) {
         //    case PLUS :
         //       fprintf(code, "+");
         //       break;
         //    case MINUS :
         //       //emitRO("SUB",ac,ac1,ac,"op -");
         //       fprintf(code, "-");
         //       break;
         //    case TIMES :
         //       //emitRO("MUL",ac,ac1,ac,"op *");
         //       fprintf(code, "*");
         //       break;
         //    case OVER :
         //       //emitRO("DIV",ac,ac1,ac,"op /");
         //       fprintf(code, "/");
         //       break;
         //    case LT :
         //       fprintf(code, "<");
         //       // emitRO("SUB",ac,ac1,ac,"op <") ;
         //       // emitRM("JLT",ac,2,pc,"br if true") ;
         //       // emitRM("LDC",ac,0,ac,"false case") ;
         //       // emitRM("LDA",pc,1,pc,"unconditional jmp") ;
         //       // emitRM("LDC",ac,1,ac,"true case") ;
         //       break;
         //    case EQ :
         //       fprintf(code, "==");
         //       // emitRO("SUB",ac,ac1,ac,"op ==") ;
         //       // emitRM("JEQ",ac,2,pc,"br if true");
         //       // emitRM("LDC",ac,0,ac,"false case") ;
         //       // emitRM("LDA",pc,1,pc,"unconditional jmp") ;
         //       // emitRM("LDC",ac,1,ac,"true case") ;
         //       break;
         //    default:
         //       emitComment("BUG: Unknown operator");
         //       break;
         // } /* case op */
         // /* gen code for ac = right operand */
         // cGen(p2);
         // /* now load left operand */
         // //emitRM("LD",ac1,++tmpOffset,mp,"op: load left");
         // //if (TraceCode)  emitComment("<- Op") ;
         // countreg--;

         break; /* OpK */

    default:
      break;
  }
} /* genExp */

/* Procedure cGen recursively generates code by
 * tree traversal
 */
static void cGen( TreeNode * tree)
{ if (tree != NULL)
  { switch (tree->nodekind) {
      case StmtK:
        genStmt(tree);
        break;
      case ExpK:
        genExp(tree);
        break;
      default:
        break;
    }
    
    invert = 0;
    cGen(tree->sibling);
  }
}

static void tabfunc (){
   int i;
   for(i = 0; i < tab; i++){
      fprintf(code, "\t");
   }
}

/**********************************************/
/* the primary function of the code generator */
/**********************************************/
/* Procedure codeGen generates code to a code
 * file by traversal of the syntax tree. The
 * second parameter (codefile) is the file name
 * of the code file, and is used to print the
 * file name as a comment in the code file
 */
void codeGen(TreeNode * syntaxTree, char * codefile)
{  char * s = malloc(strlen(codefile)+7);
   strcpy(s,"File: ");
   strcat(s,codefile);
   fprintf(code, "* C- Compilation to Intermediate Code\n");
   fprintf(code,"* %s\n",s);
   fprintf(code, "____________________________________________________________\n\n");
   
   cGen(syntaxTree);
   //tabfunc(); fprintf(code, "halt");
   
   /* finish */
   fprintf(code, "____________________________________________________________\n");
   fprintf(code, "\n* End of execution.");
}
