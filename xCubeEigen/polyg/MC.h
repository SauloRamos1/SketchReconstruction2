#ifndef MC_H
#define MC_H

#include <list>
#include "point3d.h"


#define RES 20 /* converge factor */

#define RAND()    	((rand()&32767)/32767.)  									// random number, 0--1
#define HASHBIT   	(5)
#define HSIZE     	(size_t)(1<<(3*HASHBIT)) 									// hash table size (32768)
#define MASK      	((1<<HASHBIT)-1)											// 31
#define HASH(i,j,k) ((((((i)&MASK)<<HASHBIT)|((j)&MASK))<<HASHBIT)|((k)&MASK))
#define BIT(i, bit) (((i)>>(bit))&1)

#define SP_L       0  /* left direction:   -x, -i */
#define SP_R       1  /* right direction:  +x, +i */
#define SP_B       2  /* bottom direction: -y, -j */
#define SP_T       3  /* top direction:    +y, +j */
#define SP_N       4  /* near direction:   -z, -k */
#define SP_F       5  /* far direction:    +z, +k */
#define SP_LBN     0  /* left bottom near corner  */
#define SP_LBF     1  /* left bottom far corner   */
#define SP_LTN     2  /* left top near corner     */
#define SP_LTF     3  /* left top far corner      */
#define SP_RBN     4  /* right bottom near corner */
#define SP_RBF     5  /* right bottom far corner  */
#define SP_RTN     6  /* right top near corner    */
#define SP_RTF     7  /* right top far corner     */


/* test the function for a signed value */
typedef struct Test 
{                   
    Point3D pt;                   /* location of test */
    double value;               /* function value at p */
    int ok;                     /* if value is of correct sign */
} Test;



/* list of corners */
typedef struct Corner
{        
    int i, j, k;                 /* corner id */
    double value;                /* corner value */
    bool operator==(Corner c) const {return (c.i == i && c.j == j && c.k == k);}
} Corner;

typedef std::list<Corner> Cornerlist;

/* partitioning cell (cube) */
typedef struct Cube 
{                   
    int i, j, k;                   /* lattice location of cube */
    Corner corners[8];        	   /* eight corners */
} Cube;

/* list of cube locations */
typedef struct Center
{        
    int i, j, k;               		/* cube location */
} Center;

typedef std::list<Center> Centerlist;

/* list of edges */
typedef struct MC_Edge
{          
public:
    int i1, j1, k1, i2, j2, k2;    /* edge corner ids */
    int vid;                       /* vertex id */
    bool operator==(MC_Edge e) const {return (e.i1 == i1 && e.j1 == j1 && e.k1 == k1 && e.i2 == i2 && e.j2 == j2 && e.k2 == k2);}
} MC_Edge;

typedef std::list<MC_Edge> Edgelist;



#endif 
