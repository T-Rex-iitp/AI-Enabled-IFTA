//---------------------------------------------------------------------------


#pragma hdrstop

#include "TriangulatPoly.h"
#include <stdlib.h>
#include <mem.h>
#include <vcl.h>
//---------------------------------------------------------------------------

#pragma package(smart_init)
static long    decompConcave(pfVec3 *Verts,int NumVerts,  TTriangles **, long, long, long);
static bool intersect( pfVec3* p,int n, int i1, int i2 );
static bool ccw( double p1x, double p1y, double p2x, double p2y, double p3x, double p3y );



long triangulatePoly(pfVec3 *Verts,int NumVerts,  TTriangles ** tlist)
{
	double  *p0, *p1;
	double  as[3], dx1, dy1, dx2, dy2, max;
    long    i, j, flag, asum, csum, index, x, y;
	 TTriangles           *tri;
    pfVec3 *temp_coord;


	/* calculate signed areas */
    as[0] = as[1] = as[2] = 0.0;

	for (i = 0; i < NumVerts; i++)
	{
	p0 = Verts[i];
	p1 = Verts[(i + 1) % NumVerts];
	as[0] += p0[0] * p1[1] - p0[1] * p1[0];
	as[1] += p0[0] * p1[2] - p0[2] * p1[0];
	as[2] += p0[1] * p1[2] - p0[2] * p1[1];
    }

    /* select largest signed area */
    for (max = 0.0, index = 0, i = 0; i < 3; i++)
	if (as[i] >= 0.0)
	{
	    if (as[i] > max)
	    {
		max = as[i];
		index = i;
		flag = 1;
	    }
	}
	else
	{
	    as[i] = -as[i];
	    if (as[i] > max)
	    {
		max = as[i];
		index = i;
		flag = 0;
	    }
	}

    /* pointer offsets */
    switch (index)
    {
    case 0:
	x = 0;
	y = 1;
	break;
    case 1:
	x = 0;
	y = 2;
	break;
    case 2:
	x = 1;
	y = 2;
	break;
    }

    /* concave check */
	p0 = Verts[0];
	p1 = Verts[1];
    dx1 = p1[x] - p0[x];
    dy1 = p1[y] - p0[y];
    p0 = p1;
	p1 = Verts[2];
    dx2 = p1[x] - p0[x];
    dy2 = p1[y] - p0[y];
    asum = ((dx1 * dy2 - dx2 * dy1 >= 0.0) ? 1 : 0);

	for (i = 0; i < NumVerts - 1; i++)
    {
	p0 = p1;
	p1 = Verts[(i + 3) % NumVerts];

	dx1 = dx2;
	dy1 = dy2;
	dx2 = p1[x] - p0[x];
	dy2 = p1[y] - p0[y];
	csum = ((dx1 * dy2 - dx2 * dy1 >= 0.0) ? 1 : 0);
	
if (!flag)
{
  temp_coord  = (pfVec3*)malloc((unsigned int)(sizeof(pfVec3)*NumVerts));
   for (i=0; i<NumVerts; i++)
   {
	temp_coord[i][0] = Verts[NumVerts-1-i][0];
	temp_coord[i][1] = Verts[NumVerts-1-i][1];
	temp_coord[i][2] = Verts[NumVerts-1-i][2];
   }
   for (i=0; i<NumVerts; i++)
   {
   Verts[i][0]=temp_coord[i][0];
   Verts[i][1]=temp_coord[i][1];
   Verts[i][2]=temp_coord[i][2];
   }
free(temp_coord);
flag=1;

}
	if (1) //*****  5/25/2025  temp fix  bug in convex alg below always use concave  if (csum ^ asum)
	{
		return decompConcave(Verts,NumVerts, tlist, flag, x, y);
	}
	}

    *tlist = NULL;

    /* Convert to triangles only */
	for (i = 0; i <= NumVerts - 3; i++)
    {
	tri = ( TTriangles *) malloc(sizeof( TTriangles));
	tri->indexList = (long *) malloc(sizeof(long) * 3);

	tri->indexList[0] = 0;
	for (j = 1; j < 3; j++)
	    tri->indexList[j] = j + i;

	tri->next = *tlist;
	*tlist = tri;
    }

    return NumVerts - 2;
}

typedef struct _Vtx
{
    long             index;
	pfVec3          coord;
    struct _Vtx    *next;
}               Vtx;

#define VX(p, c)    p->coord[c]

/*
 * Decompose concave polygon into triangles 
*/
static long
decompConcave(pfVec3 *Verts,int NumVerts,  TTriangles ** tlist, long asum, long x, long y)
{
	Vtx   *p0, *p1, *p2, *t0, *verts;
	double  xmin, xmax, ymin, ymax, a[3], b[3], c[3], s[3], *coord;
	long    i, init, csum;
	TTriangles           *tri;
    long             numPolys = 0;
    Vtx            *vList[5000];	/* Maximum of 5000 edges */

    *tlist = NULL;

    /* Make linked list of verts so routine matches GE code */
    verts = (Vtx *) malloc(sizeof(Vtx));
	verts->index = 0;
	coord = Verts[0];
	memcpy(verts->coord, coord,sizeof(pfVec3));
	p1 = verts;
	vList[0] = verts;
    for (i = 1; i < NumVerts; i++)
	{
	p0 = (Vtx *) malloc(sizeof(Vtx));
	vList[i] = p0;
	p0->index = i;
	coord = Verts[i];
	memcpy(p0->coord, coord,sizeof(pfVec3));
	p1->next = p0;
	p1 = p0;
    }
    p1->next = verts;

    p0 = verts;
    p1 = p0->next;
    p2 = p1->next;

    while (p0 != p2->next)
    {
	a[0] = VX(p1, y) - VX(p2, y);
	b[0] = VX(p2, x) - VX(p1, x);
	a[2] = VX(p0, y) - VX(p1, y);
	b[2] = VX(p1, x) - VX(p0, x);

	csum = ((b[0] * a[2] - b[2] * a[0] >= 0.0) ? 1 : 0);

	/* current angle is concave */
	if (csum ^ asum)
	{
	    p0 = p1;
	    p1 = p2;
	    p2 = p2->next;
	}

	/* current angle is convex */
	else
	{
	    xmin = (VX(p0, x) < VX(p1, x)) ? VX(p0, x) : VX(p1, x);
	    if (xmin > VX(p2, x))
		xmin = VX(p2, x);

	    xmax = (VX(p0, x) > VX(p1, x)) ? VX(p0, x) : VX(p1, x);
	    if (xmax < VX(p2, x))
		xmax = VX(p2, x);

	    ymin = (VX(p0, y) < VX(p1, y)) ? VX(p0, y) : VX(p1, y);
	    if (ymin > VX(p2, y))
		ymin = VX(p2, y);

	    ymax = (VX(p0, y) > VX(p1, y)) ? VX(p0, y) : VX(p1, y);
	    if (ymax < VX(p2, y))
		ymax = VX(p2, y);

	    for (init = 1, t0 = p2->next; t0 != p0; t0 = t0->next)
		if (VX(t0, x) >= xmin && VX(t0, x) <= xmax &&
		    VX(t0, y) >= ymin && VX(t0, y) <= ymax)
		{
		    if (init)
		    {
			a[1] = VX(p2, y) - VX(p0, y);
			b[1] = VX(p0, x) - VX(p2, x);
			init = 0;
			c[0] = VX(p1, x) * VX(p2, y) - VX(p2, x) * VX(p1, y);
			c[1] = VX(p2, x) * VX(p0, y) - VX(p0, x) * VX(p2, y);
			c[2] = VX(p0, x) * VX(p1, y) - VX(p1, x) * VX(p0, y);
		    }

		    s[0] = a[0] * VX(t0, x) + b[0] * VX(t0, y) + c[0];
		    s[1] = a[1] * VX(t0, x) + b[1] * VX(t0, y) + c[1];
		    s[2] = a[2] * VX(t0, x) + b[2] * VX(t0, y) + c[2];

		    if (asum)
		    {
			if (s[0] >= 0.0 && s[1] >= 0.0 && s[2] >= 0.0)
			    break;
		    }
		    else
			if (s[0] <= 0.0 && s[1] <= 0.0 && s[2] <= 0.0)
			    break;
		}

	    if (t0 != p0)
	    {
		p0 = p1;
		p1 = p2;
		p2 = p2->next;
	    }
	    else
	    {
		tri = ( TTriangles *) malloc(sizeof( TTriangles));
		tri->indexList = (long *) malloc(sizeof(long) * 3);

		tri->indexList[0] = p0->index;
		tri->indexList[1] = p1->index;
		tri->indexList[2] = p2->index;

		tri->next = *tlist;
		*tlist = tri;

		numPolys++;

		p0->next = p1->next;
		p1 = p2;
		p2 = p2->next;
	    }
	}
    }

	tri = ( TTriangles *) malloc(sizeof( TTriangles));
	tri->indexList = (long *) malloc(sizeof(long) * 3);
    tri->indexList[0] = p0->index;
	tri->indexList[1] = p1->index;
	tri->indexList[2] = p2->index;
    tri->next = *tlist;
    *tlist = tri;
    numPolys++;

    for (i = 0; i < NumVerts; i++)
	free(vList[i]);

    return numPolys;
}

// orientation2D_Polygon(): tests the orientation of a simple polygon
//    Input:  int n = the number of vertices in the polygon
//            Point* V = an array of n+1 vertices with V[n]=V[0]
//    Return: >0 for counterclockwise 
//            =0 for none (degenerate)
//            <0 for clockwise
//    Note: this algorithm is faster than computing the signed area.
int orientation2D_Polygon( pfVec3* p,int n )
{
 int i,j,k;
   int count = 0;
   double z;

   if (n < 3)
      return(0);

   for (i=0;i<n;i++) {
      j = (i + 1) % n;
      k = (i + 2) % n;
	  z  = (p[j][0] - p[i][0]) * (p[k][1] - p[j][1]);
	  z -= (p[j][1] - p[i][1]) * (p[k][0] - p[j][0]);
      if (z < 0)
         count--;
      else if (z > 0)
         count++;
   }
   if (count > 0)
      return(COUNTERCLOCKWISE);
   else if (count < 0)
      return(CLOCKWISE);
   else
      return(0);
}

//---------------------------------------------------------------------------
bool checkComplex( pfVec3* p,int n )
{
 int i = 0, j;
 for( j = i + 2; j < n - 1; j++ )
   if( intersect( p, n, i, j ) ) return true;
   for( i = 1; i < n; i++ )
	for( j = i + 2; j < n; j++ )
	 if( intersect( p, n, i, j ) ) return true;
 return false;
}
//---------------------------------------------------------------------------
 static bool intersect( pfVec3* p,int n, int i1, int i2 )
 {
  int s1 = ( i1 > 0 ) ? i1 -1 : n - 1;
  int s2 = ( i2 > 0 ) ? i2 -1 : n - 1;
  return ccw( p[ s1 ][0], p[ s1 ][1], p[ i1 ][0], p[ i1 ][1], p[ s2 ][0], p[ s2 ][1] )
	  != ccw( p[ s1 ][0], p[ s1 ][1], p[ i1 ][0], p[ i1 ][1], p[ i2 ][0], p[ i2 ][1] )
	  && ccw( p[ s2 ][0], p[ s2 ][1], p[ i2 ][0], p[ i2 ][1], p[ s1 ][0], p[ s1 ][1] )
	  != ccw( p[ s2 ][0], p[ s2 ][1], p[ i2 ][0], p[ i2 ][1], p[ i1 ][0], p[ i1 ][1] );
 }
//---------------------------------------------------------------------------
static bool ccw( double p1x, double p1y, double p2x, double p2y, double p3x, double p3y )
{
  double dx1 = p2x - p1x;
  double dy1 = p2y - p1y;
  double dx2 = p3x - p2x;
  double dy2 = p3y - p2y;
  return dy1 * dx2 < dy2 * dx1;
}
//---------------------------------------------------------------------------

