/**********************************************************************
 * $Id: Polygonizer.cpp 2730 2009-11-19 20:29:01Z strk $
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
 * Last port: operation/polygonize/Polygonizer.java rev. 1.6 (JTS-1.10)
 *
 **********************************************************************/

#include <geos/operation/polygonize/Polygonizer.h>
#include <geos/operation/polygonize/PolygonizeGraph.h>
#include <geos/operation/polygonize/EdgeRing.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/Polygon.h>
// std
#include <vector>

#ifdef _MSC_VER
#pragma warning(disable:4355)
#endif

#ifndef GEOS_DEBUG
#define GEOS_DEBUG 0
#endif

using namespace std;
using namespace geos::geom;

namespace geos {
namespace operation { // geos.operation
namespace polygonize { // geos.operation.polygonize

Polygonizer::LineStringAdder::LineStringAdder(Polygonizer *p):
	pol(p)
{
}

void
Polygonizer::LineStringAdder::filter_ro(const Geometry *g)
{
	const LineString *ls = dynamic_cast<const LineString *>(g);
	if ( ls ) pol->add(ls);
}


/*
 * Create a polygonizer with the same GeometryFactory
 * as the input Geometry
 */
Polygonizer::Polygonizer():
	lineStringAdder(new Polygonizer::LineStringAdder(this)),
	graph(NULL),
	dangles(NULL),
	cutEdges(NULL),
	invalidRingLines(NULL),
	holeList(NULL),
	shellList(NULL),
	polyList(NULL)
{
}

Polygonizer::~Polygonizer()
{
	delete lineStringAdder;
	delete dangles;
	delete cutEdges;
	delete graph;

	delete holeList;
	delete shellList;
	if ( invalidRingLines )
	{
		for (unsigned int i=0, n=invalidRingLines->size(); i<n; ++i)
			delete (*invalidRingLines)[i];
		delete invalidRingLines;
	}
	if ( polyList )
	{
		for (unsigned int i=0, n=polyList->size(); i<n; ++i)
			delete (*polyList)[i];
		delete polyList;
	}
}

/*
 * Add a collection of geometries to be polygonized.
 * May be called multiple times.
 * Any dimension of Geometry may be added;
 * the constituent linework will be extracted and used
 *
 * @param geomList a list of {@link Geometry}s with linework to be polygonized
 */
void
Polygonizer::add(vector<Geometry*> *geomList)
{
	for(unsigned int i=0, n=geomList->size(); i<n; ++i)
	{
		const Geometry *geometry=(*geomList)[i];
		add(geometry);
	}
}

/*
 * Add a collection of geometries to be polygonized.
 * May be called multiple times.
 * Any dimension of Geometry may be added;
 * the constituent linework will be extracted and used
 *
 * @param geomList a list of {@link Geometry}s with linework to be polygonized
 */
void
Polygonizer::add(vector<const Geometry*> *geomList)
{
	for(unsigned int i=0, n=geomList->size(); i<n; ++i)
	{
		const Geometry *geometry=(*geomList)[i];
		add(geometry);
	}
}

/*
 * Add a geometry to the linework to be polygonized.
 * May be called multiple times.
 * Any dimension of Geometry may be added;
 * the constituent linework will be extracted and used
 *
 * @param g a Geometry with linework to be polygonized
 */
void
Polygonizer::add(Geometry *g)
{
	g->apply_ro(lineStringAdder);
}

/*
 * Add a geometry to the linework to be polygonized.
 * May be called multiple times.
 * Any dimension of Geometry may be added;
 * the constituent linework will be extracted and used
 *
 * @param g a Geometry with linework to be polygonized
 */
void
Polygonizer::add(const Geometry *g)
{
	g->apply_ro(lineStringAdder);
}

/*
 * Add a linestring to the graph of polygon edges.
 *
 * @param line the LineString to add
 */
void
Polygonizer::add(const LineString *line)
{
	// create a new graph using the factory from the input Geometry
	if (graph==NULL)
		graph=new PolygonizeGraph(line->getFactory());
	graph->addEdge(line);
}

/*
 * Gets the list of polygons formed by the polygonization.
 * @return a collection of Polygons
 */
vector<Polygon*>*
Polygonizer::getPolygons()
{
	polygonize();
	vector<Polygon *> *ret = polyList;
	polyList = NULL;
	return ret;
}

/*
 * Get the list of dangling lines found during polygonization.
 * @return a collection of dangles LineStrings from input.
 */
vector<const LineString*>*
Polygonizer::getDangles()
{
	polygonize();
	return dangles;
}

/*
 * Get the list of cut edges found during polygonization.
 * @return a collection of the input {@LineStrings} which are cut edges
 */
vector<const LineString*>*
Polygonizer::getCutEdges()
{
	polygonize();
	return cutEdges;
}

/*
 * Get the list of lines forming invalid rings found during polygonization.
 * @return a collection of the input {@LineStrings} which form invalid rings
 */
vector<LineString*>*
Polygonizer::getInvalidRingLines()
{
	polygonize();
	vector<LineString*> *ret = invalidRingLines;
	invalidRingLines = NULL;
	return ret;
}

/*
 * Perform the polygonization, if it has not already been carried out.
 */
void
Polygonizer::polygonize()
{
	// check if already computed
	if (polyList!=NULL) return;

	polyList=new vector<Polygon*>();

	// if no geometries were supplied it's possible graph could be null
	if (graph==NULL) return; 

	// TODO: drop this heap allocation
	dangles = new std::vector<const LineString*>();
	graph->deleteDangles(*dangles);

	// TODO: drop this heap allocation
	cutEdges = new std::vector<const LineString*>();
	graph->deleteCutEdges(*cutEdges);

	vector<EdgeRing*> edgeRingList;
	graph->getEdgeRings(edgeRingList);
#if GEOS_DEBUG
	cerr<<"Polygonizer::polygonize(): "<<edgeRingList.size()<<" edgeRings in graph"<<endl;
#endif
	vector<EdgeRing*> *validEdgeRingList=new vector<EdgeRing*>();
	invalidRingLines=new vector<LineString*>();
	findValidRings(edgeRingList, validEdgeRingList, invalidRingLines);
#if GEOS_DEBUG
	cerr<<"                           "<<validEdgeRingList->size()<<" valid"<<endl;
	cerr<<"                           "<<invalidRingLines->size()<<" invalid"<<endl;
#endif

	findShellsAndHoles(validEdgeRingList);
#if GEOS_DEBUG
	cerr<<"                           "<<holeList->size()<<" holes"<<endl;
	cerr<<"                           "<<shellList->size()<<" shells"<<endl;
#endif

	assignHolesToShells(holeList, shellList);

	for (unsigned int i=0, n=shellList->size(); i<n; ++i)
	{
		EdgeRing *er=(*shellList)[i];
		polyList->push_back(er->getPolygon());
	}
	delete validEdgeRingList;
}

/* private */
void
Polygonizer::findValidRings(vector<EdgeRing*>& edgeRingList,
	vector<EdgeRing*> *validEdgeRingList,
	vector<LineString*> *invalidRingList)
{
	typedef vector<EdgeRing*> EdgeRingList;
	
	for (EdgeRingList::size_type i=0, n=edgeRingList.size(); i<n; ++i)
	{
		EdgeRing *er = edgeRingList[i];
		if (er->isValid())
			validEdgeRingList->push_back(er);
		else
		{
			invalidRingList->push_back(er->getLineString());
		}
	}
}

void
Polygonizer::findShellsAndHoles(vector<EdgeRing*> *edgeRingList)
{
	holeList=new vector<EdgeRing*>();
	shellList=new vector<EdgeRing*>();
	for (unsigned int i=0, n=edgeRingList->size(); i<n; ++i)
	{
		EdgeRing *er=(*edgeRingList)[i];
		if (er->isHole())
			holeList->push_back(er);
		else
			shellList->push_back(er);
	}
}

void
Polygonizer::assignHolesToShells(vector<EdgeRing*> *holeList,vector<EdgeRing*> *shellList)
{
	for (unsigned int i=0, n=holeList->size(); i<n; ++i)
	{
		EdgeRing *holeER=(*holeList)[i];
		assignHoleToShell(holeER,shellList);
	}
}

void
Polygonizer::assignHoleToShell(EdgeRing *holeER,
		vector<EdgeRing*> *shellList)
{
	EdgeRing *shell=EdgeRing::findEdgeRingContaining(holeER, shellList);

	if (shell!=NULL)
		shell->addHole(holeER->getRingOwnership());
}


} // namespace geos.operation.polygonize
} // namespace geos.operation
} // namespace geos

/**********************************************************************
 * $Log$
 * Revision 1.13  2006/03/22 11:19:06  strk
 * opPolygonize.h headers split.
 *
 * Revision 1.12  2006/03/03 10:46:22  strk
 * Removed 'using namespace' from headers, added missing headers in .cpp files, removed useless includes in headers (bug#46)
 *
 * Revision 1.11  2006/03/02 12:12:01  strk
 * Renamed DEBUG macros to GEOS_DEBUG, all wrapped in #ifndef block to allow global override (bug#43)
 *
 * Revision 1.10  2006/02/19 19:46:49  strk
 * Packages <-> namespaces mapping for most GEOS internal code (uncomplete, but working). Dir-level libs for index/ subdirs.
 *
 * Revision 1.9  2005/12/09 10:32:28  strk
 * Cleaned up debugging line left over from previous commit
 *
 * Revision 1.8  2005/12/09 10:03:46  strk
 * Fixed a bug making PolygonizeGraph choking on invalid LineStrings.
 * Minor optimizations in Polygonizer loops.
 *
 * Revision 1.7  2005/06/17 15:08:06  strk
 * Polygonizer segfault fix
 *
 * Revision 1.6  2004/12/08 13:54:44  strk
 * gcc warnings checked and fixed, general cleanups.
 *
 * Revision 1.5  2004/10/27 13:57:07  strk
 * Added some debugging lines (disabled by default)
 *
 * Revision 1.4  2004/10/26 16:09:21  strk
 * Some more intentation and envelope equality check fix.
 *
 * Revision 1.3  2004/10/19 19:51:14  strk
 * Fixed many leaks and bugs in Polygonizer.
 * Output still bogus.
 *
 * Revision 1.2  2004/07/02 13:28:29  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.1  2004/04/08 04:53:56  ybychkov
 * "operation/polygonize" ported from JTS 1.4
 *
 *
 **********************************************************************/
