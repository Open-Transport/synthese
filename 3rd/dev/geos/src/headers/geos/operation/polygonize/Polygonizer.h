/**********************************************************************
 * $Id: Polygonizer.h 2728 2009-11-19 19:57:36Z strk $
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
 * Last port: operation/polygonize/Polygonizer.java rev. 1.6 (JTS-1.10)
 *
 **********************************************************************/


#ifndef GEOS_OP_POLYGONIZE_POLYGONIZER_H
#define GEOS_OP_POLYGONIZE_POLYGONIZER_H

#include <geos/export.h>

#include <geos/geom/GeometryComponentFilter.h> // for LineStringAdder inheritance

#include <vector>

// Forward declarations
namespace geos {
	namespace geom { 
		class Geometry;
		class LineString;
		class Polygon;
	}
	namespace operation { 
		namespace polygonize { 
			class EdgeRing;
			class PolygonizeGraph;
		}
	}
}

namespace geos {
namespace operation { // geos::operation
namespace polygonize { // geos::operation::polygonize

/** \brief
 * Polygonizes a set of Geometrys which contain linework that
 * represents the edges of a planar graph.
 *
 * Any dimension of Geometry is handled - the constituent linework is extracted
 * to form the edges.
 * The edges must be correctly noded; that is, they must only meet
 * at their endpoints.  The Polygonizer will still run on incorrectly noded input
 * but will not form polygons from incorrected noded edges.
 * 
 * The Polygonizer reports the follow kinds of errors:
 * 
 * - <b>Dangles</b> - edges which have one or both ends which are
 *   not incident on another edge endpoint
 * - <b>Cut Edges</b> - edges which are connected at both ends but
 *   which do not form part of polygon
 * - <b>Invalid Ring Lines</b> - edges which form rings which are invalid
 *   (e.g. the component lines contain a self-intersection)
 *
 */
class GEOS_DLL Polygonizer {
private:
	/**
	 * Add every linear element in a geometry into the polygonizer graph.
	 */
	class LineStringAdder: public geom::GeometryComponentFilter {
	public:
		Polygonizer *pol;
		LineStringAdder(Polygonizer *p);
		//void filter_rw(geom::Geometry *g);
		void filter_ro(const geom::Geometry * g);
	};

	// default factory
	LineStringAdder *lineStringAdder;

	/**
	 * Add a linestring to the graph of polygon edges.
	 *
	 * @param line the {@link LineString} to add
	 */
	void add(const geom::LineString *line);

	/**
	 * Perform the polygonization, if it has not already been carried out.
	 */
	void polygonize();

	/// @todo : take all args by ref
	void findValidRings(std::vector<EdgeRing*>& edgeRingList,
			std::vector<EdgeRing*> *validEdgeRingList,
			std::vector<geom::LineString*> *invalidRingList);

	void findShellsAndHoles(std::vector<EdgeRing*> *edgeRingList);

	static void assignHolesToShells(std::vector<EdgeRing*> *holeList,
			std::vector<EdgeRing*> *shellList);

	static void assignHoleToShell(EdgeRing *holeER,
			std::vector<EdgeRing*> *shellList);

protected:

	PolygonizeGraph *graph;

	// initialize with empty collections, in case nothing is computed
	std::vector<const geom::LineString*> *dangles;
	std::vector<const geom::LineString*> *cutEdges;
	std::vector<geom::LineString*> *invalidRingLines;

	std::vector<EdgeRing*> *holeList;
	std::vector<EdgeRing*> *shellList;
	std::vector<geom::Polygon*> *polyList;

public:

	/** \brief
	 * Create a polygonizer with the same GeometryFactory
	 * as the input Geometry
	 */
	Polygonizer();

	~Polygonizer();

	/** \brief
	 * Add a collection of geometries to be polygonized.
	 * May be called multiple times.
	 * Any dimension of Geometry may be added;
	 * the constituent linework will be extracted and used
	 *
	 * @param geomList a list of Geometry with linework to be polygonized
	 */
	void add(std::vector<geom::Geometry*> *geomList);

        /** \brief
         * Add a collection of geometries to be polygonized.
         * May be called multiple times.
         * Any dimension of Geometry may be added;
         * the constituent linework will be extracted and used
         *
         * @param geomList a list of Geometry with linework to be polygonized
         */
	void add(std::vector<const geom::Geometry*> *geomList);

	/**
	 * Add a geometry to the linework to be polygonized.
	 * May be called multiple times.
	 * Any dimension of Geometry may be added;
	 * the constituent linework will be extracted and used
	 *
	 * @param g a Geometry with linework to be polygonized
	 */
	void add(geom::Geometry *g);

        /**
         * Add a geometry to the linework to be polygonized.
         * May be called multiple times.
         * Any dimension of Geometry may be added;
         * the constituent linework will be extracted and used
         *
         * @param g a Geometry with linework to be polygonized
         */
	void add(const geom::Geometry *g);

	/** \brief
	 * Gets the list of polygons formed by the polygonization.
	 *
	 * Ownership of vector is transferred to caller, subsequent
	 * calls will return NULL.
	 * @return a collection of Polygons
	 */
	std::vector<geom::Polygon*>* getPolygons();

	/** \brief
	 * Get the list of dangling lines found during polygonization.
	 *
	 * @return a collection of the input LineStrings which are dangles
	 */
	std::vector<const geom::LineString*>* getDangles();


	/** \brief
	 * Get the list of cut edges found during polygonization.
	 *
	 * @return a (possibly empty) reference to collection of the input
	 *         LineStrings which are cut edges. Ownership retained by
	 *         this object.
	 */
	std::vector<const geom::LineString*>* getCutEdges();

	/** \brief
	 * Get the list of lines forming invalid rings found during
	 * polygonization.
	 *
	 * Ownership is tranferred to caller, second call will return
	 * NULL (unless polygonize is called again).
	 * @return a collection of LineStrings which form
	 * invalid rings
	 */
	std::vector<geom::LineString*>* getInvalidRingLines();

// This seems to be needed by    GCC 2.95.4
friend class Polygonizer::LineStringAdder;
};

} // namespace geos::operation::polygonize
} // namespace geos::operation
} // namespace geos

#endif // GEOS_OP_POLYGONIZE_POLYGONIZER_H

/**********************************************************************
 * $Log$
 * Revision 1.2  2006/06/12 17:15:29  strk
 * Removed unused parameters warning
 *
 * Revision 1.1  2006/03/22 11:19:06  strk
 * opPolygonize.h headers split.
 *
 **********************************************************************/
