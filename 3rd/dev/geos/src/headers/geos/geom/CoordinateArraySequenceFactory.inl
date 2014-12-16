/**********************************************************************
 * $Id: CoordinateArraySequenceFactory.inl 1820 2006-09-06 16:54:23Z mloskot $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2005-2006 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************/

#ifndef GEOS_GEOM_COORDINATEARRAYSEQUENCEFACTORY_INL
#define GEOS_GEOM_COORDINATEARRAYSEQUENCEFACTORY_INL

#include <cassert>
#include <geos/geom/CoordinateArraySequenceFactory.h>
#include <geos/geom/CoordinateArraySequence.h>

namespace geos {
namespace geom { // geos::geom

INLINE CoordinateSequence*
CoordinateArraySequenceFactory::create(std::vector<Coordinate> *coords) const
{
	return new CoordinateArraySequence(coords);
}

INLINE CoordinateSequence *
CoordinateArraySequenceFactory::create(std::vector<Coordinate> *coords,
		size_t /* dimension */) const
{
	return new CoordinateArraySequence(coords);
}

INLINE CoordinateSequence *
CoordinateArraySequenceFactory::create(size_t size, size_t /* dimension */)
		const
{
	/* CoordinateArraySequence only accepts 3d Coordinates */
	return new CoordinateArraySequence(size);
}


} // namespace geos::geom
} // namespace geos

#endif // GEOS_GEOM_COORDINATEARRAYSEQUENCEFACTORY_INL

