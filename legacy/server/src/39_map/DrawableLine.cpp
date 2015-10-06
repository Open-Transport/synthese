
/** DrawableLine class implementation.
	@file DrawableLine.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "DrawableLine.h"

#include "Geometry.h"
#include "Map.h"

#include <cmath>
#include <algorithm>
#include <iostream>

#include "JourneyPattern.hpp"
#include "CommercialLine.h"
#include "StopPoint.hpp"

#include <geos/geom/Coordinate.h>
#include <geos/geom/LineString.h>

using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace pt;
	using namespace util;
	using namespace pt;


namespace map
{





DrawableLine::DrawableLine (const JourneyPattern* line,
			    int fromLineStopIndex,
			    int toLineStopIndex,
			    bool withPhysicalStops)
    : _lineId (line->getKey())
    , _geometry (line->getGeometry(fromLineStopIndex, toLineStopIndex))
    , _shortName (line->getName ())
	, _color (line->getCommercialLine()->getColor() ? *line->getCommercialLine ()->getColor() : RGBColor(0,0,0))
    , _withPhysicalStops (withPhysicalStops)
{
    for (unsigned int i=0; i<_geometry->getCoordinatesRO()->getSize(); ++i) {
        // Shift initially to 0;
	_shifts.push_back (0);

        // Mark initially set to non-shifted
	_shifted.push_back (false);
    }
}




DrawableLine::DrawableLine (const util::RegistryKeyType& lineId,
			    boost::shared_ptr<LineString> points,
			    const std::string& shortName,
			    const synthese::util::RGBColor& color,
			    bool withPhysicalStops)
    : _lineId (lineId)
    , _geometry (points)
    , _shortName (shortName)
    , _color (color)
    , _withPhysicalStops (withPhysicalStops)
{
    for (unsigned int i=0; i<_geometry->getCoordinatesRO()->getSize(); ++i) {
        // Shift initially to 0;
	_shifts.push_back (0);

        // Mark initially set to non-shifted
	_shifted.push_back (false);
    }


}



DrawableLine::~DrawableLine()
{
}



const util::RegistryKeyType&
DrawableLine::getLineId () const
{
    return _lineId;
}




bool
DrawableLine::hasPoint (const Coordinate& p) const
{
    return firstIndexOf (p) != -1;
}



const std::vector<Coordinate>&
DrawableLine::getFuzzyfiedPoints () const
{
    return _fuzzyfiedPoints;
}




const std::vector<Coordinate>&
DrawableLine::getShiftedPoints () const
{
    return _shiftedPoints;
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
DrawableLine::firstIndexOf (const Coordinate& p) const
{
    for(size_t i=0; i<_fuzzyfiedPoints.size (); ++i)
    {
	if (_fuzzyfiedPoints[i] == p) return i;
    }
    return -1;
}



bool
DrawableLine::isReverseWayAt (const Coordinate& p,
			      const DrawableLine* dbl) const
{
    const std::vector<Coordinate>& points2 = dbl->getFuzzyfiedPoints ();

    // if _point has a following point in _geometry which is the
    // previous one in points2 or if _point has a previous point in _geometry
    // which is the next one in points2 then reverse points2.
    int index1 = firstIndexOf (p);
    int index2 = dbl->firstIndexOf (p);

    bool reverse = false;

    if ((index1+1 < (int) getFuzzyfiedPoints ().size ()))
    {
	Coordinate fp1 = _fuzzyfiedPoints[index1+1];
	if ((index2-1 >= 0) && (points2[index2-1] == fp1)) reverse = true;
    }

    if (index1-1 >= 0)
    {
	Coordinate pp1 = _fuzzyfiedPoints[index1-1];
	if ((index2+1 < (int) points2.size ()) && (points2[index2+1] == pp1)) reverse = true;
    }

    return reverse;
}




bool
DrawableLine::isFullyReverseWay (const DrawableLine* dbl) const
{
    std::vector<Coordinate> points2 = dbl->getFuzzyfiedPoints ();
    std::reverse (points2.begin (), points2.end ());
    if (points2.size () != _fuzzyfiedPoints.size ()) return false;
    for (size_t i=0; i<_fuzzyfiedPoints.size (); ++i)
    {
	if ((_fuzzyfiedPoints[i]) != (points2[i])) return false;
    }
    return true;
}


bool
DrawableLine::isFullySameWay (const DrawableLine* dbl) const
{
    const std::vector<Coordinate>& points2 = dbl->getFuzzyfiedPoints ();
    if (points2.size () != _fuzzyfiedPoints.size ()) return false;
    for (size_t i=0; i<_fuzzyfiedPoints.size (); ++i)
    {
	if (_fuzzyfiedPoints[i] != points2[i]) return false;
    }
    return true;
}


size_t DrawableLine::numberOfCommonPointsWith(const DrawableLine* dbl) const
{
    size_t nb = 0;
    for (size_t i=0; i<_fuzzyfiedPoints.size (); ++i)
    {
	const Coordinate& p = _fuzzyfiedPoints[i];
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
return false;
//	const Coordinate* p = _geometry[pointIndex];
//    return dynamic_cast<const StopPoint*> (p) != 0;
}



bool
DrawableLine::isViaPoint (int pointIndex) const
{
    return !isStopPoint (pointIndex);
}





Coordinate
DrawableLine::calculateSingleShiftedPoint (Coordinate a,
					   Coordinate b,
					   double distance) const
{
    double gamma = calculateAngle (b, a, Coordinate (b.x, a.y));

	double deltax = distance * cos (M_PI_2 - gamma);
    double deltay = distance * sin (M_PI_2 - gamma);

    Coordinate a_ (a.x + deltax, a.y + deltay);

    double angle = calculateAngle (b, a, a_);

    // Check that the angle formed by (b, a, a_) is positive otherwise
    // reverse point.
    if (((distance > 0) && (angle < 0))
        || ((distance < 0) && (angle > 0)) ) {
        return Coordinate (a.x - deltax, a.y - deltay);
    }
    return a_;

}



Coordinate
DrawableLine::calculateSingleShiftedPoint (Coordinate a,
					   Coordinate b,
					   Coordinate c,
					   double distance) const
{

	// Special case if a == c, then replace c with symetric of a regarding b
	if (a == c) c = calculateSymetric (a, b);

    double alpha = calculateAngle (a, b, c);

    double gamma = calculateAngle (b, a, Coordinate (b.x, a.y));

    double deltax = (distance / sin (alpha/2.0)) * (cos ((alpha/2.0) - gamma));
    double deltay = (distance / sin (alpha/2.0)) * (sin ((alpha/2.0) - gamma));

/*	if ((deltax > 100) || (deltay > 100)) {
		int yy = 5;
	} */

    Coordinate b_ ( b.x + deltax , b.y + deltay );

    // Check that the angle formed by (b, a, b_) is positive otherwise
    // reverse point.
    double angle = calculateAngle (b, a, b_);

    if (((distance > 0) && (angle < 0))
        || ((distance < 0) && (angle > 0)) ) {
        return Coordinate (b.x - deltax, b.y - deltay);
    }
    return b_;

}





Coordinate
DrawableLine::calculateDoubleShiftedPoint (Coordinate a,
					   Coordinate b,
					   Coordinate c,
					   double incomingDistance,
					   double outgoingDistance) const
{
	// Special case if a == c, then replace c with symetric of a regarding b
	if (a == c) c = calculateSymetric (a, b);

/*	std::cerr << "a=" << a.x << "," << a.y << ";   "
			  << "b=" << b.x << "," << b.y << ";   "
			  << "c=" << c.x << "," << c.y << std::endl; */

	if (incomingDistance == outgoingDistance) {
        return calculateSingleShiftedPoint (a, b, c, incomingDistance);
    }

    double gamma0 = calculateAngle (b, a, Coordinate (b.x, a.y));
    double gamma1 = calculateAngle (c, b, Coordinate (c.x, b.y));

    if (gamma0 == gamma1) {
        // A, B, C are aligned; thus d0 == d1.
        Coordinate p =
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

	// std::cerr << "g0 = " << gamma0 << "; g1 = " << gamma1 << std::endl;

    if (gamma0 == -M_PI_2) {

        xk = b.x - incomingDistance;
        double sign1 = c.x - b.x >=0 ? 1.0 : -1.0;
        double bk1 = b1 + sign1 * outgoingDistance / cosGamma1;
        yk = a1 * xk + bk1;

    } else if (gamma0 == M_PI_2) {

        xk = b.x + incomingDistance;
        double sign1 = c.x - b.x >=0 ? 1.0 : -1.0;
        double bk1 = b1 + sign1 * outgoingDistance / cosGamma1;
        yk = a1 * xk + bk1;

    } else if (gamma1 == -M_PI_2) {
        xk = b.x - outgoingDistance;
        double sign0 = b.x - a.x >=0 ? 1.0 : -1.0;
        double bk0 = b0 + sign0 * incomingDistance / cosGamma0;
        yk = a0 * xk + bk0;

    } else if (gamma1 == M_PI_2) {

        xk = b.x + outgoingDistance;
        double sign0 = b.x - a.x >=0 ? 1.0 : -1.0;
        double bk0 = b0 + sign0 * incomingDistance / cosGamma0;
        yk = a0 * xk + bk0;

    } else {
        double sign0 = b.x - a.x >=0 ? 1.0 : -1.0;
        double sign1 = c.x - b.x >=0 ? 1.0 : -1.0;

        double bk0 = b0 +  sign0 * incomingDistance / cosGamma0;
        double bk1 = b1 +  sign1 * outgoingDistance / cosGamma1;

        // Finally, solving (1) and (2) gives :
        xk = (bk1 - bk0) / (a0 - a1);
        yk = a1 * xk + bk1;
    }

	Coordinate k (xk, yk);

	// <Heuristic 1 :
	// Note : this is experimental... but necessary. A better way
	// to handle this would be to insert an extra point.

	// If the distance of the shifted point relatively to B exceeds
    // a certain amount, single shift the point to preserve good-looking
	// display
	double shiftDistance = calculateDistance (b, k);
	if (shiftDistance > (incomingDistance+outgoingDistance) )
	{
		// We could even decrease this limit for aesthetic reasons...
        return calculateSingleShiftedPoint (a, b, c, incomingDistance);
	}
	// Heuristic 1>

    return k;
}



void
DrawableLine::fuzzyfyPoints (const DrawableLineIndex& lineIndex)
{
	_fuzzyfiedPoints.clear ();
	for (size_t i=0; i<_geometry->getCoordinatesRO()->getSize(); ++i) {
		_fuzzyfiedPoints.push_back (lineIndex.getFuzzyPoint(_geometry->getCoordinatesRO()->getAt(i)));
		// _fuzzyfiedPoints.push_back (*(_points[i]));
	}
}





const std::vector<Coordinate>
DrawableLine::calculateShiftedPoints (const std::vector<Coordinate>& points,
				      double spacing,
				      PointShiftingMode shiftMode) const
{
    std::vector<Coordinate> shiftedPoints;

    // First point
    shiftedPoints.push_back (
	calculateSingleShiftedPoint (points[0],
				     points[1],
				     _shifts[0]*spacing));


    // All triplets in between
    for (unsigned int i=0; i<points.size()-2; ++i)
    {
	const Coordinate& p_i = points[i];
	const Coordinate& p_i_plus_1 = points[i+1];
	const Coordinate& p_i_plus_2 = points[i+2];

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

                if (shiftMode == SQL_DOUBLE)
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

                    incomingDistance *= spacing;
                    outgoingDistance *= spacing;

                    shiftedPoints.push_back (
                        calculateDoubleShiftedPoint (p_i,
						     p_i_plus_1,
						     p_i_plus_2,
						     incomingDistance,
						     outgoingDistance) );
                }
		else
		{
                    double distance =  _shifts[i+1] * spacing;
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
    for (unsigned int j=0; j<points.size()-1; ++j)
    {
	if (points[j] == points[points.size()-1])
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
	    calculateSingleShiftedPoint (points[points.size()-1],
					 points[points.size()-2],
					 -_shifts[points.size()-1]*spacing));
    }

    return shiftedPoints;

}





const std::vector<Coordinate>
DrawableLine::calculateAbsoluteShiftedPoints (const std::vector<Coordinate>& points,
										 double spacing) const
{
    std::vector<Coordinate> shiftedPoints;

    // First point
    shiftedPoints.push_back (
	calculateSingleShiftedPoint (points[0],
				     points[1],
				     spacing));


    // All triplets in between
    for (unsigned int i=0; i<points.size()-2; ++i)
    {
		const Coordinate& p_i = points[i];
		const Coordinate& p_i_plus_1 = points[i+1];
		const Coordinate& p_i_plus_2 = points[i+2];

		shiftedPoints.push_back ( calculateSingleShiftedPoint (
					p_i,
					p_i_plus_1,
					p_i_plus_2,
					spacing) );
	}

    // Last point
	shiftedPoints.push_back(
	    calculateSingleShiftedPoint (points[points.size()-1],
					 points[points.size()-2],
					 -_shifts[points.size()-1]*spacing));

    return shiftedPoints;

}







void
DrawableLine::prepare (Map& map, double spacing, PointShiftingMode shiftMode) const
{
    std::vector<Coordinate> points;

	Coordinate previousPoint (-1, -1);
    // Convert coordinates to output frame
    for (unsigned int i=0; i<_fuzzyfiedPoints.size(); ++i) {
		Coordinate p = _fuzzyfiedPoints[i];

		// Fuzzyfication of points can lead to having to points exactly
		// identical in the list. Slightly move them (not filtering to
		// keep bijection with reference points.
		Coordinate outputPoint = map.toOutputFrame(p);

		if (outputPoint == previousPoint)
		{
			outputPoint = Coordinate(outputPoint.x + 0.1, outputPoint.y + 0.1);
		}
		previousPoint = outputPoint;
        points.push_back (outputPoint);
    }

    _shiftedPoints = (shiftMode == NONE) ? points
	: calculateShiftedPoints (points, spacing, shiftMode);

}







const std::string&
DrawableLine::getShortName () const
{
    return _shortName;
}



const util::RGBColor&
DrawableLine::getColor () const
{
	return _color;
}




bool
DrawableLine::getWithPhysicalStops () const
{
    return _withPhysicalStops;
}



}
}
