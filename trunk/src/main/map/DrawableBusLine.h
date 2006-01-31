#ifndef DRAWABLEBUSLINE_H_
#define DRAWABLEBUSLINE_H_

#include <string>
#include <vector>

#include "Drawable.h"
#include "XYPoint.h"

namespace synmap
{

class Itinerary;
class PostscriptCanvas;
class Vertex;


class DrawableBusLine : public synmap::Drawable
{
private:

    typedef enum { NONE, SINGLE, DOUBLE } PointShiftingMode;

    static const PointShiftingMode POINT_SHIFTING_MODE;

    static const bool     ENABLE_CURVES;
    static const double   RADIUS;
	static const double   RADIUS_DELTA;
	static const int      LINE_WIDTH;
	static const double   SPACING;

	static const RGBColor BORDER_COLOR;
	static const int      BORDER_WIDTH;

	const Itinerary* _itinerary;

	std::vector<XYPoint> _points;

	std::vector<int> _shifts;
	std::vector<bool> _shifted;

    mutable std::vector<XYPoint> _shiftedPoints;

	std::string _name;
	int _lineNumber;
	RGBColor _color;

public:
	DrawableBusLine(const Itinerary* itinerary);
	virtual ~DrawableBusLine();

	const Itinerary* getItinerary () const { return _itinerary; }
	
	const std::string& getName () const { return _name; }
	int getLineNumber () const { return _lineNumber; }
	const RGBColor& getColor () const { return _color; }
	
	const XYPoint& getXYPoint (int index) const { return _points[index]; };
	
	void setShift (int pointIndex, int shift) { _shifts[pointIndex] = shift; _shifted[pointIndex] = true; }
	int getShift (int pointIndex) const { return _shifts[pointIndex]; }
	int isShifted (int pointIndex) const { return _shifted[pointIndex];  }

	bool isFullyReverseWay (const DrawableBusLine* dbl) const;
	bool isFullySameWay (const DrawableBusLine* dbl) const;
	
	bool isReverseWayAt (const Vertex* v, const DrawableBusLine* dbl) const;
	int numberOfCommonVerticesWith (const DrawableBusLine* dbl) const;

    virtual void preDraw (Map& map) const;
    
    virtual void draw (Map& map) const;

    virtual void postDraw (Map& map) const;

private:

	const std::vector<XYPoint> 
    calculateShiftedPoints (const std::vector<XYPoint>& points) const;
    
    
    XYPoint  
    calculateSingleShiftedPoint (const XYPoint& a, 
                                 const XYPoint& b, 
                                 double distance) const;
    XYPoint  
    calculateSingleShiftedPoint (const XYPoint& a, 
                                 const XYPoint& b, 
                                 const XYPoint& c, 
                                 double distance) const;
    XYPoint  
    calculateDoubleShiftedPoint (const XYPoint& a, 
                                 const XYPoint& b, 
                                 const XYPoint& c, 
                                 double incomingDistance, 
                                 double outgoingDistance) const;
    
	void doDrawCurvedLine (PostscriptCanvas& canvas, 
                           const std::vector<XYPoint>& shiftedPoints) const;
	
	void doDrawTriangleArrow (PostscriptCanvas& canvas, 
                              const XYPoint& point, 
                              double angle) const;
                              
	void doDrawSquareStop (PostscriptCanvas& canvas, 
                           const XYPoint& point, 
                           double angle) const;
                           
    void 
    doDrawSquareTerminus (PostscriptCanvas& canvas, 
                          const XYPoint& point, 
                          double angle) const;
                           

};

}

#endif /*DRAWABLEBUSLINE_H_*/
