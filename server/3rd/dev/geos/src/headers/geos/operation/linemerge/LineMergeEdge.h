/**********************************************************************
 * $Id: LineMergeEdge.h 2562 2009-06-08 15:28:27Z strk $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2006 Refractions Research Inc.
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

#ifndef GEOS_OP_LINEMERGE_LINEMERGEEDGE_H
#define GEOS_OP_LINEMERGE_LINEMERGEEDGE_H

#include <geos/export.h>

#include <geos/planargraph/Edge.h> // for inheritance

// Forward declarations 
namespace geos {
	namespace geom { 
		class LineString;
	}
}


namespace geos {
namespace operation { // geos::operation
namespace linemerge { // geos::operation::linemerge

/** \brief
 * An edge of a LineMergeGraph. The <code>marked</code> field indicates
 * whether this Edge has been logically deleted from the graph.
 */
class GEOS_DLL LineMergeEdge: public planargraph::Edge {
private:
	const geom::LineString *line;
public:
	/**
	 * Constructs a LineMergeEdge with vertices given by the specified
	 * LineString.
	 */
	LineMergeEdge(const geom::LineString *newLine);

	/**
	 * Returns the LineString specifying the vertices of this edge.
	 */
	const geom::LineString* getLine() const;
};


} // namespace geos::operation::linemerge
} // namespace geos::operation
} // namespace geos

#endif // GEOS_OP_LINEMERGE_LINEMERGEEDGE_H

/**********************************************************************
 * $Log$
 * Revision 1.1  2006/03/22 10:13:53  strk
 * opLinemerge.h split
 *
 **********************************************************************/
