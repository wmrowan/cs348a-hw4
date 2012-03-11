 /* road.h

   contains definitions of general types, like indices, ...
*/

/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

extern double get_user_time ();
extern double get_time_of_day ();
extern void swapSite ();
extern int LTsite ();

typedef unsigned int bool;

typedef int indexType;

typedef struct {

  indexType  *entry;
  indexType  *indexOf;
  void       *in;
  int         free;
  int         maxn; 
  int         maxIndex;
  int         nofElts;
  int         top;
  bool        (*GT) ();
  char        *data;
  int         mode;

} queuesType;

#define POOL_FIXED_SIZE 23423
#define POOL_ADJUSTABLE_SIZE 4564243

typedef struct {

  queuesType * (*create) ();
  bool       (*top) ();
  void       (*insert) ();
  void       (*delete) ();
  bool       (*doesContain) ();
  void       (*dispose) ();
  
} poolType;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

typedef struct {
  indexType center, left, right;
} wedgeType;

/*---------------------------------------------------------------------------*/

#ifdef FLOATPOINT
  typedef double coordType;
#else
  typedef int coordType;
#endif

/*---------------------------------------------------------------------------*/

bool sosAngleGT(), sosAngleGT180 ();
float sosAngleFloat ();
void lmPrintAngle();

/*---------------------------------------------------------------------------*/

bool sosHeightLT(), sosHeightLT0 ();
float sosHeightFloat ();
void lmPrintHeight();

/*---------------------------------------------------------------------------*/

bool sosSlopeGT(), sosSlopeRightTurn ();
float sosSlopeFloat ();
void lmPrintSlope();

/*---------------------------------------------------------------------------*/

extern int GLOBAL_INTERRUPT_ALGORITHM;
#define DO_INTERRUPT_ALGORITHM do{GLOBAL_INTERRUPT_ALGORITHM = 1;}while(0)
#define DONT_INTERRUPT_ALGORITHM do{GLOBAL_INTERRUPT_ALGORITHM = 0;}while(0)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

