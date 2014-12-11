/**********************************************************************
 * $Id: EdgeString.cpp 2562 2009-06-08 15:28:27Z strk $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2006 Refractions Research Inc.
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************
 *
 * Last port: operation/linemerge/EdgeString.java rev. 1.5 (JTS-1.10)
 *
 **********************************************************************/

#include <geos/operation/linemerge/EdgeString.h>
#include <geos/operation/linemerge/LineMergeEdge.h>
#include <geos/operation/linemerge/LineMergeDirectedEdge.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/LineString.h>

#include <vector>
#include <cassert>

using namespace std;
using namespace geos::geom;

namespace geos {
namespace operation { // geos.operation
namespace linemerge { // geos.operation.linemerge

/**
 * Constructs an EdgeString with the given factory used to convert
 * this EdgeString to a LineString
 */
EdgeString::EdgeString(const GeometryFactory *newFactory):
	factory(newFactory),
	directedEdges(new vector<LineMergeDirectedEdge*>()),
	coordinates(NULL)
{
}

EdgeString::~EdgeString() {
	delete directedEdges;
}

/**
 * Adds a directed edge which is known to form part of this line.
 */
void
EdgeString::add(LineMergeDirectedEdge *directedEdge)
{
	directedEdges->push_back(directedEdge);
}

CoordinateSequence *
EdgeString::getCoordinates()
{
	if (coordinates==NULL) {
		int forwardDirectedEdges = 0;
		int reverseDirectedEdges = 0;
		coordinates=factory->getCoordinateSequenceFactory()->create(NULL);
		for (int i=0;i<(int)directedEdges->size();i++) {
			LineMergeDirectedEdge* directedEdge=(*directedEdges)[i];
			if (directedEdge->getEdgeDirection()) {
				forwardDirectedEdges++;
			} else {
				reverseDirectedEdges++;
			}

			assert(dynamic_cast<LineMergeEdge*>(directedEdge->getEdge()));
			LineMergeEdge* lme=static_cast<LineMergeEdge*>(
				directedEdge->getEdge());

			coordinates->add(lme->getLine()->getCoordinatesRO(),
					false,
					directedEdge->getEdgeDirection());
		}
		if (reverseDirectedEdges > forwardDirectedEdges) {
			CoordinateSequence::reverse(coordinates);
		}
	}
	return coordinates;
}

/*
 * Converts this EdgeString into a new LineString.
 */
LineString*
EdgeString::toLineString()
{
	return factory->createLineString(getCoordinates());
}

} // namespace geos.operation.linemerge
} // namespace geos.operation
} // namespace geos

/**********************************************************************
 * $Log$
 * Revision 1.10  2006/03/22 10:13:54  strk
 * opLinemerge.h split
 *
 **********************************************************************/

