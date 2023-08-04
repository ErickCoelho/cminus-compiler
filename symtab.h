/****************************************************/
/* File: symtab.h                                   */
/* Symbol table interface for the TINY compiler     */
/* (allows only one symbol table)                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_
#include "globals.h"

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */


/* the list of line numbers of the source 
 * code in which a variable is referenced
 */
typedef struct LineListRec
   { int lineno;
     struct LineListRec * next;
   } * LineList;

/* The record in the bucket lists for
 * each variable, including name, 
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */
typedef struct BucketListRec
   { int paramcount;
     char * name;
     LineList lines;
     int memloc ; /* memory location for variable */
     char * escopo;
     int decl;
     int type;
     struct BucketListRec * next;
   } * BucketList;

BucketList findbucketlist (char * name, char * escopo);

void st_insert( int paramcount, char * name, int lineno, char * escopo, int loc, int decl, int type );

void update_type( char * name, int type );

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
int st_lookup ( char * name, char * escopo );

int paramcomp ( char * name, char * escopo , int paramc);
int paramcount ( TreeNode * t );

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing);

void printErrors(FILE * listing);

#endif
