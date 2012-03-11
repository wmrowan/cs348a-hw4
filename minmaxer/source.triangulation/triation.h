
/*---------------------------------------------------------------------------*/

typedef struct {
  indexType center, left, right;
} criterionType;

/*---------------------------------------------------------------------------*/

typedef struct {
  int          quadrant;
  lmNumberType nominator, denominator;
  indexType    lexHigh, lexMed, lexLow;
} angleType;

/*---------------------------------------------------------------------------*/

typedef struct {
  lmNumberType area2; /* == 4 x the signed square of the are of the triangle */
  lmNumberType length2; /* == the square of the length of the base line */
  indexType    lexHigh; /* == index of the anker */
  indexType    lexMed; /* == index of the from vertex */
  indexType    lexLow; /* == index of the to vertex */
  short        status; 
               /* decides whether from vertex and to vertex are the same */
} heightType;

/*---------------------------------------------------------------------------*/

typedef struct {
  lmNumberType A;
  lmNumberType B;
  lmNumberType C;
  lmNumberType A2plusB2;
  lmNumberType C2;
  indexType    lexHigh; /* == index of the anker */
  indexType    lexMed; /* == index of the from vertex */
  indexType    lexLow; /* == index of the to vertex */
} slopeType;

/*---------------------------------------------------------------------------*/

typedef struct {
  lmNumberType x, y;
  indexType    a;
} vertexType;

/*---------------------------------------------------------------------------*/

typedef struct {
  lmNumberType x, y, z;
  indexType    a;
} vertex3dType;

/*---------------------------------------------------------------------------*/

double ANGLEMAKETIME;
long   NOFANGLECOMPARISONS;

/*---------------------------------------------------------------------------*/

#define PLANE 2
#define THREESPACE 3


#define GT_CAN_DECIDE_RIGHT_TURN 1
#define GT_CANNOT_DECIDE_RIGHT_TURN 0

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
