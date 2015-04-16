/**********************************************************************
 * $Id: SnapOverlayOp.h 2809 2009-12-06 01:05:24Z mloskot $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2009  Sandro Santilli <strk@keybit.net>
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 ***********************************************************************
 *
 * Last port: operation/overlay/snap/SnapOverlayOp.java rev 1.4 (JTS-1.10)
 *
 **********************************************************************/

#ifndef GEOS_OP_OVERLAY_SNAP_SNAPOVERLAYOP_H
#define GEOS_OP_OVERLAY_SNAP_SNAPOVERLAYOP_H

#include <geos/operation/overlay/OverlayOp.h> // for enums 
#include <geos/precision/CommonBitsRemover.h> // for dtor visibility by auto_ptr

#include <memory> // for auto_ptr

// Forward declarations
namespace geos {
	namespace geom {
		class Geometry;
	}
}

namespace geos {
namespace operation { // geos::operation
namespace overlay { // geos::operation::overlay
namespace snap { // geos::operation::overlay::snap

/** \brief
 * Performs an overlay operation using snapping and enhanced precision
 * to improve the robustness of the result.
 *
 * This class <i>always</i> uses snapping.
 * This is less performant than the standard JTS overlay code,
 * and may even introduce errors which were not present in the original data.
 * For this reason, this class should only be used
 * if the standard overlay code fails to produce a correct result.
 *
 */
class GEOS_DLL SnapOverlayOp
{

public:

	static std::auto_ptr<geom::Geometry>
	overlayOp(const geom::Geometry& g0, const geom::Geometry& g1,
	          OverlayOp::OpCode opCode)
	{
		SnapOverlayOp op(g0, g1);
		return op.getResultGeometry(opCode);
	}

	static std::auto_ptr<geom::Geometry>
	intersection(const geom::Geometry& g0, const geom::Geometry& g1)
	{
		return overlayOp(g0, g1, OverlayOp::opINTERSECTION);
	}

	static std::auto_ptr<geom::Geometry>
	Union(const geom::Geometry& g0, const geom::Geometry& g1)
	{
		return overlayOp(g0, g1, OverlayOp::opUNION);
	}

	static std::auto_ptr<geom::Geometry>
	difference(const geom::Geometry& g0, const geom::Geometry& g1)
	{
		return overlayOp(g0, g1, OverlayOp::opDIFFERENCE);
	}

	static std::auto_ptr<geom::Geometry>
	symDifference(const geom::Geometry& g0, const geom::Geometry& g1)
	{
		return overlayOp(g0, g1, OverlayOp::opSYMDIFFERENCE);
	}

	SnapOverlayOp(const geom::Geometry& g1, const geom::Geometry& g2)
		:
		geom0(g1),
		geom1(g2)
	{
		computeSnapTolerance();
	}

	
	typedef std::auto_ptr<geom::Geometry> GeomPtr;

	GeomPtr getResultGeometry(OverlayOp::OpCode opCode);

private:

	void computeSnapTolerance();

	typedef std::pair<GeomPtr, GeomPtr> GeomPtrPair;

	void snap(GeomPtrPair& ret);

	void removeCommonBits(const geom::Geometry& geom0,
	                      const geom::Geometry& geom1, GeomPtrPair& ret);

	// re-adds common bits to the given geom
	void prepareResult(geom::Geometry& geom);


	const geom::Geometry& geom0;
	const geom::Geometry& geom1;

	double snapTolerance;

	std::auto_ptr<precision::CommonBitsRemover> cbr;

    // Declare type as noncopyable
    SnapOverlayOp(const SnapOverlayOp& other);
    SnapOverlayOp& operator=(const SnapOverlayOp& rhs);
};
 

} // namespace geos::operation::overlay::snap
} // namespace geos::operation::overlay
} // namespace geos::operation
} // namespace geos

#endif // ndef GEOS_OP_OVERLAY_SNAP_SNAPOVERLAYOP_H

/**********************************************************************
 * $Log$
 **********************************************************************/

