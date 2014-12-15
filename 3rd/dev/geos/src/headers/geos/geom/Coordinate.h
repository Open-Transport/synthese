/**********************************************************************
 * $Id: Coordinate.h 2556 2009-06-06 22:22:28Z strk $
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

#ifndef GEOS_GEOM_COORDINATE_H
#define GEOS_GEOM_COORDINATE_H

#include <geos/export.h>
#include <geos/platform.h> // for DoubleNotANumber
#include <geos/inline.h>
#include <set>
#include <stack>
#include <vector> // for typedefs
#include <string>
#include <limits>

namespace geos {
namespace geom { // geos.geom

struct CoordinateLessThen;

/**
 * \class Coordinate geom.h geos.h
 *
 * \brief
 * Coordinate is the lightweight class used to store coordinates.
 *
 * It is distinct from Point, which is a subclass of Geometry.
 * Unlike objects of type Point (which contain additional
 * information such as an envelope, a precision model, and spatial
 * reference system information), a Coordinate only contains
 * ordinate values and accessor methods. 
 *
 * Coordinate objects are two-dimensional points, with an additional
 * z-ordinate. JTS does not support any operations on the z-ordinate except
 * the basic accessor functions.
 *
 * Constructed coordinates will have a z-ordinate of DoubleNotANumber.
 * The standard comparison functions will ignore the z-ordinate.
 *
 */
// Define the following to make assignments and copy constructions 
// NON-inline (will let profilers report usages)
//#define PROFILE_COORDINATE_COPIES 1
class GEOS_DLL Coordinate {

private:

	static Coordinate nullCoord;

public:
	/// A set of const Coordinate pointers
	typedef std::set<const Coordinate *, CoordinateLessThen> ConstSet;

	/// A vector of const Coordinate pointers
	typedef std::vector<const Coordinate *> ConstVect;

	/// A stack of const Coordinate pointers
	typedef std::stack<const Coordinate *> ConstStack;

	/// A vector of Coordinate objects (real object, not pointers)
	typedef std::vector<Coordinate> Vect;

	/// x-coordinate
	double x;

	/// y-coordinate
	double y;

	/// z-coordinate
	double z;

	void setNull();

	static Coordinate& getNull();

	bool isNull() const;

	~Coordinate();

	Coordinate(double xNew=0.0, double yNew=0.0, double zNew=DoubleNotANumber);

// Letting the compiler synthetize these gives us more numerical stability
#if 0
	Coordinate(const Coordinate& c);

	Coordinate &operator=(const Coordinate &c);
#endif

	bool equals2D(const Coordinate& other) const;

	/// 2D only
	bool equals(const Coordinate& other) const;

	/// TODO: deprecate this, move logic to CoordinateLessThen instead
	int compareTo(const Coordinate& other) const;

	/// 3D comparison 
	bool equals3D(const Coordinate& other) const;

	///  Returns a string of the form <I>(x,y,z)</I> .
	std::string toString() const;

	/// TODO: obsoleted this, can use PrecisionModel::makePrecise(Coordinate*)
	/// instead
	//void makePrecise(const PrecisionModel *pm);

	double distance(const Coordinate& p) const;

	int hashCode() const;

	/**
	 * Returns a hash code for a double value, using the algorithm from
	 * Joshua Bloch's book <i>Effective Java</i>
	 */
	static int hashCode(double d);

};

/// Strict weak ordering Functor for Coordinate
struct GEOS_DLL CoordinateLessThen {

	bool operator()(const Coordinate* a, const Coordinate* b) const;
	bool operator()(const Coordinate& a, const Coordinate& b) const;

};

/// Output function
GEOS_DLL std::ostream& operator<< (std::ostream& os, const Coordinate& c);

/// Equality operator for Coordinate. 2D only.
GEOS_DLL bool operator==(const Coordinate& a, const Coordinate& b);

/// Inequality operator for Coordinate. 2D only.
GEOS_DLL bool operator!=(const Coordinate& a, const Coordinate& b);



} // namespace geos.geom
} // namespace geos

#ifdef GEOS_INLINE
# include "geos/geom/Coordinate.inl"
#endif

#endif // ndef GEOS_GEOM_COORDINATE_H

/**********************************************************************
 * $Log$
 * Revision 1.6  2006/06/14 19:27:02  strk
 * Let the compiler synthetize copy ctor and assignment op for Coordinate class to obtain better numerical stability.
 *
 * Revision 1.5  2006/05/23 15:06:07  strk
 * * source/headers/geos/geom/Coordinate.h: added missing <string> include.
 *
 * Revision 1.4  2006/03/27 15:57:39  strk
 * Commented need for platform.h include
 *
 * Revision 1.3  2006/03/24 09:52:41  strk
 * USE_INLINE => GEOS_INLINE
 *
 * Revision 1.2  2006/03/20 10:12:45  strk
 * Bug #70 - Small fix in fwd decl. of CoordinateLessThen
 *
 * Revision 1.1  2006/03/09 16:46:49  strk
 * geos::geom namespace definition, first pass at headers split
 *
 **********************************************************************/
