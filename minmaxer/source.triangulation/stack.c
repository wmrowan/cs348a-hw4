/*
 * NAME: stack.c
 *
 * DESCRIPTION:
 *	Within this file are routines supporting a stack data structure.
 *
 * HISTORY:
 *	89/10/26: This header added
 */


#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

//extern char *malloc(), *calloc();


stackType *stkNew(n)
int n;
{
  stackType *stk;
  static char *fail = { "stkNew: fail on [mc]alloc.\n" };

  if ((stk = (stackType *) malloc(sizeof(stackType))) == NULL) {
    (void) fprintf(stdout, fail);
    exit(1);
  }

  stk->stkwords = (stackEntry *) calloc((unsigned int) n, sizeof(stackEntry));
  if (stk->stkwords == NULL) {
    (void) fprintf(stdout, fail);
    exit(1);
  }

  stk->maxn = n;
  stk->sp = 0;

  return(stk);
}  /* -------------------- stkNew -------------------- */


void stkDispose(stk)
stackType *stk;
{
  free((char *) stk->stkwords);
  free((char *) stk);
}  /* -------------------- stkDispose -------------------- */


void stkReset(stk)
stackType *stk;
{
  stk->sp = 0;
}  /* -------------------- stkReset --------------------- */


void stkPush(stk, i)
stackType *stk;
int i;

{
  void (*xx) ();

  xx = 0;
  
  if (stk->sp < stk->maxn)
    stk->stkwords[stk->sp++] = i;

  else {
    (void) fprintf(stdout, "stkPush: stack overflow.\n");
    xx ();
  }
}  /* -------------------- stkPush -------------------- */


void stkPop(stk, ip)
stackType *stk;
int *ip;
{
  if (stk->sp)
    *ip = stk->stkwords[--stk->sp];

  else
    (void) fprintf(stdout, "stkPop: stack underflow.\n");
}  /* -------------------- stkPop -------------------- */


int stkEmpty(stk)
stackType *stk;
{
  return(! stk->sp);
}  /* -------------------- stkEmpty -------------------- */
