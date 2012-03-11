/*
 * NAME: quicksort.c
 *
 * DESCRIPTION:
 *	Quicksort with types to make lint happy.
 *
 * HISTORY:
 *	89/10/26: This header added
 */

#include <stdio.h>
#include <sys/time.h>
#include "road.h"
#include "longmath.h"
#include "quadedge.h"
#include "graph.h"


int randint(lo, hi)
int lo, hi;
{
  return((rand() % (hi-lo+1)) + lo);
}  /* -------------------- randint -------------------- */


void quickSort1Index(item, lo, hi, lt, swap)
indexType item[];
int lo, hi;
int (*lt)();
void (*swap)();
{
  int m, i;

  if (lo < hi) {
    (swap)(item, lo, randint(lo, hi));
    m = lo;
    for (i=lo+1; i<=hi; i++) {
      /* invariant: item[l+1 .. m] < t and item[m+1 .. i-1] >= t */
      if ((lt)(item, i, lo)) {
        m++;
        (swap)(item, m, i);
      }
    }

    (swap)(item, lo, m);
    /* item[l .. m-1] < item[m] <= item[m+1 .. hi] */
    quickSort1Index(item, lo, m-1, lt, swap);
    quickSort1Index(item, m+1, hi, lt, swap);
  }
}  /* -------------------- quickSort1Index -------------------- */


void quickSortIndex(item, n, lt, swap)
indexType item[];
int n;
int (*lt)();
void (*swap)();
{
  quickSort1Index(item, 0, n-1, lt, swap);
}  /* -------------------- quickSortIndex -------------------- */


void quickSort1Site(item, lo, hi, lt, swap)
siteType item[];
int lo, hi;
int (*lt)();
void (*swap)();
{
  int m, i;

  if (lo < hi) {
    (swap)(item, lo, randint(lo, hi));
    m = lo;
    for (i=lo+1; i<=hi; i++) {
      /* invariant: item[l+1 .. m] < t and item[m+1 .. i-1] >= t */
      if ((lt)(item, i, lo)) {
        m++;
        (swap)(item, m, i);
      }
    }

    (swap)(item, lo, m);
    /* item[l .. m-1] < item[m] <= item[m+1 .. hi] */
    quickSort1Site(item, lo, m-1, lt, swap);
    quickSort1Site(item, m+1, hi, lt, swap);
  }
}  /* -------------------- quickSort1Site -------------------- */


void quickSortSite(item, n, lt, swap)
siteType item[];
int n;
int (*lt)();
void (*swap)();
{
  quickSort1Site(item, 0, n-1, lt, swap);
}  /* -------------------- quickSortSite -------------------- */


static void 
randomizeOrderIntern (item, lo, hi, swap)

     siteType item[];
     int lo, hi;
     void (*swap)();

{
  long random();
  struct timeval tp;
  struct timezone tzp;
  int seed;
  int i;
  indexType index;

  gettimeofday(&tp, &tzp);
  seed = tp.tv_usec % (256*256);
  srandom(seed);

  for (i = hi; i > lo; i--) {
    index = random() % (i - lo + 1) + lo;
    (swap)(item, index, i);
  }
}

void 
randomizeOrder (item, n, swap)

     siteType item[];
     int n;
     void (*swap)();

{
  randomizeOrderIntern (item, 0, n-1, swap);
}  
