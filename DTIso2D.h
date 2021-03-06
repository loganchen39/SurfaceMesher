/* *************************************************************
 * Delaunay triangulation in 2-dimensions
 *      with automatic point creation
 *
 * Chen Jianjun  Center for Engineering & Scientific Computation,
 * Zhejiang University, P. R. China
 * Copyright reserved, 2005, 04, 27
 * 
 * For further information, please conctact
 *  Tel: +86-571-87953165
 *  Fax: +86-571-87953167
 * Mail: zdchenjj@yahoo.com.cn
 * **************************************************************/
 
#ifndef __ISO_2D_H__
#define __ISO_2D_H__

#include <vector>
#include <list>
#include <stack>

#include "SurfaceMesher.h"

/*
 * use double or float type to define float variables according to
 * memory & precision considerations
 */
// #ifdef _DOUBLE
typedef double REAL;
// #else //_FLOAT
// typedef float REAL;
// #endif

/*
 * while more than billions elements are required, 
 * "long" type  is required
 */
#ifdef _LONG
typedef long INTEGER;
#else
typedef int INTEGER;
#endif 

/*
 * define some constants controlling memory allocation
 */
const REAL PROB_SIZE = 100.0;        /*while problem size increases, increasing the variable 
                                   correspondingly*/ 
#define INIT_ALLOC_BND_NUM ((INTEGER) (1000 * PROB_SIZE))    /*the initial number of the boundary nodes array*/
#define INIT_ALLOC_NOD_NUM ((INTEGER) (10000 * PROB_SIZE))  /*the initial number of the mesh nodes array*/
#define INIT_ALLOC_ELE_NUM ((INTEGER) (40000 * PROB_SIZE))   /*the initial number of the mesh elements array*/
#define NUM_ADD_FAC 0.25         /*an factor indicating how much more memories 
                                   (percentage to the current size) will be added 
                                   while the memory bound is reached*/

/*
 * problem dimensions & initial triangulations
 */
#define DIM 2                   /*problem dimensions*/
#define INIT_TRI_NUM 2          /*the number of the initial triangles*/
#define INIT_NOD_NUM 4          /*the number of the initial nodes(redundant)*/

/* 
 * an indices refers to an empty neighbor 
 */
#define NULL_NEIG -1
#define NULL_ELEM -1
 
/*
 * epsilon value definition
 */
#ifdef _DOUBLE
#define EPS_ZERO_SQ (1e-15)
#else
#define EPS_ZERO_SQ (1e-5)
#endif

#define EPS_DISTURB (1e-4)

/*
 * define some types
 */
 
 /* boundary data*/
typedef struct Bnd
{
	INTEGER beg, end;	/*begin & end node indices*/
	int curve, loop;  /*curve & loop indices*/
	INTEGER ele;      /*element index it belongs to*/
} Bnd;

/* POINT */
typedef REAL POINT[2];

#define X 0
#define Y 1
#define Z 2

/* NODE */
typedef struct Node
{
	POINT pt;
	REAL spacing;
	INTEGER iReserved;
} Node;

/* neighboring elements & forming points */
typedef INTEGER NEIG_ARR[DIM+1];
typedef NEIG_ARR FORM_ARR;
typedef POINT FORM_PNTS[DIM+1];

/* elements (triangle in 2D & tetrahedral in 3D) */
typedef struct Elem
{
	POINT cen;     /*center of circumcircle*/
	REAL rad;      /*radius of circumcircle*/
	NEIG_ARR neig;  /*neigboring element indices*/
	FORM_ARR form; /*forming points*/  
	INTEGER iReserved;
} Elem;

/* disturbance info. */
typedef struct DistInfo
{
	INTEGER iNod;  /* disturbing point index */
	POINT old_pt;  /* old position of disturbing point, 
				      utilized for restoring the disturbance */
} DistInfo;

/* edge for swapping. */
typedef struct SwapEdge
{
	INTEGER iDiag[DIM];
	INTEGER iElemLeft;
	INTEGER iElemRight;
} SwapEdge;

/* inner point*/
typedef struct InnerPnt
{
	INTEGER iNod;
	INTEGER iEle;
} InnerPnt;

/* point source*/
typedef struct PointSource
{
	POINT pt;
	REAL rInnerRad;
	REAL rOuterRad;
	REAL rIntensity;
} PointSource;

/* line source*/
typedef struct LineSource
{
	PointSource points[2];
} LineSource;

/* triangle source*/
typedef struct TriangleSource
{
	PointSource points[3];
} TriangleSource;
/*
 * for scaling the coordinates
 */
const POINT g_minN = {0.0, 0.0};
const POINT g_maxN = {1.0, 1.0};

/*
 * corner points & forming points & neigboring elements of the initial triangulations
 */
const POINT g_cors[] = 
{
	{-1.0, -1.0}, {-1.0, 25.0}, {22.0, -1.0}, {22.0, 25.0}
};
const FORM_ARR g_form[] =
{ 
	{0, 2, 1}, {1, 2, 3}
};
const NEIG_ARR g_neig[] =
{
	{1, NULL_NEIG, NULL_NEIG}, {NULL_NEIG, NULL_NEIG, 0}
};

extern REAL g_alpha;
/*
 * declaration of class DTIso2D
 */
class DTIso2D
{
public:
	int updateBndParent();
	bool clrDelEles();
	bool undoInsertion();
	bool clrTakNods();
	bool clrAddEles();
	int writeDt2(char *fname);
	int writePl2(char *fname);
	int writeOff(char *fname); // added by clg at 20071124-1137
	/*
	 * constructors & destructor
	 */
	DTIso2D();
	virtual ~DTIso2D();
		 
	/*
	 * create initial environment
	 * (incluing allocating memory & setup initial triangulation)
	 */
	bool crtEnv();

	// extended interface for set the boundary information juse like readFr2
	// bool initBouInfo(std::vector<MeshPoint2D> *_bou_node);
	bool initBouInfo(std::vector<std::vector<MeshPoint2D>*> &_bou_node);
 
	/*
	 * read data from *.fr2 file
	 */
	bool readFr2(const char* fname);
	
	/*
	 * read data from *.ba2 file
	 */
	bool readBa2(const char* fname);

	/*
	 * calculate outer box
	 */
	bool calcBox(POINT *minW, POINT* maxW);
	
	/*
	 *	scale the geometry
	 */
	bool scaGeom();

	/*
	 * scale the geometry back
	 */ 
	bool rescaGeom();

	/*
	 * calculate spacing values 
	 */
	bool calcDens();
	
	/*
	 * setup initial triangulation
	 */
	int setupInitTri();
	
	/*
	 * find first elements including the point
	 */
	int findFirstEle(POINT pnt, INTEGER *ele);
	
	/*
	 * boundary point insertion
	 */
	int bndPntInst();
	
	/* 
	 * add boundary point
	 */
	int addBndPnt(POINT pt, INTEGER iNod);
	 
	/*
	 * add inner point
	 */
	int addInnerPnt(INTEGER iNod, INTEGER iEle);

	/*
     * create new inner points, using gravity center. The new created inner nodes' info is 
     * stored in m_lstInstInnerNods.
     */
	int crtInnerPnts();

	/*
	 * inner point insertion
	 */
	int innerPntInst();
	/* --------------------------------------
	 * function for tree searching          |
	 * -------------------------------------*/
	INTEGER pickTreeSearch();
	int addTreeSearch(INTEGER iEle);
	bool isTreeSearchEmpty();
	bool clrTreeSearch();

	/* -----------------------------------------
	 * function for deleted elements          |
	 * ---------------------------------------*/
	int addDeleted(INTEGER iEle);

	/* ------------------------------------------------------
	 * function for location management of new elements     |
	 * -----------------------------------------------------*/
	INTEGER getNewEleLoc();

	/*
	 * update the empty location vector after a point is added
	 */
	bool updateNewEleLoc();

	/* ------------------------------------------------------
	 * function for management of node-taken elements       |
	 * -----------------------------------------------------*/
	INTEGER getNodTakEle(INTEGER iNod);
	bool addNodTakEle(INTEGER iNod, INTEGER iEle);

	/*
	 *update neighboring info. of elements near the cavity
	 */
	bool updateNeigInfo();

	/*
	 * check if an elememt is deleted
	 */
	bool isDelEle(INTEGER iEle);

	/*
	 * check if an elememt is deleted
	 */
	bool isTstEle(INTEGER iEle);

	/*
	 * add an element to tested vector
	 */
	bool addTstEle(INTEGER iEle);

	/*
	 * enable test flag of an element
	 */
	bool enabEleTst(INTEGER iEle, bool flag);

	/*
	 * clear flags of all tested elements
	 */
	bool clrTstEles();

	/*
	 * calcuate element parameters
	 * d_a: double area
	 * cen: center of circumcenter
	 * rad: radius of circumcenter
	 */
	bool calcElePar(POINT pnt[], REAL *d_a, POINT *cen, REAL *rad);

	/*
	 * create a new element
	 */
	INTEGER crtEle(INTEGER form[], POINT cen, REAL rad);
	
	/* ---------------------------------
	 * for disturbance point info.     |
	 * ---------------------------------*/
	bool isDisturbed(INTEGER iNod);

	/*
     * Record the disturbance info for given node, so it can be restored later.
     */
	bool addDistInfo(INTEGER iNod, POINT pnt);

	/* recover disturbed points */
	bool recDistPnts();

	/* ---------------------------------
	 * for normalizing the coordinates |
	 * ---------------------------------*/
	/*
	 * scale the coordinates range [minW, maxW] to a range of [minN, maxN],
	 * and return according center & scale value
	 */
	bool scaleFactor(POINT minW, POINT maxW, POINT minN, POINT maxN);
						
	/* 
	 * world coordinates to normalization coordinates 
	 */
	bool wToN(POINT wp, POINT *np); 

	/* 
	 * normalization coordinates to world coordinates 
	 */
	bool nToW(POINT np, POINT *wp);		

	/*
	 * recover all boundarys
	 */
	bool recoverBnds();

	/*
	 * check whether swapping has finished
	 */
	bool isSwapingFinished();

	/*
	 * swap the current edge according the iBnd-th boundary
     */
	bool swapEdge(INTEGER iBnd);

	/*
	 * add the edge need swapping
	 */
	bool addSwapEdge(SwapEdge *edge);

	/*
	 * delete current edge
	 */
	bool delSwapEdge();

	/*
	 * current swapping edge
	 */
	SwapEdge * curSwapEdge();

	/*
	 * previous swapping edge
	 */
	SwapEdge * prevSwapEdge();

	/*
	 * next swapping edge
	 */
	SwapEdge * nextSwapEdge();

	/*
	 * move to next edge
	 */
	bool moveAlong();

	/*
	 * clear all outer elements
	 */
	bool clrOuterEles();

	/*
	 *	scale the background mesh and source control
	 */
	bool scaBkGrnd();

	/*
	 * computes the spacing at a point from a source point
	 */ 
	REAL spacFrmPnt(PointSource src, POINT pt);

	/*
	 * computes the spacing at a point from a source line
	 */ 
	REAL spacFrmLne(PointSource src1, PointSource src2, POINT pt);

	/*
	 * computes the spacing at a point from background mesh 
	 */
	REAL spacFrmBkGrnd(POINT pt);

	/*
	 * remove empty elements
	 */
	int rmvEmpEles();
	/*
	 * remove empty nodes
	 */
	int rmvEmpNods();
	/*
	 * smoothing
	 */
	int smooth();

	/*
	 * output information
	 */
	int output();

	/*
	 * checks whether the incircle criteria for given element is broken if specified point is inserted
	 * iEle: element index
	 * pt: point to be inserted
	 * return: 1 if incircle criteria is broken
	 *         0 if four points in the same circle
	 *         -1 if incircle criteria is kept
	 */
	int DTIso2D::isElemBroken(INTEGER iEle, POINT pt); 

//protected:
public:
	/*basic data structures*/
	Elem *m_pElems;			/* elements array */
	Node *m_pNodes;         /* nodes array */
	Bnd* m_pBnds;           /* boundaries array */
	
	INTEGER m_nElems;       /* the number of elements */
	INTEGER m_nNodes;       /* the number of nodes */
	INTEGER m_nBnds;        /* the number of boundaries */
	
	/* for dynamical memory allocation */
	INTEGER m_nAllocElems;  /*allocated data size for elements*/
	INTEGER m_nAllocNodes;  /*allocated data size for nodes*/
	INTEGER m_nAllocBnds;   /*allocated data size for boundaries*/
		
	/* scale factor & scale center*/
	REAL m_scale;						/*scale factor*/
	POINT m_cenW;	          /*world coordinates of center points*/
	POINT m_cenN; 					/*normalization coordinates of center points*/
	POINT m_minW, m_maxW;   /*world coordinates range*/
	POINT m_minN, m_maxN;   /*normalization coordinates range*/
	
	/*a stack of elements for tree search*/
	std::stack<INTEGER> m_stkTreeSear;
			
	/*a vector of deleted elements*/
	std::vector<INTEGER> m_vecDelEles;
		
	/*a vector of empty locations*/
	std::vector<INTEGER> m_vecEmpLocs;
	int m_nLocInd; /*location indicator*/
				
	/*a vector of added elements*/
	std::vector<INTEGER> m_vecAddEles;

	/*a vector of tested elements*/
	std::vector<INTEGER> m_vecTstEles;

	/*a vector of taken nodes*/
	std::vector<INTEGER> m_vecTakNods;

	/*a list of boundary node list (sorted by the inserting order)*/
	std::list<INTEGER> m_lstInstBndNods;

	/* a list of disturbance information */
	std::list<DistInfo*> m_lstDistInfo;

	/* a list of edge need for swapping */
	std::list<SwapEdge*> m_lstSwapEdge;
	int m_nCurInd; /*current indicator*/

	/* a list of inner node list (sorted by the inserting order)*/
	std::list<InnerPnt*> m_lstInstInnerNods;
	INTEGER m_nInners;      /* the number of inner points need inserting */

	PointSource *m_pntSources; /* point sources array */
	LineSource *m_lineSources; /* line sources array */
	TriangleSource *m_triSources; /* triangle sources array */
	INTEGER m_nPntSrcNum; /* the number of point sources */
	INTEGER m_nLneSrcNum; /* the number of line sources */
	INTEGER m_nTriSrcNum; /* the number of triangle sources */
	
	Elem * m_pBkGrndElem; /*background element array*/
	Node * m_pBkGrndNode; /*background node array */
	INTEGER m_nBkGrndElem;/*the number of background elements*/
	INTEGER m_nBkGrndNode;/*the number of background nodes*/
};

/*
 * calculate square of distance between two points
 */
REAL squaDist(POINT p1, POINT p2);

#endif //#define __ISO_2D_H__
 