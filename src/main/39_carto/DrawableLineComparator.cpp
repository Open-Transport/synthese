#include "DrawableLineComparator.h"

#include "DrawableLine.h"
#include "Geometry.h"

#include "15_env/Line.h"
#include "15_env/Point.h"

#include <cmath>
#include <iostream>
#include <algorithm>

using synthese::env::Point;

using namespace std;

namespace synthese
{
namespace carto
{


DrawableLineComparator::DrawableLineComparator(
    const DrawableLine* reference, 
    const Point* referencePoint, 
    const Point* point)
    : _reference (reference)
    , _referencePoint (referencePoint)
    , _point (point)
{
}



DrawableLineComparator::~DrawableLineComparator()
{
}


int 
DrawableLineComparator::firstIndexOf (const Point* point, 
				      const std::vector<const Point*>& points) const
{
    for (unsigned int i=0; i<points.size (); ++i)
    {
	if ((*(points[i])) == (*point)) return i;	 	
    }
    return -1;
}




void 
DrawableLineComparator::setPoint (const Point* point)
{
    _point = point;	
}



double
DrawableLineComparator::calculateStartAngleAtIndex (
    const std::vector<const Point*>& points, 
    int index) const
{
    if (index+2 >= (int) points.size ()) return -1;
    
    const Point* vs0_1 = (const Point*) points[index];
    const Point* vs1_1 = (const Point*) points[index + 1];
    const Point* vs2_1 = (const Point*) points[index + 2];
    
    double angle = calculateAngle (*vs0_1, *vs1_1, *vs2_1);
    if (angle < 0) angle += 2*M_PI;
    return angle;
}




double
DrawableLineComparator::calculateEndAngleAtIndex (
    const std::vector<const Point*>& points, 
    int index) const
{
    if (index < 2) return -1;
    
    const Point* vs0_1 = (const Point*) points[index - 2];
    const Point* vs1_1 = (const Point*) points[index - 1];
    const Point* vs2_1 = (const Point*) points[index];
    
    double angle = calculateAngle (*vs0_1, *vs1_1, *vs2_1);
    if (angle < 0) angle += 2*M_PI;
    return angle;
}





int 
DrawableLineComparator::operator() (const DrawableLine* bl1, 
				    const DrawableLine* bl2) const 
{
    // Special case : the lines follow exactly reverse ways
    if (bl1->isFullyReverseWay(bl2)) {
	bool result = 1;	
	if (bl1->isReverseWayAt(_referencePoint, _reference)) result = !result;
	return result;
    }
    
    // Special case : the lines follow exactly same ways
    if (bl1->isFullySameWay(bl2)) {
	bool result = bl1->getShortName () > bl2->getShortName ();
	if (bl1->isReverseWayAt(_referencePoint, _reference)) result = !result;
	return result;
    }
    
    // TODO Other special cases to be handled...
    
    std::vector<const Point*> points1 = bl1->getPoints ();
    std::vector<const Point*> points2 = bl2->getPoints ();
    
    double sangle1 = -1; double sangle2 = -1;
    double eangle1 = -1; double eangle2 = -1;
    
    int index1_1, index1_2, index2_1, index2_2;
    
    bool mustReverseLine2 = bl2->isReverseWayAt (_point, bl1);
    if (mustReverseLine2) std::reverse (points2.begin (), points2.end ());
    
    const Point* curPoint = _point;
    index1_1 = firstIndexOf(curPoint, points1);
    index1_2 = firstIndexOf(curPoint, points2);
    
    if ((index1_1 == -1) || (index1_2 == -1)) return 0; // ????
    
    while ((index1_2 >= 0) &&
	   (index1_1 >= 0) &&
	   (firstIndexOf (curPoint, points1) != -1)) 
    {
	--index1_2;
	--index1_1;
	if (index1_2 >=0) curPoint = points2[index1_2];
    }
    
    if (index1_1 >= 0) {
	// Consider sangle1
	sangle1 = calculateStartAngleAtIndex (points1, index1_1);
    }
    
    if (index1_2 >= 0) {
	// Consider sangle1
	sangle2 = calculateStartAngleAtIndex (points2, index1_2);
    }
    
    curPoint = _point;
    index2_1 = firstIndexOf(curPoint, points1);
    index2_2 = firstIndexOf(curPoint, points2);
    
    while (
	   (index2_2 < (int) points2.size ()) &&
	   (index2_1 < (int) points1.size ()) &&
	   (firstIndexOf (curPoint, points1) != -1)) 
    {
	++index2_2;
	++index2_1;
	if (index2_2 <= points2.size ()-1) curPoint = points2[index2_2];
    }
    
    if (index2_1 < (int) points1.size ()) {
	// Consider eangle1
	eangle1 = calculateEndAngleAtIndex (points1, index2_1);
    }
	
    if (index2_2 < (int) points2.size ()) {
	// Consider eangle2
	eangle2 = calculateEndAngleAtIndex (points2, index2_2);
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

    if (_reference->isReverseWayAt (_referencePoint, bl1)) result = !result;
	
    return result;
	
}		








}
}
