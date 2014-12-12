/**********************************************************************
 * $Id: Point.h 2556 2009-06-06 22:22:28Z strk $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 * Copyright (C) 2005 2006 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************
 *
 * Last port: geom/Point.java rev. 1.37 (JTS-1.10)
 *
 **********************************************************************/

#ifndef GEOS_GEOS_POINT_H
#define GEOS_GEOS_POINT_H

#include <geos/export.h>
#include <geos/platform.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/CoordinateSequence.h> // for proper use of auto_ptr<>
#include <geos/geom/Envelope.h> // for proper use of auto_ptr<>
#include <geos/geom/Dimension.h> // for Dimension::DimensionType

#include <geos/inline.h>

#include <string>
#include <vector>
#include <memory> // for auto_ptr

// Forward declarations
namespace geos {
	namespace geom { // geos::geom
		class Coordinate;
		class CoordinateArraySequence;
		class CoordinateFilter;
		class CoordinateSequenceFilter;
		class GeometryComponentFilter;
		class GeometryFilter;
	}
}

namespace geos {
namespace geom { // geos::geom

/**
 * \class Point geom.h geos.h
 * \brief Basic implementation of Point.
 */
class GEOS_DLL Point : public Geometry 

	// NOTE: in JTS Point implements Puntual
{

public:

	friend class GeometryFactory;

	/// A vector of const Point pointers
	typedef std::vector<const Point *> ConstVect;

	virtual ~Point();

	/**
	 * Creates and returns a full copy of this {@link Point} object.
	 * (including all coordinates contained by it).
	 *
	 * @return a clone of this instance
	 */
	Geometry *clone() const { return new Point(*this); }

	CoordinateSequence* getCoordinates(void) const;

	const CoordinateSequence* getCoordinatesRO() const;

	size_t getNumPoints() const;
	bool isEmpty() const;
	bool isSimple() const;
	//bool isValid() const;

	/// Returns point dimension (0)
	Dimension::DimensionType getDimension() const;

	/// Returns Dimension::False (Point has no boundary)
	int getBoundaryDimension() const;

	/**
	 * Gets the boundary of this geometry.
	 * Zero-dimensional geometries have no boundary by definition,
	 * so an empty GeometryCollection is returned.
	 *
	 * @return an empty GeometryCollection
	 * @see Geometry::getBoundary
	 */
	Geometry* getBoundary() const;

	double getX() const;
	double getY() const;
	const Coordinate* getCoordinate() const;
	std::string getGeometryType() const;
	virtual GeometryTypeId getGeometryTypeId() const;
	void apply_ro(CoordinateFilter *filter) const;
	void apply_rw(const CoordinateFilter *filter);
	void apply_ro(GeometryFilter *filter) const;
	void apply_rw(GeometryFilter *filter);
	void apply_rw(GeometryComponentFilter *filter);
	void apply_ro(GeometryComponentFilter *filter) const;
	void apply_rw(CoordinateSequenceFilter& filter);
	void apply_ro(CoordinateSequenceFilter& filter) const;

	bool equalsExact(const Geometry *other, double tolerance=0) const;

	void normalize(void)
	{
		// a Point is always in normalized form
	}

  	Geometry* reverse() const
	{
		return clone();
	}

protected:

	/**
	 * \brief
	 * Creates a Point taking ownership of the given CoordinateSequence
	 * (must have 1 element)
	 *
	 * @param  newCoords
	 *	contains the single coordinate on which to base this
	 *	<code>Point</code> or <code>null</code> to create
	 *	the empty geometry.
	 *
	 * @param newFactory the GeometryFactory used to create this geometry
	 */  
	Point(CoordinateSequence *newCoords, const GeometryFactory *newFactory);

	Point(const Point &p); 

	Envelope::AutoPtr computeEnvelopeInternal() const;

	int compareToSameClass(const Geometry *p) const;

private:

	/**
	 *  The <code>Coordinate</code> wrapped by this <code>Point</code>.
	 */
	std::auto_ptr<CoordinateSequence> coordinates;
};

} // namespace geos::geom
} // namespace geos

//#ifdef GEOS_INLINE
//# include "geos/geom/Point.inl"
//#endif

#endif // ndef GEOS_GEOS_POINT_H

