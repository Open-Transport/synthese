/**********************************************************************
 * $Id: LineStringSnapper.cpp 2809 2009-12-06 01:05:24Z mloskot $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2009  Sandro Santilli <strk@keybit.net>
 * Copyright (C) 2006 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 ***********************************************************************
 *
 * Last port: operation/overlay/snap/lineStringSnapper.java rev 1.5 (JTS-1.10)
 *
 **********************************************************************/

#include <geos/operation/overlay/snap/LineStringSnapper.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateList.h>
#include <geos/util/UniqueCoordinateArrayFilter.h>
#include <geos/geom/LineSegment.h>

#include <vector>
#include <memory>

#ifndef GEOS_DEBUG
#define GEOS_DEBUG 0
#endif

#if GEOS_DEBUG
#include <iostream>
using std::cerr;
using std::endl;
#endif

//using namespace std;
using namespace geos::geom;

namespace geos {
namespace operation { // geos.operation
namespace overlay { // geos.operation.overlay
namespace snap { // geos.operation.overlay.snap

/*public*/
std::auto_ptr<Coordinate::Vect>
LineStringSnapper::snapTo(const geom::Coordinate::ConstVect& snapPts)
{
	geom::CoordinateList coordList(srcPts);

	snapVertices(coordList, snapPts);
	snapSegments(coordList, snapPts);

	return coordList.toCoordinateArray();
}

/*private*/
void
LineStringSnapper::snapVertices(geom::CoordinateList& srcCoords,
			const geom::Coordinate::ConstVect& snapPts)
{
	using geom::CoordinateList;

	geom::Coordinate::ConstVect::const_iterator not_found = snapPts.end();

	// try snapping vertices
	// assume src list has a closing point (is a ring)
	CoordinateList::iterator it = srcCoords.begin();
	CoordinateList::iterator end = srcCoords.end(); 
	CoordinateList::iterator last = end; --last;
	for ( ; it != end; ++it )
	{
		Coordinate& srcPt = *it;

#if GEOS_DEBUG
cerr << "Checking for a snap for source coordinate " << srcPt << endl;
#endif

		geom::Coordinate::ConstVect::const_iterator found = findSnapForVertex(srcPt, snapPts);
		if ( found == not_found )
		{	// no snaps found (or no need to snap)
#if GEOS_DEBUG
cerr << "No snap found" << endl;
#endif
			continue;
		}

		assert(*found);
		const Coordinate& snapPt = *(*found);
		
#if GEOS_DEBUG
cerr << "Found snap point " << snapPt << endl;
#endif

		// update src with snap pt
		*it = snapPt;

#if GEOS_DEBUG
cerr << "Source point became " << srcPt << endl;
#endif

		// keep final closing point in synch (rings only)
		if (it == srcCoords.begin() && isClosed)
		{
			*last = snapPt;
		}
	}
}

/*private*/
Coordinate::ConstVect::const_iterator
LineStringSnapper::findSnapForVertex(const Coordinate& pt,
			const Coordinate::ConstVect& snapPts)
{

	// TODO: use std::find_if
	Coordinate::ConstVect::const_iterator end=snapPts.end();
	for ( Coordinate::ConstVect::const_iterator
			it=snapPts.begin();
			it != end;
			++it)
	{
		assert(*it);
		const Coordinate& snapPt = *(*it);

#if GEOS_DEBUG
cerr << " misuring distance between snap point " << snapPt << " and source point " << pt << endl;
#endif

		// shouldn't we look for *all* segments to be snapped rather then a single one?
		if ( snapPt.equals2D(pt) )
		{
#if GEOS_DEBUG
cerr << " points are equal, returning not-found " << endl;
#endif
			return end;
			//continue;
		}

		double dist = snapPt.distance(pt);
		if ( dist < snapTolerance )
		{
#if GEOS_DEBUG
cerr << " points are within distance (" << dist << ") returning iterator to snap point" << endl;
#endif
			return it;
		}
	}

#if GEOS_DEBUG
cerr << " No snap point within distance, returning not-found" << endl;
#endif

	return end;
}


/*private*/
void
LineStringSnapper::snapSegments(geom::CoordinateList& srcCoords,
			const geom::Coordinate::ConstVect& snapPts)
{

#if GEOS_DEBUG
cerr << " Snapping segment from: " << srcCoords << endl;
#endif

	for ( Coordinate::ConstVect::const_iterator
			it=snapPts.begin(), end=snapPts.end();
			it != end;
			++it)
	{
		assert(*it);
		const Coordinate& snapPt = *(*it);

#if GEOS_DEBUG
cerr << "Checking for a segment to snap to snapPt " << snapPt << endl;
#endif

		// shouldn't we look for *all* segments to be snapped
		// rather then a single one?
		CoordinateList::iterator too_far = srcCoords.end(); --too_far;
		CoordinateList::iterator segpos =
			findSegmentToSnap(snapPt, srcCoords.begin(), too_far);
		if ( segpos == too_far)
		{
#if GEOS_DEBUG
cerr << " No segment to snap" << endl;
#endif
			continue;
		}
#if GEOS_DEBUG
cerr << " Segment to be snapped found, inserting point" << endl;
#endif
		// insert must happen one-past first point (before next point)
		++segpos;
		srcCoords.insert(segpos, snapPt);
	}

#if GEOS_DEBUG
cerr << " After segment snapping, srcCoors are: " << srcCoords << endl;
#endif

}

/*private*/
CoordinateList::iterator
LineStringSnapper::findSegmentToSnap(
			const Coordinate& snapPt,
			CoordinateList::iterator from,
			CoordinateList::iterator too_far)
{
	LineSegment seg;
	double minDist = snapTolerance+1; // make sure the first closer then
	                                  // snapTolerance is accepted
	CoordinateList::iterator match=too_far;

	// TODO: use std::find_if
	for ( ; from != too_far; ++from)
	{
		seg.p0 = *from; 
		CoordinateList::iterator to = from;
		++to;
		seg.p1 = *to;

#if GEOS_DEBUG
cerr << " Checking segment " << seg << " for snapping against point " << snapPt << endl;
#endif

		if ( seg.p0.equals2D(snapPt) || seg.p1.equals2D(snapPt) )
		{

#if GEOS_DEBUG
cerr << " One of segment endpoints equal snap point, returning too_far" << endl;
#endif
			// If the snap pt is already in the src list,
			// don't snap
			return too_far;
		}

		double dist = seg.distance(snapPt);
#if GEOS_DEBUG
cerr << " dist=" << dist << " minDist=" << minDist << " snapTolerance=" << snapTolerance << endl;
#endif
		if ( dist < minDist && dist < snapTolerance )
		{
#if GEOS_DEBUG
cerr << " Segment/snapPt distance within tolerance and closer then previous match (" << dist << ") " << endl;
#endif
			match = from;
			minDist = dist;
		}
	}

	return match;
}

} // namespace geos.operation.snap
} // namespace geos.operation.overlay
} // namespace geos.operation
} // namespace geos

