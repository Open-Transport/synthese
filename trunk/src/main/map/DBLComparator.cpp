#include "DBLComparator.h"

#include "DrawableBusLine.h"
#include "Geometry.h"
#include "Itinerary.h"
#include "Vertex.h"

#include <cmath>
#include <iostream>
#include <algorithm>


using namespace std;

namespace synmap
{

DBLComparator::DBLComparator(const DrawableBusLine* reference, const Vertex* referenceVertex, const Vertex* vertex)
: _reference (reference)
, _referenceVertex (referenceVertex)
, _vertex (vertex)
{
}

DBLComparator::~DBLComparator()
{
}


int 
DBLComparator::firstIndexOf (const Vertex* vertex, const std::vector<const Vertex*>& vertices) const
{
	for (unsigned int i=0; i<vertices.size (); ++i)
	{
		if (vertices[i] == vertex) return i;	 	
	}
	return -1;
}


void 
DBLComparator::setVertex (const Vertex* vertex)
{
	_vertex = vertex;	
}



double
DBLComparator::calculateStartAngleAtIndex (const std::vector<const Vertex*>& vertices, int index) const
{
	if (index+2 >= (int) vertices.size ()) return -1;
	
	const XYPoint* vs0_1 = (const XYPoint*) vertices[index];
	const XYPoint* vs1_1 = (const XYPoint*) vertices[index + 1];
	const XYPoint* vs2_1 = (const XYPoint*) vertices[index + 2];
			
	double angle = calculateAngle (*vs0_1, *vs1_1, *vs2_1);
	if (angle < 0) angle += 2*M_PI;
	return angle;
}




double
DBLComparator::calculateEndAngleAtIndex (const std::vector<const Vertex*>& vertices, int index) const
{
	if (index < 2) return -1;
	
	const XYPoint* vs0_1 = (const XYPoint*) vertices[index - 2];
	const XYPoint* vs1_1 = (const XYPoint*) vertices[index - 1];
	const XYPoint* vs2_1 = (const XYPoint*) vertices[index];
			
	double angle = calculateAngle (*vs0_1, *vs1_1, *vs2_1);
	if (angle < 0) angle += 2*M_PI;
	return angle;
}





int 
DBLComparator::operator()(const DrawableBusLine* bl1, const DrawableBusLine* bl2) const {
	
	// Special case : the lines follow exactly reverse ways
	if (bl1->isFullyReverseWay(bl2)) {
		bool result = 1;	
		if (bl1->isReverseWayAt(_referenceVertex, _reference)) result = !result;
		return result;
	}
	
	// Special case : the lines follow exactly same ways
	if (bl1->isFullySameWay(bl2)) {
		bool result = bl1->getLineNumber() > bl2->getLineNumber ();
		if (bl1->isReverseWayAt(_referenceVertex, _reference)) result = !result;
		return result;
	}
    
    // TODO Other special cases to be handled...
	
	std::vector<const Vertex*> vertices1 = bl1->getItinerary ()->getVertices ();
	std::vector<const Vertex*> vertices2 = bl2->getItinerary ()->getVertices ();

	double sangle1 = -1; double sangle2 = -1;
	double eangle1 = -1; double eangle2 = -1;

	int index1_1, index1_2, index2_1, index2_2;

	bool mustReverseLine2 = bl2->isReverseWayAt (_vertex, bl1);
	if (mustReverseLine2) std::reverse (vertices2.begin (), vertices2.end ());
	
	const Vertex* curVertex = _vertex;
	index1_1 = firstIndexOf(curVertex, vertices1);
	index1_2 = firstIndexOf(curVertex, vertices2);
	
	if ((index1_1 == -1) || (index1_2 == -1)) return 0; // ????

	while ((firstIndexOf (curVertex, vertices1) != -1) && 
	       (index1_2 >= 0) &&
	       (index1_1 >= 0)) {
		--index1_2;
		--index1_1;
		curVertex = vertices2[index1_2];
	}
	
	if (index1_1 >= 0) {
		// Consider sangle1
		sangle1 = calculateStartAngleAtIndex (vertices1, index1_1);
	}
	
	if (index1_2 >= 0) {
		// Consider sangle1
		sangle2 = calculateStartAngleAtIndex (vertices2, index1_2);
	}
	
	curVertex = _vertex;
	index2_1 = firstIndexOf(curVertex, vertices1);
	index2_2 = firstIndexOf(curVertex, vertices2);
	
	while ((firstIndexOf (curVertex, vertices1) != -1) && 
		   (index2_2 < (int) vertices2.size ()) &&
		   (index2_1 < (int) vertices1.size ())) {
		++index2_2;
		++index2_1;
		curVertex = vertices2[index2_2];
	}
		
	if (index2_1 < (int) vertices1.size ()) {
		// Consider eangle1
		eangle1 = calculateEndAngleAtIndex (vertices1, index2_1);
	}
	
	if (index2_2 < (int) vertices2.size ()) {
		// Consider eangle2
		eangle2 = calculateEndAngleAtIndex (vertices2, index2_2);
	}
	
	if ((sangle1 == -1) && (eangle1 == -1)) {
		sangle1 = M_PI;
		eangle1 = M_PI;
	}
	if ((sangle2 == -1) && (eangle2 == -1)) {
		sangle2 = M_PI;
		eangle2 = M_PI;
	}
	
	bool result = true;
	if ((sangle1 != -1) && (sangle2 != -1)) result = result && (sangle1 <= sangle2);
	if ((eangle1 != -1) && (eangle2 != -1)) result = result && (eangle1 <= eangle2);

    if (_reference->isReverseWayAt (_referenceVertex, bl1)) result = !result;
	
	return result;
	
}		









}
