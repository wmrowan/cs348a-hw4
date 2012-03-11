/* geometry.objects.h

   contains definitions of geometrical objects, like points, angles ...
*/

typedef int indexType;

/*---------------------------------------------------------------------------*/
/* definition of a coordinate */

typedef int coordType;

/*---------------------------------------------------------------------------*/
/* definition of a vertex */

typedef struct {

  coordType x;
  coordType y;

} vertexType;

#define VX   (v) ((v).x)
#define VY   (v) ((v).y)

/*---------------------------------------------------------------------------*/
/* definition of an angle */

/* the angle $\angle v_{from}v_{center}v_{to}$ is defined in terms of the 
   (directed) quadedge $v_{center}v_{to}$. */

typedef struct {

  indexType qeIndex;  

} angleType;


/* the following is the type for an angle if the angle has to be defined in
   terms of the three verices $v_{from}$, $v_{center}$, and $v_{to}$. */

typedef struct {

  indexType  from,
             center,
             to;

} longAngleType;

#define VIND (v) ((v).index)

typedef int bool;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

