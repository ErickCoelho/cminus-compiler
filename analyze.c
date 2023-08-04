/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"

/* counter for variable memory locations */
static int location = 0;
static char * escopo = "global";

/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{ if (t != NULL)
  { preProc(t);
    if(t->kind.stmt == FuncDeclK) escopo = t->attr.name;
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    postProc(t);
    if(t->kind.stmt == FuncDeclK) escopo = "global";
    traverse(t->sibling,preProc,postProc);
  }
}

/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t)
{ if (t==NULL) return;
  else return;
}

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
static void insertDecl( TreeNode * t)
{ switch (t->nodekind)
  { case StmtK:
      switch (t->kind.stmt){
        case VarDeclK:
          t->decl = 1;
          break;
        case FuncDeclK:
          t->decl = 2;
          break;
        case ArrDeclK:
          t->decl = 3;
          break;
        default:
          break;
      }
      break;
    case ExpK:
      switch (t->kind.exp)
      { 
        default:
          break;
      }
      break;
    default:
      break;
  }
}

static void semanticError(TreeNode * t, char * message)
{ fprintf(listing,"Erro semantico na linha %d: %s\n",t->lineno,message);
  Error = TRUE;
}

static void insertType( TreeNode * t)
{ switch (t->nodekind)
  { case StmtK:
      switch (t->kind.stmt){
        case TypeK:
          if (strcmp(t->attr.name, "void") == 0) {
            t->child[0]->type = Void;
          } else if (strcmp(t->attr.name, "int") == 0) {
            t->child[0]->type = Integer;
          }
          break;
        default:
          break;
      }
      break;
    case ExpK:
      switch (t->kind.exp)
      { 
        default:
          break;
      }
      break;
    default:
      break;
  }
}

static void insertNode( TreeNode * t)
{ switch (t->nodekind)
  { case StmtK:
      switch (t->kind.stmt)
      { case AssignK:
          if(t->child[1]->nodekind == StmtK){
            if(t->child[1]->kind.stmt == ActivK){
              BucketList l1 = NULL;
              BucketList l2 = NULL;
              l1 = findbucketlist(t->child[0]->attr.name, escopo);
              l2 = findbucketlist(t->child[1]->attr.name, escopo);
              if(l1 != NULL && l2 != NULL && strcmp(t->child[0]->attr.name, "input") && strcmp(t->child[1]->attr.name, "input") && strcmp(t->child[0]->attr.name, "output") && strcmp(t->child[1]->attr.name, "output")){
                if(l1->type != l2->type){
                  fprintf(listing,"Erro semantico na linha %d: atribuição errada, %s e %s são de tipos diferentes\n",t->lineno, t->child[0]->attr.name, t->child[1]->attr.name);
                  Error = TRUE;
                }
              }
            }
          }
          if (st_lookup(t->child[0]->attr.name, escopo) == -1)
            semanticError(t->child[0], "variável não declarada");
          st_insert(0,t->attr.name,t->lineno,escopo,location++,t->decl,t->type);
          break;
        case VarDeclK:
          if (st_lookup(t->attr.name, escopo) == -1) {
          /* not yet in table, so treat as new definition */
            st_insert(0,t->attr.name,t->lineno,escopo,location++,t->decl,t->type);
          }

          else{
          /* already in table, so ignore location, 
             add line number of use only */ 
            fprintf(listing,"");
            BucketList l;
            l = findbucketlist(t->attr.name, escopo);
            if(l->decl == 2) semanticError(t, "nome da variavel ja foi usado como nome de função");
            else semanticError(t, "variavel ja declarada anteriormente");
            st_insert(0,t->attr.name,t->lineno,escopo,0,t->decl,t->type);
          }
          break;
        case ActivK:
          //st_insert(0,t->attr.name,t->lineno,escopo,location++,t->decl,t->type);
          if (strcmp(t->attr.name, "input") == 0 || strcmp(t->attr.name, "output") == 0) {break;}
          if (st_lookup(t->attr.name, escopo) == -1){
            semanticError(t, "chamada de função não declarada");
            break;
          }
          if(paramcomp(t->attr.name, escopo, paramcount(t)) == 0){
            semanticError(t, "quantidade de parâmetros incompatível");
            break;
          }
          break;
        case FuncDeclK:
          if (st_lookup(t->attr.name, escopo) == -1){
          /* not yet in table, so treat as new definition */
            st_insert(paramcount(t),t->attr.name,t->lineno,escopo,location++,t->decl,t->type);}
          else
          /* already in table, so ignore location, 
             add line number of use only */ 
            
            semanticError(t, "funcao ja declarada anteriormente");
            st_insert(0,t->attr.name,t->lineno,escopo,0,t->decl, t->type);
          break;
        case ArrDeclK:
          if (st_lookup(t->attr.name, escopo) == -1)
          /* not yet in table, so treat as new definition */
            st_insert(0,t->attr.name,t->lineno,escopo,location++,t->decl,t->type);
          else{
          /* already in table, so ignore location, 
             add line number of use only */ 
            fprintf(listing,"");
            BucketList l;
            l = findbucketlist(t->attr.name, escopo);
            if(l->decl == 2) semanticError(t, "nome da variavel ja foi usado como nome de função");
            else semanticError(t, "variavel ja declarada anteriormente");
            st_insert(0,t->attr.name,t->lineno,escopo,0,t->decl,t->type);
          }
          break;
        default:
          break;
      }
      break;
    case ExpK:
      switch (t->kind.exp)
      { case IdK:
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree)
{ 
  traverse(syntaxTree,insertDecl,nullProc);
  traverse(syntaxTree,insertType,nullProc);
  traverse(syntaxTree,insertNode,nullProc);
  if (TraceAnalyze)
  { 
    printErrors(listing);
    fprintf(listing,"\nSymbol table:\n\n");
    printSymTab(listing);
  }
}

static void typeError(TreeNode * t, char * message)
{ fprintf(listing,"Type error at line %d: %s\n",t->lineno,message);
  Error = TRUE;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode * t)
{ switch (t->nodekind)
  { case ExpK:
      switch (t->kind.exp)
      { case OpK:
          if ((t->child[0]->type != Integer) ||
              (t->child[1]->type != Integer))
            typeError(t,"Op applied to non-integer");
          if ((t->attr.op == EQ) || (t->attr.op == LT))
            t->type = Boolean;
          else
            t->type = Integer;
          break;
        case ConstK:
        case IdK:
          t->type = Integer;
          break;
        default:
          break;
      }
      break;
    case StmtK:
      switch (t->kind.stmt)
      { 
        // case IfK:
        //   if (t->child[0]->type == Integer)
        //     typeError(t->child[0],"if test is not Boolean");
        //   break;
        case AssignK:
          if (t->child[0]->type != Integer)
            typeError(t->child[0],"assignment of non-integer value");
          break;
        case WriteK:
          if (t->child[0]->type != Integer)
            typeError(t->child[0],"write of non-integer value");
          break;
        case RepeatK:
          if (t->child[1]->type == Integer)
            typeError(t->child[1],"repeat test is not Boolean");
          break;
        case TypeK:
          if (strcmp(t->attr.name, "void") == 0 && t->child[0]->decl == 1) {
            typeError(t, "declaracao invalida de variavel");
          }

        default:
          break;
      }
      break;
    default:
      break;

  }
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree)
{ traverse(syntaxTree,nullProc,checkNode);
}
