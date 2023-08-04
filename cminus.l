/****************************************************/
/* File: cminus.l                                   */
/* Lex specification for CMINUS                     */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

%{
#include "globals.h"
#include "util.h"
#include "scan.h"
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
char globalId[MAXTOKENLEN + 1];
%}
digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+
%%
"if"            {return IF;}
"else"          {return ELSE;}
"int"           {return INT;}
"return"        {return RETURN;}
"void"          {return VOID;}
"while"         {return WHILE;}
""              {return EMPTY;}
"="             {return EQ;}
"<"             {return LT;}
">"             {return GT;}
">="            {return GEQ;}
"<="            {return LEQ;}
"=="            {return EQEQ;}
"!="            {return INEQ;}
"+"             {return PLUS;}
","             {return COMMA;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"             {return OVER;}
"("             {return LPAREN;}
")"             {return RPAREN;}
"["             {return LBRACKETS;}
"]"             {return RBRACKETS;}
"{"             {return LCBRACES;}
"}"             {return RCBRACES;}
";"             {return SEMI;}
{number}        {return NUM;}
{identifier}    {strncpy(globalId, yytext, MAXTOKENLEN);return ID;}
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}
"/*"            { char c;
                  int end_loop = 0;
                  int found_asterisk = 0;
                  do
                  { c = input();
                    if (c == EOF) break;
                    if (c == '\n') lineno++;
                    if (found_asterisk == 1) {
                      if (c == '/') {
                        end_loop = 1;
                      } else {
                        found_asterisk = 0;
                      }
                    }
                    if (c == '*') {
                      found_asterisk = 1;
                    }
                  } while (end_loop == 0);
                }
.               {return ERROR;}
%%
TokenType getToken(void)
{ static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}
