/**********************************************************************
 * $Id: LineMergeEdge.cpp 2562 2009-06-08 15:28:27Z strk $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2005-2006 Refractions Research Inc.
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************
 *
 * Last port: operation/linemerge/LineMergeEdge.java rev. 1.4 (JTS-1.10)
 *
 **********************************************************************/

#include <geos/operation/linemerge/LineMergeEdge.h>

using namespace geos::geom;

namespace geos {
namespace operation { // geos.operation
namespace linemerge { // geos.operation.linemerge

/**
* Constructs a LineMergeEdge with vertices given by the specified LineString.
*/
LineMergeEdge::LineMergeEdge(const LineString *newLine):
	line(newLine)
{
	//line=newLine;
}

/**
 * Returns the LineString specifying the vertices of this edge.
 */
const LineString *
LineMergeEdge::getLine() const {
	return line;
}

} // namespace geos.operation.linemerge
} // namespace geos.operation
} // namespace geos

/**********************************************************************
 * $Log$
 * Revision 1.6  2006/03/22 10:13:54  strk
 * opLinemerge.h split
 *
 **********************************************************************/

