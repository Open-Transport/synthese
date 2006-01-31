#include "Polygon.h"


#include "XYPoint.h"

namespace synmap
{

Polygon::Polygon(const std::vector<const XYPoint*>& points)
: _points (points)
{
}

Polygon::~Polygon()
{
}



bool 
Polygon::hasPoint (const XYPoint& point) const
{
	for (unsigned int i=0; i<_points.size(); ++i) {
		if (*(_points[i]) == point) return true;
	}	
	return false;
}


}


