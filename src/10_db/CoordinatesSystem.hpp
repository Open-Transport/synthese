
/** CoordinatesSystem class header.
	@file CoordinatesSystem.hpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#ifndef SYNTHESE_geography_CoordinatesSystem_hpp__
#define SYNTHESE_geography_CoordinatesSystem_hpp__

#include "Exception.h"
#include "DBTypes.h"

#include <string>
#include <map>
#include <proj_api.h>
#include <boost/lexical_cast.hpp>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/PrecisionModel.h>

namespace synthese
{
	namespace db
	{
		/** CoordinatesSystem class.
			@ingroup m10
			@author Hugues Romain
			@since 3.2.0
			@date 2010
		*/
		class CoordinatesSystem
		{
		private:
			SRID _srid;
			std::string _name;
			std::string _projSequence;
			projPJ _projObject;
			geos::geom::GeometryFactory _geometryFactory;

		public:

			CoordinatesSystem(
				db::SRID srid,
				const std::string& name,
				const std::string& projSequence
			):	_srid(srid),
				_name(name),
				_projSequence(projSequence),
				_projObject(pj_init_plus(projSequence.c_str())),
				_geometryFactory(new geos::geom::PrecisionModel(), srid)
			{}

			CoordinatesSystem() {}

			//! @name Getters
			//@{
				const projPJ& getProjObject() const { return _projObject; }
				const geos::geom::GeometryFactory& getGeometryFactory() const { return _geometryFactory; }
				const std::string& getName() const { return _name; }
				SRID getSRID() const { return _srid; }
			//@}

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Conversion of the coordinate system of a point.
				//////////////////////////////////////////////////////////////////////////
				/// @param source point to convert
				/// @return result of the conversion
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.2.0
				boost::shared_ptr<geos::geom::Point> convertPoint(const geos::geom::Point& source) const;
			//@}
		};
}	}

#endif // SYNTHESE_geography_CoordinatesSystem_hpp__
