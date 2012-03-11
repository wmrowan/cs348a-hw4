// #include "general.objects.h"
#include "geometry.objects.h"
#include <stdio.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------*/

#define InitPower(elt)  {(elt).nofInd0 = (elt).nofInd1 = 0;}

/*---------------------------------------------------------------------------*/

/* this type represents the power of an epsilon. this representation
should make it easy to enumerate the powers of epsilon in increasing order.
the power can be comprised of at most two indices of each of the
vertices composing an angle and the coordinates. hence,

nofIndi \in {0, 1, 2},
name0i \in {0, 1, 2, 3}, where a := 1, b := 2, and c := 3,
name1i \in {0, 1, 2, 3}, where d := 1, e := 2, and f := 3, and
coordij \in {0, 1}, where x0 := 0, x1 := 1.
*/

typedef struct {
  bool nsi              : 1; /* nsi ... next entry has the same index */
  
  unsigned int nofInd0  : 2;
  unsigned int name01   : 2;
  unsigned int coord01  : 1;
  unsigned int name02   : 2;
  unsigned int coord02  : 1;
  
  unsigned int nofInd1  : 2;
  unsigned int name11   : 2;
  unsigned int coord11  : 1;
  unsigned int name12   : 2;
  unsigned int coord12  : 1;
} powerType;

/*---------------------------------------------------------------------------*/

typedef struct {

  unsigned int angle : 1;
  unsigned int name  : 2;
  unsigned int coord : 1;

} singlePowerType;

/*---------------------------------------------------------------------------*/

typedef struct {

  indexType index;
  singlePowerType ident;

} sortedIndicesType;

/*---------------------------------------------------------------------------*/

/* this type contains all the information to enumerate a new (one which has
   not been used before.

   for example: 
   
        new index: (b, 0) yields enumerations: ((b, 0)).
        new index: (a, 0) yields enumerations: ((a, 0)) and
                                               ((a, 0) (a, 0)).
        new index: (a, 0) = (d, 1) yields enumerations:
                ((a, 0)), ((d, 1)) and
                ((a, 0) (a, 0)), ((d, 1) (d, 1)), ((a, 0) (d, 1)) and
		((a, 0) (a, 0) (d, 1)), ((a, 0) (d, 1) (d, 1)) and
		((a, 0) (a, 0) (d, 1) (d, 1)), for xi_b = xi_b' and
                                                   xi_e <> xi_e'.

         len is the number of indices that make up the power.
         nofPower is the number of ways in which the same power
         can be generated.
	 actIndex is position in si which is being enumerated.
	 iC gives the 'xi' values.
	 si is an array that contains the sorted list of indices.
	 indices is the array that contains the enumerated indices.
*/
        
typedef struct {

  unsigned int      len;
  unsigned int      nofPowers;
  unsigned int      actIndex;
  unsigned int      iC[2][2];
  sortedIndicesType si[12];
  powerType         indices[3];

} nextIndexType;

/*---------------------------------------------------------------------------*/

typedef struct {
  nextIndexType ni;
  powerType power[900];
  powerType * top;
  powerType * act;
  powerType * stop;
  powerType * scan;
} powerStackType;

/*---------------------------------------------------------------------------*/

/* this function adds a new index (nI) to an old set of powers (oP), if
possible, and returns the result in nP. the return value of the
function is 1 (true) if the operation is ok; and 0 (false) if the
adding of nI would result in an power of epsilon which is not
obtainable.
iC is a pointer to the index conversion table. */


static bool
addPower (oP, nI, iC, nP)

     powerType oP, * nP;
     singlePowerType nI;
     unsigned int iC[2][2];

{
  bool addIsOK = 1;

  if (nI.angle == 0)                 /* add to angle 0 ? */
    if (oP.nofInd0 == 2)          /* already 2 indices in angle 0 ? */
      addIsOK = 0;                       /* can't add anything */
    else 
      if (oP.nofInd0 == 1)        /* already 1 index in angle 0 ? */
	if (((oP.nofInd1 == 2) &&    /* and already 2 indices in angle 1 ? */
	     (((oP.coord11 == oP.coord12) ==       /* and both pairs are */
	       (iC[1][0] == iC[1][1])) ==          /* in the nominator */
	      ((oP.coord01 == nI.coord) ==       /* or both pairs are in */
	       (iC[0][0] == iC[0][1])))) ||        /* the denominator ? */
	    ((oP.name01 == nI.name) &&   /* or the new index is the same */
	     (oP.name01 != 1))) /* as the 1st in oP but it's not the center ?*/
	  addIsOK = 0;                             /* can't add anything */
	else {
	  *nP = oP;                            /* add new index */
	  nP->nofInd0 = 2;
	  nP->name02 = nI.name;
	  nP->coord02 = nI.coord;
	}
      else {
	*nP = oP;                            /* add new index */
	nP->nofInd0 = 1;
	nP->name01 = nI.name;
	nP->coord01 = nI.coord;
      }
  else
    if (oP.nofInd1 == 2)         
      addIsOK = 0;               
    else 
      if (oP.nofInd1 == 1)       
	if (((oP.nofInd0 == 2) &&
	     (((oP.coord01 == oP.coord02) ==       
	       (iC[0][0] == iC[0][1])) ==
	      ((oP.coord11 == nI.coord) ==       
	       (iC[1][0] == iC[1][1])))) ||
	    ((oP.name11 == nI.name) &&
	     (oP.name11 != 1)))
	  addIsOK = 0;                             
	else {
	  *nP = oP;                           
	  nP->nofInd1 = 2;
	  nP->name12 = nI.name;
	  nP->coord12 = nI.coord;
	}
      else {
	*nP = oP;                            
	nP->nofInd1 = 1;
	nP->name11 = nI.name;
	nP->coord11 = nI.coord;
      }
  return addIsOK;
}

/*---------------------------------------------------------------------------*/
static bool
addMultiplePowers (oP1, oP2, iC, nP)

     powerType oP1, oP2, * nP;
     unsigned int iC[2][2];

{
  bool addIsOK = 1;
  singlePowerType nsi;
  powerType tmp1, tmp2;

  if ((oP1.nofInd0 + oP2.nofInd0 > 2) ||
      (oP1.nofInd1 + oP2.nofInd1 > 2))
    addIsOK = 0;
  else {
    if (oP2.nofInd0 > 0) {
      nsi.angle = 0;
      nsi.name = oP2.name01;
      nsi.coord = oP2.coord01;
      addIsOK = addPower (oP1, nsi, iC, &tmp1);
      if ((addIsOK) && (oP2.nofInd0 > 1)) {
	nsi.angle = 0;
	nsi.name = oP2.name02;
	nsi.coord = oP2.coord02;
	addIsOK = addPower (tmp1, nsi, iC, &tmp2);
	tmp1 = tmp2;
      }
    } else
      tmp1 = oP1;
    if ((addIsOK) &&(oP2.nofInd1 > 0)) {
      nsi.angle = 1;
      nsi.name = oP2.name11;
      nsi.coord = oP2.coord11;
      addIsOK = addPower (tmp1, nsi, iC, &tmp2);
      if ((addIsOK) && (oP2.nofInd1 > 1)) {
	nsi.angle = 1;
	nsi.name = oP2.name12;
	nsi.coord = oP2.coord12;
	addIsOK = addPower (tmp2, nsi, iC, &tmp1);
      } else
	tmp1 = tmp2;
    }
  }
  *nP = tmp1;
  return addIsOK;
}
    
/*---------------------------------------------------------------------------*/

static void
sortIndices (angle0, angle1, si)

     longAngleType     * angle0, * angle1;
     sortedIndicesType si[12]; 

{
  static unsigned int centerName = 1;
  static unsigned int fromName =   2;
  static unsigned int toName =     3;

  int i;
  int i1, i2;
  sortedIndicesType v[2][3];
  longAngleType * angle;
  sortedIndicesType  * vv;
 

  /* sort the indices of angle[i] into v[i] */

  angle = angle0;

  vv = v[0];
  for (i = 0; i < 2; i++) {
    vv[0].ident.angle = vv[1].ident.angle = vv[2].ident.angle = i;
    if (angle->from < angle->center)
      if (angle->center < angle->to) {
	vv[0].index = angle->from;
	vv[0].ident.name  = fromName;
	vv[1].index = angle->center;
	vv[1].ident.name  = centerName;
	vv[2].index = angle->to;
	vv[2].ident.name  = toName;
      } else {
	vv[2].index = angle->center;
	vv[2].ident.name  = centerName;
	if (angle->to < angle->from) {
	  vv[0].index = angle->to;
	  vv[0].ident.name  = toName;
	  vv[1].index = angle->from;
	  vv[1].ident.name  = fromName;
	} else {
	  vv[0].index = angle->from;
	  vv[0].ident.name  = fromName;
	  vv[1].index = angle->to;
	  vv[1].ident.name  = toName;
	}
      }
    else
      if (angle->from < angle->to) {
	vv[0].index = angle->center;
	vv[0].ident.name  = centerName;
	vv[1].index = angle->from;
	vv[1].ident.name  = fromName;
	vv[2].index = angle->to;
	vv[2].ident.name  = toName;
      } else {
	vv[2].index = angle->from;
	vv[2].ident.name  = fromName;
	if (angle->to < angle->center) {
	  vv[0].index = angle->to;
	  vv[0].ident.name  = toName;
	  vv[1].index = angle->center;
	  vv[1].ident.name  = centerName;
	} else {
	  vv[0].index = angle->center;
	  vv[0].ident.name  = centerName;
	  vv[1].index = angle->to;
	  vv[1].ident.name  = toName;
	}
      }
    angle = angle1;
    vv = v[1];
  };

  /* merge v[1] and v[2] into si s.t. the indices si[].index in si
     are sorted  */

  i = i1 = i2 = 0;

  do {
    si[i+1] = si[i] =
      (v[0][i1].index < v[1][i2].index) ? v[0][i1++] : v[1][i2++];
    si[i++].ident.coord = 1;
    si[i++].ident.coord = 0;
  } while ((i1 != 3) && (i2 != 3));

  for (;i1 != 3;) {
    si[i+1] = si[i] = v[0][i1++];
    si[i++].ident.coord = 1;
    si[i++].ident.coord = 0;
  };
  for (;i2 != 3;) {
    si[i+1] = si[i] = v[1][i2++];
    si[i++].ident.coord = 1;
    si[i++].ident.coord = 0;
  };

  /* test if two successive indices are equal and if they are
     rearange the names and coordinate identifiers s.t. the x1-coordinates
     of both names are followed by the x0-coordinates  */

  for (i = 0; i <= 10; i += 2)
    if (si[i].index == si[i+2].index) {
      si[i+1].ident = si[i+2].ident;
      si[i+2].ident = si[i].ident;
      i += 2;
      si[i].ident.coord = 0;
    };
}

/*---------------------------------------------------------------------------*/
static bool
initIndexType (ni, iC, angle0, angle1)

     nextIndexType * ni;
     unsigned int iC[2][2];
     longAngleType * angle0, * angle1;

{
  ni->len = 0;
  ni->nofPowers = 0;
  ni->actIndex = 0;
  ni->indices[2].nsi = 0;
  ni->iC[0][0] = iC[0][0];
  ni->iC[0][1] = iC[0][1];
  ni->iC[1][0] = iC[1][0];
  ni->iC[1][1] = iC[1][1];
  sortIndices (angle0, angle1, ni->si);
}
/*---------------------------------------------------------------------------*/

static bool
getNextNewIndex (ni)

     nextIndexType * ni;

{
  unsigned int tmp;

  while (1)
    switch (ni->len) {
    case 0:
      if (ni->actIndex < 12) {
	InitPower(ni->indices[0]);
	(void) addPower (ni->indices[0], ni->si[ni->actIndex].ident,
			 ni->iC, &(ni->indices[0]));
	if ((ni->actIndex < 11) &&
	    (ni->si[ni->actIndex].ident.coord ==
	     ni->si[ni->actIndex + 1].ident.coord)) {
	  InitPower(ni->indices[1]);
	  (void) addPower (ni->indices[1], ni->si[ni->actIndex + 1].ident,
			   ni->iC, &(ni->indices[1]));
	  ni->nofPowers = 2;
	} else {
	  ni->nofPowers = 1;
	}
	ni->len = 1;
	return 1;
      } else {
	printf ("ERROR: all index enumeration possibilities exhausted!\n");
	exit(1);
      }
      break;
    case 1:
      if (ni->nofPowers == 1)
	if (addPower (ni->indices[0], ni->si[ni->actIndex].ident,
		      ni->iC, &(ni->indices[0]))) {
	  return 1;
	} else {
	  (ni->actIndex)++;
	  ni->len = 0;
	}
      else {
	tmp = 0;
	if (addPower (ni->indices[0], ni->si[ni->actIndex].ident,
		      ni->iC, &(ni->indices[tmp])))
	  tmp++;
	if (addPower (ni->indices[1], ni->si[ni->actIndex + 1].ident,
		      ni->iC, &(ni->indices[tmp])))
	  tmp++;
	InitPower(ni->indices[tmp]);
	(void) addPower (ni->indices[tmp], ni->si[ni->actIndex].ident,
			 ni->iC, &(ni->indices[tmp]));
	(void) addPower (ni->indices[tmp], ni->si[ni->actIndex + 1].ident,
			 ni->iC, &(ni->indices[tmp]));
	tmp++;
	ni->nofPowers = tmp;
	ni->len = 2;
	return 1;
      }
      break;
    case 2:
      if (ni->nofPowers == 1) {
	if ((ni->actIndex < 11) &&
	    (ni->si[ni->actIndex].ident.coord ==
	     ni->si[ni->actIndex + 1].ident.coord)) 
	  ni->actIndex += 2;
	else
	  (ni->actIndex)++;
	ni->len = 0;
      } else {
	if (ni->nofPowers == 2) {
	  if (addPower (ni->indices[0], ni->si[ni->actIndex + 1].ident,
			ni->iC, &(ni->indices[0]))) {
	    ni->nofPowers = 1;
	    ni->len = 3;
	    return 1;
	  } else {
	    if (addPower (ni->indices[0], ni->si[ni->actIndex].ident,
			  ni->iC, &(ni->indices[0]))) {
	      ni->nofPowers = 1;
	      ni->len = 3;
	      return 1;
	    } else {
	      ni->actIndex += 2;
	      ni->len = 0;
	    }
	  }
	} else {
	  tmp = 0;
	  if (addPower (ni->indices[0], ni->si[ni->actIndex + 1].ident,
			ni->iC, &(ni->indices[tmp])))
	    tmp++;
	  if (addPower (ni->indices[1], ni->si[ni->actIndex].ident,
			ni->iC, &(ni->indices[tmp])))
	    tmp++;
	  if (tmp == 0) {
	    ni->len = 0;
	    ni->actIndex += 2;
	  } else {
	    ni->nofPowers = tmp;
	    ni->len = 3;
	    return 1;
	  }
	}
      }
      break;
    case 3:
      if (ni->nofPowers == 1) {
	ni->actIndex += 2;
	ni->len = 0;
      } else {
	if (addPower (ni->indices[0], ni->si[ni->actIndex + 1].ident,
		      ni->iC, &(ni->indices[0]))) {
	  ni->nofPowers = 1;
	  ni->len = 4;
	  return 1;
	} else {
	  ni->len = 0;
	  ni->actIndex += 2;
	}
      }
      break;
    case 4:
      ni->actIndex += 2;
      ni->len = 0;
      break;
    }
}

/*---------------------------------------------------------------------------*/
static void
initPowerType (ps, xi, angle0, angle1)

     powerStackType * ps;
     unsigned int xi[2][2];
     longAngleType * angle0, * angle1;

{
  ps->top = ps->power - 1;
  ps->stop = ps->scan = ps->power;
  initIndexType (&(ps->ni), xi, angle0, angle1);
}
/*---------------------------------------------------------------------------*/

static void
getNextPower (ps)

     powerStackType * ps;

{
  powerType * sc;
  int i;

  (ps->top)++;
  ps->act = ps->top;
    
  do {
    if (ps->scan >= ps->stop) {
      (void) getNextNewIndex (&(ps->ni));
      ps->scan = ps->power;
      if (ps->ni.len == 1)
	ps->stop = ps->top;
      for (i = 0; i < ps->ni.nofPowers; i++) {
	*(ps->top) = ps->ni.indices[i];
	((ps->top)++)->nsi = 1;
      }
    } else {
      for (i = 0; i < ps->ni.nofPowers; i++) {
	sc = ps->scan - 1;
	do {
	  sc++;
	  if (addMultiplePowers (*sc, ps->ni.indices[i], ps->ni.iC, ps->top)) {
	    ps->top->nsi = 1;
	    (ps->top)++;
	  }
	} while (sc->nsi);
      }
      ps->scan = sc + 1;
    }
  } while (ps->act >= ps->top);

  (ps->top)--;
  ps->top->nsi = 0;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
