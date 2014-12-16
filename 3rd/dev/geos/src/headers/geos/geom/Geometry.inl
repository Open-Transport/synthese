/**********************************************************************
 * $Id: Geometry.inl 2465 2009-05-04 23:23:10Z strk $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2009  Sandro Santilli <strk@keybit.net>
 * Copyright (C) 2005-2006 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************
 *
 * Last port: geom/Geometry.java rev. 1.112
 *
 **********************************************************************/

#ifndef GEOS_GEOM_GEOMETRY_INL
#define GEOS_GEOM_GEOMETRY_INL

#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>

namespace geos {
namespace geom { // geos::geom

INLINE const PrecisionModel*
Geometry::getPrecisionModel() const
{
	return factory->getPrecisionModel();
}

} // namespace geos::geom
} // namespace geos

#endif // GEOS_GEOM_GEOMETRY_INL


