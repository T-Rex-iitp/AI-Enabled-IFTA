//---------------------------------------------------------------------------

#ifndef TriangulatPolyH
#define TriangulatPolyH
typedef double pfVec3[3];

typedef struct _ttrianles {
    long	*indexList;
	struct _ttrianles *next;
} TTriangles;


#define CLOCKWISE 1
#define COUNTERCLOCKWISE -1


int  orientation2D_Polygon( pfVec3* V,int n );
long triangulatePoly(pfVec3 *Verts,int NumVerts,  TTriangles **);
bool checkComplex(pfVec3* p,int n );

//---------------------------------------------------------------------------
#endif
