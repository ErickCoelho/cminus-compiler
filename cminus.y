/******************/
/* File: cminus.y                                   */
/* The CMINUS Yacc/Bison specification file         */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/******************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static char * savedName; /* for use in assignments */
static int savedLineNo;  /* ditto */
static int savedNum;     /* for use in assignments */
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void);
int yyerror(char *s);

%}

%token IF ELSE INT RETURN VOID WHILE EMPTY
%token ID NUM
%token EQ LT GT GEQ LEQ EQEQ INEQ PLUS COMMA MINUS TIMES OVER
LPAREN RPAREN LBRACKETS RBRACKETS LCBRACES RCBRACES SEMI
%token ERROR

%% /* Grammar for Cminus */

program     : decl_list
                {
                  savedTree = $1;
                }
            ;
decl_list    : decl_list decl {
                YYSTYPE t = $1;
                if (t != NULL) {
                  while (t->sibling != NULL){
                    t = t->sibling;
                  }
                  t->sibling = $2;
                  $$ = $1;
                } else { $$ = $2; }
            }
            | decl  { $$ = $1; }
            ;
decl        : var_decl { $$ = $1; }
            | fun_decl { $$ = $1;}
            ;
var_decl    : type_spec ID {
                $$ = $1;
                savedName = copyString(globalId);
                savedLineNo = lineno;
                $$->child[0] = newStmtNode(VarDeclK);
                $$->child[0]->attr.name = savedName;
                $$->child[0]->lineno = savedLineNo;
                } SEMI {
                  $$ = $3;
                }
            | type_spec ID {
                savedName = copyString(globalId);
                savedLineNo = lineno;
              }
              LBRACKETS NUM { savedNum = atoi(tokenString); } RBRACKETS SEMI  {
                $$ = $1;
                $$->child[0] = newStmtNode(ArrDeclK);
                $$->child[0]->attr.name = savedName;
                $$->child[0]->lineno = savedLineNo;
                $$->child[0]->child[0] = newExpNode(ConstK);
                $$->child[0]->child[0]->attr.val = savedNum;
            }
            ;
type_spec   : INT {
                $$ = newStmtNode(TypeK);
                $$->attr.name = copyString(tokenString);
            }
            | VOID {
                $$ = newStmtNode(TypeK);
                $$->attr.name = copyString(tokenString);
            }
            ;
fun_decl    : type_spec ID {
                  savedName = copyString(globalId);
                  savedLineNo = lineno;
                  $$ = $1;
                  $$->child[0] = newStmtNode(FuncDeclK);
                  $$->child[0]->attr.name = savedName;
                  $$->child[0]->lineno = savedLineNo;
                } LPAREN params RPAREN comp_decl {
               $$ = $3;
               $$->child[0]->child[0] = $5;
               $$->child[0]->child[1] = $7;
            }
            ;
params      : param_list { $$ = $1; }
            | VOID {
                $$ = NULL;
            }
            ;
param_list  : param_list COMMA param {
                YYSTYPE t = $1;
                if (t != NULL) {
                  while (t->sibling != NULL) {
                    t = t->sibling;
                  }
                  t->sibling = $3;
                  $$ = $1;
                } else { $$ = $3; }
            }
            | param { $$ = $1; }
            ;
param       : type_spec ID {
                savedName = copyString(globalId); savedLineNo = lineno;
                $$ = $1;
                $$->child[0] = newStmtNode(VarDeclK);
                $$->child[0]->attr.name = savedName;
                $$->child[0]->lineno = lineno;
                } {
                $$ = $3;
            }
            | type_spec ID { savedName = copyString(globalId); savedLineNo = lineno; }  LBRACKETS RBRACKETS {
                $$ = $1;
                $$->child[0] = newStmtNode(ArrDeclK);
                $$->child[0]->attr.name = savedName;
                $$->child[0]->lineno = lineno;
            }
            ;
comp_decl   : LCBRACES loc_decl stmt_list RCBRACES {
                YYSTYPE t = $2;
                if (t != NULL) {
                  while (t->sibling != NULL) {
                    t = t->sibling;
                  }
                  t->sibling = $3;
                  $$ = $2;
                } else { $$ = $3; }
            }
            ;
loc_decl    : loc_decl var_decl {
                YYSTYPE t = $1;
                if (t != NULL) {
                  while (t->sibling != NULL) {
                    t = t->sibling;
                  }
                  t->sibling = $2;
                  $$ = $1;
                } else { $$ = $2; }
            }
            | %empty { $$ = NULL; }
            ;
stmt_list   : stmt_list stmt {
                YYSTYPE t = $1;
                if (t != NULL) {
                  while (t->sibling != NULL) {
                    t = t->sibling;
                  }
                  t->sibling = $2;
                  $$ = $1;
                } else { $$ = $2; }
            }
            | %empty { $$ = NULL; }
            ;
stmt        : exp_decl { $$ = $1; }
            | comp_decl { $$ = $1; }
            | sel_decl { $$ = $1; }
            | iter_decl { $$ = $1; }
            | rtrn_decl { $$ = $1;}
            ;
exp_decl    : exp SEMI { $$ = $1; }
            | SEMI { $$ = NULL; }
            ;
sel_decl    : IF LPAREN exp RPAREN stmt {
                $$ = newStmtNode(IfK);
                $$->child[0] = $3;
                $$->child[1] = $5;
              }
            | IF LPAREN exp RPAREN stmt ELSE stmt {
              $$ = newStmtNode(IfK);
              $$->child[0] = $3;
              $$->child[1] = $5;
              $$->child[2] = $7;
            }
            ;
iter_decl   : WHILE LPAREN exp RPAREN stmt {
                $$ = newStmtNode(RepeatK);
                $$->child[0] = $3;
                $$->child[1] = $5;
            }
            ;
rtrn_decl   : RETURN
              SEMI {
                $$ = newStmtNode(RetK);
            }
            | RETURN
              exp SEMI {
              $$ = newStmtNode(RetK);
              $$->child[0] = $2;
            }
            ;
exp         : var EQ exp {
                $$ = newStmtNode(AssignK);
                $$->attr.name = $1->attr.name;
                $$->child[0] = $1;
                $$->child[1] = $3;
              }
            | simple_exp {  $$ = $1; }
            ;
var         : ID {
                  $$ = newExpNode(IdK);
                  $$->attr.name = copyString(globalId);
                  $$->lineno = savedLineNo; }
            | ID {
              savedName = copyString(globalId);
              savedLineNo = lineno;
            } LBRACKETS exp RBRACKETS {
              $$ = newExpNode(IdK);
              $$->child[0] = $4;
              $$->attr.name = savedName;
              $$->lineno = savedLineNo;
            }
            ;
simple_exp  : sum_exp relational sum_exp {
              $$ = newExpNode(OpK);
              $$->child[0] = $1;
              $$->child[1] = $3;
              $$->attr.op = $2->attr.op;
            }
            | sum_exp { $$ = $1;}
            ;
relational  : LEQ {
                $$ = newExpNode(OpK);
                $$->attr.op = LEQ;
            }
            | LT {
                $$ = newExpNode(OpK);
                $$->attr.op = LT;
            }
            | GT {
                $$ = newExpNode(OpK);
                $$->attr.op = GT;
            }
            | GEQ {
                $$ = newExpNode(OpK);
                $$->attr.op = GEQ;
            }
            | EQEQ {
                $$ = newExpNode(OpK);
                $$->attr.op = EQEQ;
            }
            | INEQ {
                $$ = newExpNode(OpK);
                $$->attr.op = INEQ;
            }
            ;
sum_exp     : sum_exp sum term {
              $$ = newExpNode(OpK);
              $$->child[0] = $1;
              $$->child[1] = $3;
              $$->attr.op = $2->attr.op;
            }
            | term { $$ = $1; }
            ;
sum         : PLUS {
                $$ = newExpNode(OpK);
                $$->attr.op = PLUS;
            }
            | MINUS {
                $$ = newExpNode(OpK);
                $$->attr.op = MINUS;
             }
            ;
term        : term mult factor {
                $$ = newExpNode(OpK);
                $$->child[0] = $1;
                $$->child[1] = $3;
                $$->attr.op = $2->attr.op;
            }
            | factor { $$ = $1; }
            ;
mult        : TIMES {
                $$ = newExpNode(OpK);
                $$->attr.op = TIMES;
            }
            | OVER {
               $$ = newExpNode(OpK);
               $$->attr.op = OVER;
            }
            ;
factor      : LPAREN exp RPAREN { $$ = $2; }
            | var { $$ = $1; }
            | activ { $$ = $1; }
            | NUM {
              $$ = newExpNode(ConstK);
              $$->attr.val = atoi(tokenString);
            }
            ;
activ       : ID {
                savedName = copyString(globalId);
                savedLineNo = lineno;
                $$ = newStmtNode(ActivK);
                $$->attr.name = savedName;
                $$->lineno = savedLineNo;
              }
              LPAREN args RPAREN {
                $$ = $2;
                $$->child[0] = $4;
            }
            ;
args        : arg_list {
                 $$ = $1;
              }
            | %empty { $$ = NULL; }
            ;
arg_list    : arg_list COMMA exp {
                YYSTYPE t = $1;
                if (t != NULL) {
                  while (t->sibling != NULL) {
                    t = t->sibling;
                  }
                  t->sibling = $3;
                  $$ = $1;
                } else {
                  $$ = $3;
                }
              }
            | exp {  $$ = $1;  }
            ;
%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}