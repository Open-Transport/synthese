#ifndef SYNTHESE_CARTO_DRAWABLELINE_H
#define SYNTHESE_CARTO_DRAWABLELINE_H

#include <string>
#include <vector>


#include "Drawable.h"

#include "15_env/Point.h"



namespace synthese
{
namespace env
{
    class Line;
}


namespace carto
{

class PostscriptCanvas;


class DrawableLine : public Drawable
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

    const synthese::env::Line* _line;   //!< Reference line
    const std::vector<const synthese::env::Point*> _points;  //!< Reference line points

    std::vector<int> _shifts;
    std::vector<bool> _shifted;
	
    mutable std::vector<synthese::env::Point> _shiftedPoints;

    RGBColor _color;

public:
    
    DrawableLine (const synthese::env::Line* line);
    ~DrawableLine();

    //! @name Getters/Setters
    //@{
    // const std::string& getShortName () const;
    int getLineNumber () const;
    // const RGBColor& getColor () const;

    const std::vector<const synthese::env::Point*>& 
	getPoints () const;
	
    int isShifted (int pointIndex) const;
    int getShift (int pointIndex) const;
    void setShift (int pointIndex, int shift);

    bool isFullyReverseWay (const DrawableLine* dbl) const;
    bool isFullySameWay (const DrawableLine* dbl) const;
    bool isReverseWayAt (const synthese::env::Point* p, const DrawableLine* dbl) const;
    int numberOfCommonPointsWith (const DrawableLine* dbl) const;

    virtual void preDraw (Map& map) const;
    
    virtual void draw (Map& map) const;
    
    virtual void postDraw (Map& map) const;

private:
    
    /** Find first point in this line points
	which is equal (by value) to a given point.
    */
    int firstIndexOf (const synthese::env::Point* p) const;

    const std::vector<synthese::env::Point> 
	calculateShiftedPoints (const std::vector<synthese::env::Point>& points) const;
    
    
    synthese::env::Point  
	calculateSingleShiftedPoint (const synthese::env::Point& a, 
				     const synthese::env::Point& b, 
				     double distance) const;
    synthese::env::Point  
	calculateSingleShiftedPoint (const synthese::env::Point& a, 
				     const synthese::env::Point& b, 
				     const synthese::env::Point& c, 
				     double distance) const;
    synthese::env::Point  
	calculateDoubleShiftedPoint (const synthese::env::Point& a, 
				     const synthese::env::Point& b, 
				     const synthese::env::Point& c, 
				     double incomingDistance, 
				     double outgoingDistance) const;
    
    void doDrawCurvedLine (PostscriptCanvas& canvas, 
                           const std::vector<synthese::env::Point>& shiftedPoints) const;
    
    void doDrawTriangleArrow (PostscriptCanvas& canvas, 
                              const synthese::env::Point& point, 
                              double angle) const;
    
    void doDrawSquareStop (PostscriptCanvas& canvas, 
                           const synthese::env::Point& point, 
                           double angle) const;
    
    void doDrawSquareTerminus (PostscriptCanvas& canvas, 
			       const synthese::env::Point& point, 
			       double angle) const;
    
    
};
 
}
}

#endif
