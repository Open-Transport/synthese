#include "DrawableLine.h"

#include "Geometry.h"
#include "Map.h"
#include "PostscriptCanvas.h"

#include <cmath>
#include <algorithm>
#include <iostream>

#include "01_util/Conversion.h"
#include "15_env/Line.h"
#include "15_env/PhysicalStop.h"


using synthese::util::Conversion;
using synthese::util::RGBColor;
using synthese::env::Line;
using synthese::env::Point;
using synthese::env::PhysicalStop;


namespace synthese
{
namespace carto
{


const DrawableLine::PointShiftingMode 
DrawableLine::POINT_SHIFTING_MODE = DOUBLE;
const bool   DrawableLine::ENABLE_CURVES = false;
const double   DrawableLine::RADIUS = 15.0;
const double   DrawableLine::RADIUS_DELTA = 10.0;
const int   DrawableLine::LINE_WIDTH = 5;
const double   DrawableLine::SPACING = LINE_WIDTH+1;
const RGBColor DrawableLine::BORDER_COLOR ("black");
const int   DrawableLine::BORDER_WIDTH = LINE_WIDTH+2;



DrawableLine::DrawableLine (const Line* line,
			    int fromLineStopIndex,
			    int toLineStopIndex)
    : _points (line->getPoints (fromLineStopIndex, toLineStopIndex))
    , _shortName (line->getId ())
    , _color (line->getColor ())
{

//    std::cout << "*** POINTs for line " << line->getId () << std::endl;
    
    for (unsigned int i=0; i<_points.size (); ++i) {
	//std::cout << i << "= " << _points[i]->getX () << " " << _points[i]->getY () << std::endl;
        // Shift initially to 0; 
	_shifts.push_back (0); 
        
        // Mark initially set to non-shifted
	_shifted.push_back (false); 
    }
    
}



    
DrawableLine::DrawableLine (const std::vector<const synthese::env::Point*>& points,
			    const std::string& shortName,
			    const synthese::util::RGBColor& color)
    : _points (points)
    , _shortName (shortName)
    , _color (color)
{
    for (unsigned int i=0; i<_points.size (); ++i) {
        // Shift initially to 0; 
	_shifts.push_back (0); 
        
        // Mark initially set to non-shifted
	_shifted.push_back (false); 
    }
    
}
    


DrawableLine::~DrawableLine()
{
}


bool 
DrawableLine::hasPoint (const synthese::env::Point* p) const
{
    return firstIndexOf (p) != -1;
}



const std::vector<const Point*>& 
DrawableLine::getPoints () const
{
    return _points;
}





int 
DrawableLine::isShifted (int pointIndex) const 
{ 
    return _shifted[pointIndex];  
}



int 
DrawableLine::getShift (int pointIndex) const 
{ 
    return _shifts[pointIndex]; 
}



void 
DrawableLine::setShift (int pointIndex, int shift) 
{ 
    _shifts[pointIndex] = shift; 
    _shifted[pointIndex] = true; 
}



int 
DrawableLine::firstIndexOf (const Point* p) const
{
    for (int i=0; i<_points.size (); ++i)
    {
	if (*(_points[i]) == *p) return i;	 	
    }
    return -1;
}



bool 
DrawableLine::isReverseWayAt (const Point* p, 
			      const DrawableLine* dbl) const
{
    const std::vector<const Point*>& points2 = dbl->getPoints ();
	
    // if _point has a following point in _points which is the
    // previous one in points2 or if _point has a previous point in _points
    // which is the next one in points2 then reverse points2.
    int index1 = firstIndexOf (p);
    int index2 = dbl->firstIndexOf (p);

    bool reverse = false;
    
    if ((index1+1 < (int) _points.size ())) 
    {
	const Point* fp1 = _points[index1+1];
	if ((index2-1 >= 0) && (points2[index2-1] == fp1)) reverse = true;
    }
    
    if (index1-1 >= 0) 
    {
	const Point* pp1 = _points[index1-1];
	if ((index2+1 < (int) points2.size ()) && (points2[index2+1] == pp1)) reverse = true;
    }
    
    return reverse;	
}




bool 
DrawableLine::isFullyReverseWay (const DrawableLine* dbl) const
{
    std::vector<const Point*> points2 = dbl->getPoints ();
    std::reverse (points2.begin (), points2.end ());
    if (points2.size () != _points.size ()) return false;
    for (int i=0; i<_points.size (); ++i)
    {
	if ((*_points[i]) != (*points2[i])) return false;
    }
    return true;
}


bool 
DrawableLine::isFullySameWay (const DrawableLine* dbl) const
{
    const std::vector<const Point*>& points2 = dbl->getPoints ();
    if (points2.size () != _points.size ()) return false;
    for (int i=0; i<_points.size (); ++i)
    {
	if (*(_points[i]) != *(points2[i])) return false;
    }
    return true;
}


int 
DrawableLine::numberOfCommonPointsWith (const DrawableLine* dbl) const
{
    int nb = 0;
    for (int i=0; i<_points.size (); ++i) 
    {
	const Point* p = _points[i];
	if (dbl->firstIndexOf (p) != -1) 
	{
	    ++nb;
	}	
    }	
    return nb;
}



bool 
DrawableLine::isStopPoint (int pointIndex) const
{
    const Point* p = _points[pointIndex];
    return dynamic_cast<const PhysicalStop*> (p) != 0;
}



bool 
DrawableLine::isViaPoint (int pointIndex) const
{
    return !isStopPoint (pointIndex);
}





Point 
DrawableLine::calculateSingleShiftedPoint (const Point& a, 
					   const Point& b, 
					   double distance) const
{
    double gamma = calculateAngle (b, a, Point (b.getX(), a.getY()));
    
    double deltax = distance * cos (M_PI_2 - gamma);
    double deltay = distance * sin (M_PI_2 - gamma);
    
    Point a_ (a.getX() + deltax, a.getY() + deltay);
        
    double angle = calculateAngle (b, a, a_);
    
    // Check that the angle formed by (b, a, a_) is positive otherwise
    // reverse point.
    if (((distance > 0) && (angle < 0)) 
        || ((distance < 0) && (angle > 0)) ) {
        return Point (a.getX() - deltax, a.getY() - deltay);
    } 
    return a_;
    
}



Point  
DrawableLine::calculateSingleShiftedPoint (const Point& a, 
					   const Point& b, 
					   const Point& c, 
					   double distance) const
{
    double alpha = calculateAngle (a, b, c);
    double gamma = calculateAngle (b, a, Point (b.getX(), a.getY()));
    
    double deltax = (distance / sin (alpha/2.0)) * (cos ((alpha/2.0) - gamma));
    double deltay = (distance / sin (alpha/2.0)) * (sin ((alpha/2.0) - gamma));

    Point b_ ( b.getX() + deltax , b.getY() + deltay );
        
    // Check that the angle formed by (b, a, b_) is positive otherwise
    // reverse point.
    double angle = calculateAngle (b, a, b_);
    
    if (((distance > 0) && (angle < 0)) 
        || ((distance < 0) && (angle > 0)) ) {
        return Point (b.getX() - deltax, b.getY() - deltay);
    } 
    return b_;
    
}





Point  
DrawableLine::calculateDoubleShiftedPoint (const Point& a, 
					   const Point& b, 
					   const Point& c, 
					   double incomingDistance, 
					   double outgoingDistance) const
{
    if (incomingDistance == outgoingDistance) {
        return calculateSingleShiftedPoint (a, b, c, incomingDistance);
    }
    
    double gamma0 = calculateAngle (b, a, Point (b.getX(), a.getY()));
    double gamma1 = calculateAngle (c, b, Point (c.getX(), b.getY()));
    
    if (gamma0 == gamma1) {
        // A, B, C are aligned; thus d0 == d1.
        Point p = 
	    calculateSingleShiftedPoint (a, b, c, incomingDistance);    
        return p;
    }
    
    double cosGamma0 = cos (gamma0);
    double cosGamma1 = cos (gamma1);

    // D0 <=> (AB) ; D1 <=> (BC)
    // D0 : y = a0.x + b0
    // D1 : y = a1.x + b1
    
    // Calculate a0, b0, a1, b1
    std::pair<double, double> a0b0 = calculateAffineEquation (a, b);
    std::pair<double, double> a1b1 = calculateAffineEquation (b, c);
    
    double a0 = a0b0.first;
    double b0 = a0b0.second;
    double a1 = a1b1.first;
    double b1 = a1b1.second;

    // We look for point K which is at a distance d0 of (D0)
    // and a distance d1 of (D1). Thus, K (xk, yk) verifies :
    // yk = a0.xk + bk0   (1)
    // yk = a1.xk + bk1   (2)
    
    double xk, yk;

    if (gamma0 == -M_PI_2) {
        
        xk = b.getX() - incomingDistance;
        double sign1 = c.getX() - b.getX() >=0 ? 1.0 : -1.0;
        double bk1 = b1 + sign1 * outgoingDistance / cosGamma1;
        yk = a1 * xk + bk1;
        
    } else if (gamma0 == M_PI_2) {
        
        xk = b.getX() + incomingDistance;
        double sign1 = c.getX() - b.getX() >=0 ? 1.0 : -1.0;
        double bk1 = b1 + sign1 * outgoingDistance / cosGamma1;
        yk = a1 * xk + bk1;

    } else if (gamma1 == -M_PI_2) {
        xk = b.getX() - outgoingDistance;
        double sign0 = b.getX() - a.getX() >=0 ? 1.0 : -1.0;
        double bk0 = b0 + sign0 * incomingDistance / cosGamma0;
        yk = a0 * xk + bk0;
        
    } else if (gamma1 == M_PI_2) {
        
        xk = b.getX() + outgoingDistance;
        double sign0 = b.getX() - a.getX() >=0 ? 1.0 : -1.0;
        double bk0 = b0 + sign0 * incomingDistance / cosGamma0;
        yk = a0 * xk + bk0;
    
    } else {
        double sign0 = b.getX() - a.getX() >=0 ? 1.0 : -1.0;
        double sign1 = c.getX() - b.getX() >=0 ? 1.0 : -1.0;
        
        double bk0 = b0 +  sign0 * incomingDistance / cosGamma0;
        double bk1 = b1 +  sign1 * outgoingDistance / cosGamma1;
        
        // Finally, solving (1) and (2) gives :
        xk = (bk1 - bk0) / (a0 - a1);
        yk = a1 * xk + bk1;
    }
    
    return Point (xk, yk);
    
}




const std::vector<Point>
DrawableLine::calculateShiftedPoints (const std::vector<Point>& points) const
{
    std::vector<Point> shiftedPoints;
    
    // First point
    shiftedPoints.push_back (
	calculateSingleShiftedPoint (points[0], 
				     points[1], 
				     _shifts[0]*SPACING));
    
    
    // All triplets in between
    for (unsigned int i=0; i<points.size()-2; ++i) 
    {
	const Point& p_i = points[i]; 
	const Point& p_i_plus_1 = points[i+1];
	const Point& p_i_plus_2 = points[i+2];
	
	if (_shifts[i+1] != 0) 
	{

	    bool previouslyShifted = false;

	    // Check if the point was previously shifted
	    for (unsigned int j=0; j<i+1; ++j) 
	    {
		if (points[j] == points[i+1]) 
		{
		    // If previously shifted, reuse the same point
		    previouslyShifted = true;
		    shiftedPoints.push_back (shiftedPoints[j]);
		    break;
		}
	    }	
	    if (!previouslyShifted)	{
                
                if (POINT_SHIFTING_MODE == DOUBLE) 
		{
                    
                    int shift0 = _shifts[i];
		    int shift1 = _shifts[i+1];
		    int shift2 = _shifts[i+2];
		    
                    double incomingDistance = shift1;
                    double outgoingDistance = shift1;
                    
                    // Not sure about this : maybe rather check that the number of shared
                    // lines is changing ?
                    if ((shift0 != shift1) && (abs (shift0) < abs (shift1))) 
		    {
			incomingDistance = shift0;
		    }

                    if (shift2 != shift1) 
		    {
			outgoingDistance = shift2;
		    }
		    
                    incomingDistance *= SPACING; 
                    outgoingDistance *= SPACING;
                    
                    shiftedPoints.push_back ( 
                        calculateDoubleShiftedPoint (p_i, 
						     p_i_plus_1, 
						     p_i_plus_2, 
						     incomingDistance, 
						     outgoingDistance) );
                } 
		else 
		{
                    double distance =  _shifts[i+1] * SPACING;
                    shiftedPoints.push_back ( 
                        calculateSingleShiftedPoint (
			    p_i, 
			    p_i_plus_1, 
			    p_i_plus_2, 
			    distance) );
                }
	    }
			
	} 
	else 
	{
	    shiftedPoints.push_back (p_i_plus_1);
	}
    }

    // Last point 
	
    bool previouslyShifted = false;

    // Check if the point was previously shifted
    for (unsigned int j=0; j<_points.size()-1; ++j) 
    {
	if (points[j] == points[_points.size()-1]) 
	{
	    // If previously shifted, reuse the same point
	    previouslyShifted = true;
	    shiftedPoints.push_back (shiftedPoints[j]);
	    break;
	}
    }	
    if (!previouslyShifted)	
    {
	shiftedPoints.push_back(
	    calculateSingleShiftedPoint (points[_points.size()-1], 
					 points[_points.size()-2], 
					 -_shifts[_points.size()-1]*SPACING));
    }
    
    return shiftedPoints;	
    
}






void 
DrawableLine::doDrawCurvedLine (PostscriptCanvas& canvas, 
				const std::vector<Point>& shiftedPoints) const
{
    canvas.newpath();
    canvas.moveto(shiftedPoints[0].getX()+5, shiftedPoints[0].getY()+10);
    canvas.rotate (45.0);
    canvas.text (_shortName);
    canvas.rotate (-45.0);
    canvas.moveto(shiftedPoints[0].getX(), shiftedPoints[0].getY());
    
    for (unsigned int i=1; i<shiftedPoints.size (); ++i) 
    {
	double x = shiftedPoints[i].getX();
	double y = shiftedPoints[i].getY();
	double radiusShift = 0.0;
	
	if (ENABLE_CURVES && (i < shiftedPoints.size () - 1)) 
	{
	    // Take care of intern/extern turn to invert radius
	    const Point& p_minus_1 = shiftedPoints[i-1];
	    const Point& p_plus_1 = shiftedPoints[i+1];
	    
	    double angle = calculateAngle (p_minus_1, shiftedPoints[i], p_plus_1);
	    if (angle < 0) 
	    {
		radiusShift = -_shifts[i] * SPACING;
	    } 
	    else 
	    {
		radiusShift = +_shifts[i] * SPACING;
	    } 
	    
	    
	    double radiusToUse = RADIUS + radiusShift;
	    if (radiusToUse > RADIUS + RADIUS_DELTA) radiusToUse = RADIUS + RADIUS_DELTA;
	    if (radiusToUse < RADIUS - RADIUS_DELTA) radiusToUse = RADIUS - RADIUS_DELTA;
	    
	    
	    canvas.arct(x, y, shiftedPoints[i+1].getX(), 
			shiftedPoints[i+1].getY(), radiusToUse);
	    
	} 
	else 
	{ 
	    canvas.lineto(x, y);	
	}
	
    }
    
    canvas.stroke();
    
}



void 
DrawableLine::doDrawTriangleArrow (PostscriptCanvas& canvas, 
				   const Point& point, 
				   double angle) const
{
    canvas.gsave ();
    canvas.setrgbcolor(0, 0, 0);
    canvas.moveto(point.getX(), point.getY ());
    canvas.rotate (angle);
    canvas.rmoveto (0.0, LINE_WIDTH / 2.0 + 1);
    canvas.triangle(LINE_WIDTH-1);  // size of the base side
    canvas.fill ();
    canvas.grestore ();	
}



void 
DrawableLine::doDrawSquareStop (PostscriptCanvas& canvas, 
				const Point& point, 
				double angle) const
{
    canvas.gsave ();
    canvas.setrgbcolor(0, 0, 0);
    canvas.moveto(point.getX(), point.getY ());
    canvas.rotate (angle);
    canvas.square(LINE_WIDTH-1);  // size of the base side
    canvas.fill ();
    canvas.grestore ();	
}




void 
DrawableLine::doDrawSquareTerminus (PostscriptCanvas& canvas, 
				    const Point& point, 
				    double angle) const
{
    canvas.gsave ();
    canvas.setrgbcolor(0, 0, 0);
    canvas.moveto(point.getX(), point.getY ());
    canvas.rotate (angle);
    canvas.square(LINE_WIDTH*2);  // size of the base side
    canvas.fill ();
    canvas.grestore (); 
}




void 
DrawableLine::preDraw (Map& map, PostscriptCanvas& canvas) const
{
    std::vector<Point> points;
    
    // Convert coordinates to output frame
    for (unsigned int i=0; i<_points.size(); ++i) {
        points.push_back (map.toOutputFrame(*_points[i]));
    }
        
    _shiftedPoints = (POINT_SHIFTING_MODE == NONE) ? points 
	: calculateShiftedPoints (points);
    
    
}



void 
DrawableLine::postDraw (Map& map, PostscriptCanvas& canvas) const
{
    // Draw Terminuses
    if (_shiftedPoints.size () >= 2) {

        Point pt (_shiftedPoints[0].getX() + 100.0, 
				 _shiftedPoints[0].getY());

        double angle = calculateAngle (pt, 
				       _shiftedPoints[0], 
				       _shiftedPoints[1]);

        doDrawSquareTerminus (canvas, 
			      _shiftedPoints[0], 
			      toDegrees(angle - M_PI_2));
        
        pt = Point (_shiftedPoints[_shiftedPoints.size ()-2].getX() + 100.0, 
				   _shiftedPoints[_shiftedPoints.size ()-2].getY());

        angle = calculateAngle (pt, 
				_shiftedPoints[_shiftedPoints.size ()-2], 
				_shiftedPoints[_shiftedPoints.size ()-1]);
     
	doDrawSquareTerminus (canvas, 
			      _shiftedPoints[_shiftedPoints.size ()-1], 
			      toDegrees(angle - M_PI_2));
    }

    // Clear cached information    
    _shiftedPoints.clear ();
}



void 
DrawableLine::draw (Map& map, PostscriptCanvas& canvas) const
{

    canvas.setlinewidth (BORDER_WIDTH);
    canvas.setrgbcolor(BORDER_COLOR.r, BORDER_COLOR.g, BORDER_COLOR.b);
    doDrawCurvedLine(canvas, _shiftedPoints);
    
    canvas.setlinewidth (LINE_WIDTH);
    
    canvas.setrgbcolor(_color);
    doDrawCurvedLine(canvas, _shiftedPoints);
    
    for (unsigned int i=1; i<_shiftedPoints.size()-1; ++i) 
    {
	Point pt (_shiftedPoints[i].getX() + 100.0, _shiftedPoints[i].getY());
	double angle = calculateAngle (pt, _shiftedPoints[i], _shiftedPoints[i+1]);

	if (isStopPoint (i)) 
	{
	    doDrawTriangleArrow(canvas, _shiftedPoints[i], toDegrees(angle - M_PI_2));
	    doDrawSquareStop(canvas, _shiftedPoints[i], toDegrees(angle - M_PI_2));
	}
	
    }
    
    canvas.setrgbcolor (0.5, 0.5 , 0.5);
    
    // For debug : draw unshifted points as circles
    // For debug : draw unshifted points as circles
    // For debug : draw unshifted points as circles
    // For debug : draw unshifted points as circles
    std::vector<Point> points;
    
    // Convert coordinates to output frame
    for (unsigned int i=0; i<_points.size(); ++i) 
    {
        points.push_back (map.toOutputFrame(*_points[i]));
    }

    canvas.setlinewidth (1);

    for (unsigned int i=0; i<points.size(); ++i) 
    {
	canvas.newpath();
	canvas.moveto (points[i].getX (), points[i].getY ());
	canvas.square (3.0);
	canvas.stroke ();
    } 
    
}



const std::string& 
DrawableLine::getShortName () const
{
    return _shortName;
}




}
}

