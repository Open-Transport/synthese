/**********************************************************************
 * $Id: CoordinateList.h 2619 2009-08-12 19:01:57Z pramsey $
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
 **********************************************************************/

#ifndef GEOS_GEOM_COORDINATELIST_H
#define GEOS_GEOM_COORDINATELIST_H

#include <geos/export.h>
#include <geos/geom/Coordinate.h> 

#include <list>
#include <ostream> // for operator<<
#include <memory> // for auto_ptr 

// Forward declarations
namespace geos {
	namespace geom { 
		//class Coordinate;
	}
}


namespace geos {
namespace geom { // geos::geom

/**
 * A list of {@link Coordinate}s.
 *
 * Use this class when fast insertions and removal at arbitrary
 * position is needed.
 * The class keeps ownership of the Coordinates.
 *
 */
class GEOS_DLL CoordinateList {

public:

	typedef std::list<Coordinate>::iterator iterator;
	typedef std::list<Coordinate>::const_iterator const_iterator;
	typedef std::list<Coordinate>::size_type size_type;

	friend std::ostream& operator<< (std::ostream& os,
		const CoordinateList& cl);

	CoordinateList(const std::vector<Coordinate>& v)
		:
		coords(v.begin(), v.end())
	{
	}

	CoordinateList()
		:
		coords()
	{
	}

	size_type size() const
	{
		return coords.size();
	}

	iterator begin()
	{
		return coords.begin();
	}

	iterator end()
	{
		return coords.end();
	}

	const_iterator begin() const
	{
		return coords.begin();
	}

	const_iterator end() const
	{
		return coords.end();
	}

	iterator insert(iterator pos, const Coordinate& c)
	{
		return coords.insert(pos, c);
	}

	iterator erase(iterator pos)
	{
		return coords.erase(pos);
	}

	iterator erase(iterator first, iterator last)
	{
		return coords.erase(first, last);
	}

	std::auto_ptr<Coordinate::Vect> toCoordinateArray() const
	{
		std::auto_ptr<Coordinate::Vect> ret(new Coordinate::Vect);
		ret->assign(coords.begin(), coords.end());
		return ret;
	}

private:

	std::list<Coordinate> coords;


};

inline
std::ostream& operator<< (std::ostream& os, const CoordinateList& cl)
{
	os << "(";
	for (CoordinateList::const_iterator
		it=cl.begin(), end=cl.end();
		it != end;
		++it)
	{
		const Coordinate& c = *it;
		if ( it != cl.begin() ) os << ", ";
		os << c;
	}
	os << ")";

	return os;
}

} // namespace geos::geom
} // namespace geos


#endif // ndef GEOS_GEOM_COORDINATELIST_H

/**********************************************************************
 * $Log$
 * Revision 1.2  2006/07/21 17:05:22  strk
 * added operator<< for CoordinateList class
 *
 * Revision 1.1  2006/07/21 14:53:12  strk
 * CoordinateList class re-introduced, for list-based ops
 * (not strictly mapped to JTS version, not yet at least)
 *
 **********************************************************************/
