/*
 * NAME: bitvector.c
 *
 * DESCRIPTION:
 *	This file provides routines supporting a bitvector data structure.
 *
 * HISTORY:
 *	89/10/26: this header added
 */

#include <stdio.h>
#include <stdlib.h>
#include "bitvector.h"

// extern char *malloc(), *calloc();

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
bvAdjustSize (bv, newN)

     bvType *bv;
     int newN;

{
  static char *fail = { "bvNew: fail on [mc]alloc.\n" };
  int i;
  unsigned int bitsPerWord, nwordsBefore, nwordsAfter;

  bv->bvwords = (bvWordType *) 
    realloc ((char *) bv->bvwords,
	     (unsigned int) (2 * newN + 1) * sizeof (bvWordType));
  if (bv->bvwords == NULL) {
    (void) fprintf(stdout, fail);
    exit(1);
  }

  bitsPerWord = sizeof(bvWordType) << 3;
  nwordsBefore = bv->maxn / bitsPerWord + 1;
  nwordsAfter = (2 * newN) / bitsPerWord + 1;

  for (i = nwordsBefore; i < nwordsAfter; i++)
    bv->bvwords[i] = 0;

  bv->maxn = 2 * newN;
}
/*---------------------------------------------------------------------------*/
/*   bvNew allocates a bitvector with bits to toggle for 0 through maxn.
 */
bvType *bvNew(maxn, mode)
int maxn;
int mode;
{
  bvType *bv;
  static char *fail = { "bvNew: fail on [mc]alloc.\n" };
  unsigned int bitsPerWord, nwords;

  bitsPerWord = sizeof(bvWordType) << 3;
  nwords = maxn / bitsPerWord + 1;

  if ((bv = (bvType *) malloc(sizeof(bvType))) == NULL) {
    (void) fprintf(stdout, fail);
    exit(1);
  }

  /* calloc initializes the words to 0 */
  bv->bvwords = (bvWordType *) calloc(nwords, sizeof(bvWordType));
  if (bv->bvwords == NULL) {
    (void) fprintf(stdout, fail);
    exit(1);
  }

  bv->maxn = maxn;

  if ((mode != BV_FIXED_SIZE) && (mode != BV_ADJUSTABLE_SIZE)) {
    printf("ERROR: bvNew: mode inconsistent.\n");
    exit (1);
  }
  bv->mode = mode;

  /* calculate log2(bitsPerWord) */
  bv->lgbits = 0;
  while (bitsPerWord >>= 1)
    bv->lgbits++;

  return(bv);
}  /* -------------------- bvNew -------------------- */
  

void bvDispose(bv)
bvType *bv;
{
  free((char *) bv->bvwords);
  free((char *) bv);
}  /* -------------------- bvDispose ------------------- */


void bvReset(bv)
bvType *bv;
{
  int i;
  unsigned int bitsPerWord, nwords;

  bitsPerWord = sizeof(bvWordType) << 3;
  nwords = bv->maxn / bitsPerWord + 1;

  for (i=0; i < nwords; i++)
    bv->bvwords[i] = 0;
}  /* -------------------- bvReset -------------------- */


void bvSet(bv, i)
bvType *bv;
int i;
{
  bvWordType modMask = (sizeof(bvWordType)<<3) - 1;

  if (i > bv->maxn)
    if (bv->mode == BV_ADJUSTABLE_SIZE)
      bvAdjustSize (bv, i);
    else {
      (void) fprintf(stdout, "bvSet: bounds error.\n");
      exit (1);
    }
  if (i < 0) {
    (void) fprintf(stdout, "bvSet: bounds error.\n");
    exit (1);
  }

  bv->bvwords[i >> bv->lgbits] |= 1 << (i & modMask);
}  /* -------------------- bvSet -------------------- */


void bvClear(bv, i)
bvType *bv;
int i;
{
  unsigned int word = i >> bv->lgbits;	
  bvWordType modMask = (sizeof(bvWordType)<<3) - 1;
  bvWordType setbit;

  setbit = 1 << (i & modMask);

  if (0 <= i && i <= bv->maxn) {
    bv->bvwords[word] |= setbit;	/* make sure bit is set */
    bv->bvwords[word] ^= setbit;	/* toggle it off with xor */
  }
  else {
    (void) fprintf(stdout, "bvClear: bounds error.\n");
    exit (1);
  }
}  /* -------------------- bvClear -------------------- */


int bvTest(bv, i)
bvType *bv;
int i;
{
  bvWordType modMask = (sizeof(bvWordType)<<3) - 1;

  if (0 <= i && i <= bv->maxn)
    return(bv->bvwords[i >> bv->lgbits] & (1 << (i & modMask)));

  /* else ... */
  (void) fprintf(stdout, "bvTest: bounds error.\n");
  exit (1);
}  /* -------------------- bvTest -------------------- */
