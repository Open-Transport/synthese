#ifndef XYPOINT_H_
#define XYPOINT_H_

namespace synmap
{


class Polygon;


class XYPoint
{
private:
	double _x;
	double _y;
	
public:

	XYPoint(double x, double y);
	XYPoint(const XYPoint& ref);
	
	virtual ~XYPoint();
	
	double getX () const { return _x; }
	double getY () const { return _y; }
	
	void setX (double x) { _x = x; }
	void setY (double y) { _y = y; }
	
	double distanceTo (const XYPoint& p) const;
	
	XYPoint& operator= (const XYPoint& rhs);
	bool operator== (const XYPoint& rhs) const;
	
	
};

}

#endif /*XYPOINT_H_*/
