/**********************************************************************
 * $Id: EdgeIntersection.cpp 2429 2009-04-30 10:43:16Z strk $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2009      Sandro Santilli <strk@keybit.net>
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************
 *
 * Last port: geomgraph/EdgeIntersection.java rev. 1.5 (JTS-1.10)
 *
 **********************************************************************/

#include <sstream>
#include <string>

#include <geos/geomgraph/EdgeIntersection.h>
#include <geos/geom/Coordinate.h>

using namespace std;
using namespace geos::geom;

namespace geos {
namespace geomgraph { // geos.geomgraph

EdgeIntersection::EdgeIntersection(const Coordinate& newCoord,
		int newSegmentIndex, double newDist)
	:
	coord(newCoord),
	segmentIndex(newSegmentIndex),
	dist(newDist)
{
}

EdgeIntersection::~EdgeIntersection()
{
}

int
EdgeIntersection::compare(int newSegmentIndex, double newDist) const
{
	if (segmentIndex<newSegmentIndex) return -1;
	if (segmentIndex>newSegmentIndex) return 1;
	if (dist<newDist) return -1;
	if (dist>newDist) return 1;
	return 0;
}

bool
EdgeIntersection::isEndPoint(int maxSegmentIndex)
{
	if (segmentIndex==0 && dist==0.0) return true;
	if (segmentIndex==maxSegmentIndex) return true;
	return false;
}

string
EdgeIntersection::print() const
{
	ostringstream s;
	s<<coord.toString()<<" seg#="<<segmentIndex<<" dist="<<dist;
	return s.str();

}

int
EdgeIntersection::compareTo(const EdgeIntersection *other) const
{
	return compare(other->segmentIndex, other->dist);
}

} // namespace geos.geomgraph
} // namespace geos


