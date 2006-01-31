#ifndef POLYGON_H_
#define POLYGON_H_

#include <vector>

namespace synmap
{
	
class XYPoint;
	

class Polygon
{
private: 
	const std::vector<const XYPoint*> _points;
	
	
public:
	Polygon(const std::vector<const XYPoint*>& points);
	virtual ~Polygon();
	
	const std::vector<const XYPoint*>& getPoints () const { return _points; }
	
	bool hasPoint (const XYPoint& point) const;
};

}

#endif /*POLYGON_H_*/
