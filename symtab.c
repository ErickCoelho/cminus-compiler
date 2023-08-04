 /****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "globals.h"
#include "analyze.h"

/* SIZE is the size of the hash table */
#define SIZE 211

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

/* the hash function */
static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

/* the hash table */
static BucketList hashTable[SIZE];

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
BucketList findbucketlist (char * name, char * escopo){
  BucketList l = NULL;

  char nameescopo[50];
  strcpy(nameescopo, "");
  strcat(nameescopo,name);
  strcat(nameescopo,escopo);
  //printf("$$$$$$$%s",nameescopo);
    // fprintf(listing,"HASH: %s\n", nameescopo);

  int h1 = hash(nameescopo);

  BucketList l1 =  hashTable[h1];

  char nameescopol1[50];
  strcpy(nameescopol1, "");
  if(l1 != NULL) strcat(nameescopol1,l1->name);
  strcat(nameescopol1,escopo);

  while ((l1 != NULL) && (strcmp(nameescopo,nameescopol1) != 0)){
    l1 = l1->next;

    strcpy(nameescopol1, "");
    if(l1 != NULL) strcat(nameescopol1,l1->name);
    strcat(nameescopol1,escopo);
  }

  if (l1 != NULL) l =l1;
  else {
    // fprintf(listing,"\n\n___________\n");
    char nameglobal[50];
    strcpy(nameglobal, "");
    strcat(nameglobal,name);
    strcat(nameglobal,"global");
    // fprintf(listing,"nameglobal: %s\n", nameglobal);

    int h2 = hash(nameglobal);

    BucketList l2 =  hashTable[h2];

    char namegloball2[50];
    strcpy(namegloball2, "");
    if(l2 != NULL) strcat(namegloball2,l2->name);
    else strcat(namegloball2,"VAZIO");
    strcat(namegloball2,"global");
    // fprintf(listing,"namegloball2: %s\n", namegloball2);

    while ((l2 != NULL) && (strcmp(nameglobal,namegloball2) != 0)){
      l2 = l2->next;

      strcpy(namegloball2, "");
      if(l2 != NULL) strcat(namegloball2,l2->name);
      else strcat(namegloball2,"VAZIOOO");
      strcat(namegloball2,"global");
      // fprintf(listing,"namegloball2: %s\n", namegloball2);
    }

    if (l2 != NULL) l = l2;
    
    // fprintf(listing,"___________");
  }
  return l;
}

void st_insert( int paramcount, char * name, int lineno, char * escopo, int loc, int decl, int type )
{ 
  BucketList l = NULL;
  
  l = findbucketlist(name, escopo);
  
  char nameescopo[50];
  strcpy(nameescopo, "");
  strcat(nameescopo,name);
  strcat(nameescopo,escopo);
  //printf("$$$$$$$%s",nameescopo);
    // fprintf(listing,"HASH: %s\n", nameescopo);

  int h = hash(nameescopo);

  if (l == NULL) /* variable not yet in table */
  { l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->paramcount = paramcount;
    l->name = name;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->escopo = escopo;
    l->memloc = loc;
    l->decl = decl;
    l->type = type;
    l->lines->next = NULL;
    l->next = hashTable[h];
    hashTable[h] = l; }
  else /* found in table, so just add line number */
  { //fprintf(listing, "\n####\n%s %s %d %d\n####\n", l->name, l->escopo, l->lines->lineno, lineno);
    LineList t = l->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
} /* st_insert */

void update_type ( char * name, int type )
{ int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l != NULL) {
    l->type = type;
  }
}

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
int st_lookup ( char * name, char * escopo )
{ 
  BucketList l = NULL;
  
  l = findbucketlist(name, escopo);
  if(l != NULL) return l->memloc;
  else return -1;
}

int paramcount ( TreeNode * t ){
  int count = 0;
  t = t->child[0];
  while (t != NULL) {
    count++;
    t = t->sibling;
  }
  return count;
}

int paramcomp ( char * name, char * escopo , int paramc)
{ 
  char nameescopo[50];
  strcpy(nameescopo, "");
  strcat(nameescopo,name);
  strcat(nameescopo,escopo);

  int h1 = hash(nameescopo);

  BucketList l1 =  hashTable[h1];

  char nameescopol1[50];
  strcpy(nameescopol1, "");
  if(l1 != NULL) strcat(nameescopol1,l1->name);
  strcat(nameescopol1,escopo);

  while ((l1 != NULL) && (strcmp(nameescopo,nameescopol1) != 0)){
    l1 = l1->next;

    strcpy(nameescopol1, "");
    if(l1 != NULL) strcat(nameescopol1,l1->name);
    strcat(nameescopol1,escopo);
  }

  if (l1 != NULL && paramc == l1->paramcount) return 1;
  else {
    // fprintf(listing,"\n\n___________\n");
    char nameglobal[50];
    strcpy(nameglobal, "");
    strcat(nameglobal,name);
    strcat(nameglobal,"global");
    // fprintf(listing,"nameglobal: %s\n", nameglobal);

    int h2 = hash(nameglobal);

    BucketList l2 =  hashTable[h2];

    char namegloball2[50];
    strcpy(namegloball2, "");
    if(l2 != NULL) strcat(namegloball2,l2->name);
    else strcat(namegloball2,"VAZIO");
    strcat(namegloball2,"global");
    // fprintf(listing,"namegloball2: %s\n", namegloball2);

    while ((l2 != NULL) && (strcmp(nameglobal,namegloball2) != 0)){
      l2 = l2->next;

      strcpy(namegloball2, "");
      if(l2 != NULL) strcat(namegloball2,l2->name);
      else strcat(namegloball2,"VAZIOOO");
      strcat(namegloball2,"global");
      // fprintf(listing,"namegloball2: %s\n", namegloball2);
    }

    if (l2 != NULL && paramc == l2->paramcount) return 1;
    else return 0;
    
    // fprintf(listing,"___________");
  }
}

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
char *convertDeclToMessage (int decl) {
  if (decl == 1) {
    return "VAR";
  }
  if (decl == 2) {
    return "FUNC";
  }
  if (decl == 3) {
    return "ARR";
  }
  return " ";
}

char *convertTypeToMessage (int type) {
  if (type == 0) {
    return "void";
  }
  if (type == 1) {
    return "int";
  }
  if (type == 2) {
    return "bool";
  }
  return " ";
}


void printSymTab(FILE * listing)
{ int i;
  fprintf(listing,"Variable Name Location Escopo     Tipo ID Tipo Dado Line Numbers\n");
  fprintf(listing,"------------- -------- ---------- ------- --------- ------------\n");
  for (i=0;i<SIZE;++i)
  { if (hashTable[i] != NULL)
    { BucketList l = hashTable[i];
      while (l != NULL)
      { //if(strcmp(l->name, "input") != 0 && strcmp(l->name, "output") != 0){
          LineList t = l->lines;
          fprintf(listing,"%-13s ",l->name);
          fprintf(listing,"%-8d ",l->memloc);
          fprintf(listing,"%-10s ",l->escopo);
          fprintf(listing,"%-7s ",convertDeclToMessage(l->decl));
          fprintf(listing,"%-9s ",convertTypeToMessage(l->type));
          t = t->next;
          while (t != NULL)
          { fprintf(listing,"%4d ",t->lineno);
            t = t->next;
          }
          fprintf(listing,"\n");
        //}
        l = l->next;
      }
    }
  }
} /* printSymTab */

void printErrors(FILE * listing)
{ int i;
  int no_main = 0;
  for (i=0;i<SIZE;++i)
  { if (hashTable[i] != NULL)
    { BucketList l = hashTable[i];
      while (l != NULL)
      { LineList t = l->lines;
        if (strcmp(l->name, "main") == 0 && l->decl == 2){
          no_main = 1;
        }
        while (t != NULL)
        {
          t = t->next;
        }
        l = l->next;
      }
    }
  }
  if (no_main == 0){
    printf("Erro semantico: funcao main() não declarada\n");
    Error = 1;
  }
}
