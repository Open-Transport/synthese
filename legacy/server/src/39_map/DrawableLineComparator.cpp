
/** DrawableLineComparator class implementation.
	@file DrawableLineComparator.cpp

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

#include "DrawableLineComparator.h"

#include "DrawableLine.h"
#include "Geometry.h"

#include "JourneyPattern.hpp"

#include <assert.h>

#include <cmath>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace geos::geom;


namespace synthese
{

namespace map
{


DrawableLineComparator::DrawableLineComparator(
    const DrawableLine* reference,
    const Coordinate& referencePoint,
    const Coordinate& point)
    : _reference (reference)
    , _referencePoint (referencePoint)
    , _point (point)
{
}



DrawableLineComparator::~DrawableLineComparator()
{
}


int
DrawableLineComparator::firstIndexOf (const Coordinate& point,
				      const std::vector<Coordinate>& points) const
{
    for (unsigned int i=0; i<points.size (); ++i)
    {
	if (points[i] == point) return i;
    }
    return -1;
}




void
DrawableLineComparator::setPoint (const Coordinate& point)
{
    _point = point;
}


/*
double
DrawableLineComparator::calculateStartAngleAtIndex (
    const std::vector<const Coordinate*>& points,
    int index) const
{
    if (index+2 >= (int) points.size ()) return -1;

    const Coordinate* vs0_1 = (const Coordinate*) points[index];
    const Coordinate* vs1_1 = (const Coordinate*) points[index + 1];
    const Coordinate* vs2_1 = (const Coordinate*) points[index + 2];

    double angle = calculateAngle (*vs0_1, *vs1_1, *vs2_1);
    if (angle < 0) angle += 2*M_PI;
    return angle;
}




double
DrawableLineComparator::calculateEndAngleAtIndex (
    const std::vector<const Coordinate*>& points,
    int index) const
{
    if (index < 2) return -1;

    const Coordinate* vs0_1 = (const Coordinate*) points[index - 2];
    const Coordinate* vs1_1 = (const Coordinate*) points[index - 1];
    const Coordinate* vs2_1 = (const Coordinate*) points[index];

    double angle = calculateAngle (*vs0_1, *vs1_1, *vs2_1);
    if (angle < 0) angle += 2*M_PI;
    return angle;
}
*/



double
DrawableLineComparator::calculateStartAngleAtIndex (
    const std::vector<Coordinate>& points,
    int index) const
{
    if (index+2 >= (int) points.size ()) return -1;

    const Coordinate& vs0_1 = points[index];
    const Coordinate& vs1_1 = points[index + 1];
    const Coordinate& vs2_1 = points[index + 2];

    double angle = calculateAngle (vs0_1, vs1_1, vs2_1);
    if (angle < 0) angle += 2*M_PI;
    return angle;
}




double
DrawableLineComparator::calculateEndAngleAtIndex (
    const std::vector<Coordinate>& points,
    int index) const
{
    if (index < 2) return -1;

    const Coordinate& vs0_1 = points[index - 2];
    const Coordinate& vs1_1 = points[index - 1];
    const Coordinate& vs2_1 = points[index];

    double angle = calculateAngle (vs0_1, vs1_1, vs2_1);
    if (angle < 0) angle += 2*M_PI;
    return angle;
}




std::pair<double, double>
DrawableLineComparator::calculateStartAngles (
	const std::vector<Coordinate>& points1, int index1,
	const std::vector<Coordinate>& points2, int index2) const
{
    assert (points1.size () >= 2);
    assert (points2.size () >= 2);

    // The six points for calculating angles
    Coordinate p1_0 (0, 0);
    Coordinate p1_1 (0, 0);
    Coordinate p1_2 (0, 0);
    Coordinate p2_0 (0, 0);
    Coordinate p2_1 (0, 0);
    Coordinate p2_2 (0, 0);

    if ((index1 == -1) && (index2 == -1))
    {
	p1_0 = calculateSymetric (points1[1], points1[0]);
	p1_1 = points1[0];
	p1_2 = points1[1];

	p2_0 = calculateSymetric (points2[1], points2[0]);
	p2_1 = points2[0];
	p2_2 = points2[1];
    }
    else if ((index1 == -1) && (index2 != -1))
    {
	p1_2 = points1[1];

	// Take the symetric of second point regarding first point
	p2_0 = points2[index2];
	p2_1 = points2[index2+1];
	p2_2 = ((size_t)index2+2 >= points2.size ()) ? p1_2 : points2[index2+2];

	p1_0 = p2_0;
	p1_1 = points1[0];

    }
    else if ((index1 != -1) && (index2 == -1))
    {
	p2_2 = points2[1];

	// Take the symetric of second point regarding first point
	p1_0 = points1[index1];
	p1_1 = points1[index1+1];
	p1_2 = ((size_t)index1+2 >= points1.size ()) ? p2_2 : points1[index1+2];

	p2_0 = p1_0;
	p2_1 = points2[0];


    }
    else
    {
	p1_0 = points1[index1];
	p1_1 = points1[index1+1];

	if ((size_t)index1+2 >= points1.size ())
	{
	    if ((size_t)index2+2 < points2.size ())
	    {
		p2_2 = points2[index2+2];
		p1_2 = p2_2;
	    }
	    else
	    {
		p1_2 = calculateSymetric (points1[index1],
					  points1[index1+1]);
	    }
	}
	else
	{
	    p1_2 = points1[index1+2];
	}

	p2_0 = points2[index2];
	p2_1 = points2[index2+1];

	// p2_2 = (index1+2 >= points1.size ()) ?
	p2_2 = ((size_t)index2+2 >= points2.size ()) ?
	    p1_2
	    : points2[index2+2];

    }


    double sangle1 = calculateAngle (p1_0, p1_1, p1_2);
    if (sangle1 < 0) sangle1 += 2*M_PI;

    double sangle2 = calculateAngle (p2_0, p2_1, p2_2);
    if (sangle2 < 0) sangle2 += 2*M_PI;

    return std::make_pair (sangle1, sangle2);

}






std::pair<double, double>
DrawableLineComparator::calculateEndAngles (const std::vector<Coordinate>& points1, int index1,
					    const std::vector<Coordinate>& points2, int index2) const
{
    assert (points1.size () >= 2);
    assert (points2.size () >= 2);

    // The six points for calculating angles
    Coordinate p1_0 (0, 0);
    Coordinate p1_1 (0, 0);
    Coordinate p1_2 (0, 0);
    Coordinate p2_0 (0, 0);
    Coordinate p2_1 (0, 0);
    Coordinate p2_2 (0, 0);

    if (((size_t)index1 == points1.size ()) && ((size_t)index2 == points2.size ()))
    {
	p1_0 = points1[index1-2];
	p1_1 = points1[index1-1];
	p1_2 = calculateSymetric (p1_0, p1_1);

	p2_0 = points2[index2-2];
	p2_1 = points2[index2-1];
	p2_2 = calculateSymetric (p2_0, p2_1);
    }
    else if (((size_t)index1 == points1.size ()) && ((size_t)index2 < points2.size ()))
    {
	p2_2 = points2[index2];

	p1_0 = points1[index1-2];
	p1_1 = points1[index1-1];
	p1_2 = p2_2;

	p2_0 = (index2-2 < 0) ? p1_0 : points2[index2-2];
	p2_1 = points2[index2-1];

    }
    else if (((size_t)index1 < points1.size ()) && ((size_t)index2 == points2.size ()))
    {
	p1_2 = points1[index1];

	p2_0 = points2[index2-2];
	p2_1 = points2[index2-1];
	p2_2 = p1_2;

	p1_0 = (index1-2 < 0) ? p2_0 : points1[index1-2];
	p1_1 = points1[index1-1];

    }
    else
    {
	if (index1-2 < 0)
	{
	    if (index2-2 >= 0)
	    {
		p2_0 = points2[index2-2];
		p1_0 = p2_0;
	    }
	    else
	    {
		p1_0 = calculateSymetric (points1[index1],
					  points1[index1-1]);
	    }
	}
	else
	{
	    p1_0 = points1[index1-2];
	}
	p1_1 = points1[index1-1];
	p1_2 = points1[index1];

	p2_0 = (index2-2 < 0) ?
	    p1_0
	    : points2[index2-2];
	p2_1 = points2[index2-1];
	p2_2 = points2[index2];
    }


    double eangle1 = calculateAngle (p1_0, p1_1, p1_2);
    if (eangle1 < 0) eangle1 += 2*M_PI;

    double eangle2 = calculateAngle (p2_0, p2_1, p2_2);
    if (eangle2 < 0) eangle2 += 2*M_PI;

    return std::make_pair (eangle1, eangle2);

}








int
DrawableLineComparator::operator() (const DrawableLine* bl1,
				    const DrawableLine* bl2) const
{
	// ********************************
	// ***  !!!!  BIG WARNING !!!!  ***
    // This operator MUST implement a strict weak ordering relationship
	// Otherwise, behavior is UNDEFINED, which means : crashes anywhere
	// but here...
	// ********************************

    // return 0;
    // std::cout << "Comparing " <<  bl1->getShortName () << " and " << bl2->getShortName () << std::endl;

    // Special case : the lines follow exactly reverse ways
    if (bl1->isFullyReverseWay(bl2))
    {
	/*
	bool result = 1;
	if (bl1->isReverseWayAt(_referencePoint, _reference)) result = !result;
	return result;
	*/
//	std::cerr << ((long) bl1) << " < " << ((long) bl2) << " = " << 0 << std::endl;
	return 0;
    }

    // Special case : the lines follow exactly same ways
    if (bl1->isFullySameWay(bl2))
    {
	/*
	bool result = bl1->getShortName () > bl2->getShortName ();
	if (bl1->isReverseWayAt(_referencePoint, _reference)) result = !result;
	return result;
	*/
//	std::cerr << ((long) bl1) << " < " << ((long) bl2) << " = " << 0 << std::endl;
	return 0;
    }

    // TODO Other special cases to be handled...
    std::vector<Coordinate> points1 = bl1->getFuzzyfiedPoints ();
    std::vector<Coordinate> points2 = bl2->getFuzzyfiedPoints ();

    int index1_1, index1_2, index2_1, index2_2;

    bool mustReverseLine2 = bl2->isReverseWayAt (_point, bl1);
    if (mustReverseLine2) std::reverse (points2.begin (), points2.end ());

    Coordinate curPoint = _point;
    index1_1 = firstIndexOf(curPoint, points1);
    index1_2 = firstIndexOf(curPoint, points2);

    if ((index1_1 == -1) || (index1_2 == -1)) {
/*	bool result = bl1->getShortName () > bl2->getShortName ();
	if (bl1->isReverseWayAt(_referencePoint, _reference)) result = !result;
	return result;*/
	// std::cerr << ((long) bl1) << " < " << ((long) bl2) << " = " << 0 << std::endl;
        return  0;
    }

    while ((index1_2 >= 0) &&
	   (index1_1 >= 0) &&
	   (points1[index1_1] == curPoint))
    {
	--index1_2;
	--index1_1;
	if (index1_2 >=0) curPoint = points2[index1_2];
    }


    std::pair<double,double> sangles = calculateStartAngles (
	points1, index1_1,
	points2, index1_2);


    curPoint = _point;
    index2_1 = firstIndexOf(curPoint, points1);
    index2_2 = firstIndexOf(curPoint, points2);

    while (
	   (index2_2 < (int) points2.size ()) &&
	   (index2_1 < (int) points1.size ()) &&
	   (points1[index2_1] == curPoint))
    {
	++index2_2;
	++index2_1;
	if ((size_t)index2_2 <= points2.size ()-1) curPoint = points2[index2_2];
    }


    std::pair<double,double> eangles = calculateEndAngles (
	points1, index2_1,
	points2, index2_2);


    bool result = true;
    double sangle1 = sangles.first; double sangle2 = sangles.second;
    double eangle1 = eangles.first; double eangle2 = eangles.second;

    if ((sangle1 == sangle2) && (eangle1 == eangle2))
    {
	// Replace angle calculation according to significant points
	if (index1_1 == -1) sangle1 = M_PI;
	if (index1_2 == -1) sangle2 = M_PI;

	if ((size_t)index2_1 == points2.size ()) eangle1 = M_PI;
	if ((size_t)index2_2 == points2.size ()) eangle2 = M_PI;
    }


    if ((sangle1 <= sangle2) != (eangle1 <= eangle2))
    {
	// std::cerr << ((long) bl1) << " < " << ((long) bl2) << " = " << 0 << std::endl;
	return 0; // Considered equivalent.
    }
    else
    {
	result = result && (sangle1 <= sangle2);
	result = result && (eangle1 <= eangle2);
    }

    if (_reference->isReverseWayAt (_referencePoint, bl1)) result = !result;

    // return 0;
    // std::cerr << ((long) bl1) << " < " << ((long) bl2) << " = " << result << std::endl;
    return result;

}








}
}

