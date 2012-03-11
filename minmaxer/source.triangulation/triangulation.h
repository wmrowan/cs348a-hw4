typedef int triangleType[3];

typedef struct {
  triangleType *v; /* indices of vertices of triangle in ccw order. First */
	     /* triangle stored in v[0] */
  int nofTriangles; /* # triangles stored in v */
  int maxTriangles; /* size of v */
} triangleList;

extern int GLOBAL_INTERRUPT_ALGORITHM;
