
/** WithGeometry class header.
	@file WithGeometry.hpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_WithGeometry_hpp__
#define SYNTHESE_WithGeometry_hpp__

#include "CoordinatesSystem.hpp"

#include <boost/shared_ptr.hpp>

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// Base class for objects with geometry.
	///	@ingroup m00
	///	@author Hugues Romain
	///	@since 3.2.0
	///	@date 2010
	template<class G>
	class WithGeometry
	{
	public:
		typedef G Geometry;

	private:
		boost::shared_ptr<G> _geometry;


	public:
		//////////////////////////////////////////////////////////////////////////
		/// Default constructor
		WithGeometry() {}



		//////////////////////////////////////////////////////////////////////////
		/// Copy constructor
		/// @param geometry geometry to copy
		WithGeometry(
			const boost::shared_ptr<G>& geometry
		):	_geometry(geometry.get() ? dynamic_cast<G*>(geometry->clone()) : NULL)
		{}



		//////////////////////////////////////////////////////////////////////////
		/// Copy constructor with spatial reference system conversion
		/// @param geometry geometry to convert
		/// @param spatialReference new spatial reference
		WithGeometry(
			const boost::shared_ptr<G>& geometry,
			const CoordinatesSystem& spatialReference
		):	_geometry(
			geometry.get() ?
			boost::static_pointer_cast<G, geos::geom::Geometry>(
				spatialReference.convertGeometry(static_cast<geos::geom::Geometry&>(*geometry))
			):
			boost::shared_ptr<G>()
		)
		{}



		virtual ~WithGeometry(){}



		//! @name Getters
		//@{
			const boost::shared_ptr<G>& getGeometry() const { return _geometry; }
		//@}



		//! @name Setters
		//@{
			void setGeometry(const boost::shared_ptr<G>& value){ _geometry = value; }
		//@}



		//! @name Modifiers
		//@{
			void resetGeometry(){ _geometry.reset(); }
			void cloneGeometry(const G& value){_geometry.reset(value.clone()); }
		//@}



		//! @name Services
		//@{
			bool hasGeometry() const { return _geometry.get() && !_geometry->isEmpty(); }
		//@}
	};
}

#endif // SYNTHESE_geography_CoordinatesSystem_hpp__
