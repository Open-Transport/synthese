
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
#include "GeographyModule.h"
#include "DBTypes.h"

#include <string>
#include <map>
#include <proj_api.h>
#include <boost/lexical_cast.hpp>

namespace synthese
{
	namespace geography
	{
		/** CoordinatesSystem class.
			@ingroup m32
			@author Hugues Romain
			@since 3.2.0
			@date 2010
		*/
		class CoordinatesSystem
		{
		private:
			db::SRID _srid;
			std::string _name;
			std::string _projSequence;
			projPJ _projObject;

			typedef std::map<db::SRID, CoordinatesSystem> Map;

			////////////////////////////////////////////////////////////////////
			/// All coordinates systems.
			static Map _coordinates_systems;

		public:
			class NotFoundException:
				public util::Exception
			{
			public:
				NotFoundException(db::SRID srid):
				  util::Exception("Coordinates system "+ boost::lexical_cast<std::string>(srid) +" not found")
				{}
			};

			CoordinatesSystem(
				db::SRID srid,
				const std::string& name,
				const std::string& projSequence
			):	_srid(srid),
				_name(name),
				_projSequence(projSequence),
				_projObject(pj_init_plus(projSequence.c_str()))
			{}

			CoordinatesSystem() {}

			//! @name Getters
			//@{
				const projPJ& getProjObject() const { return _projObject; }
				const std::string& getName() const { return _name; }
			//@}

				
			static void AddCoordinatesSystem(
				db::SRID srid,
				const std::string& name,
				const std::string& projSequence
			);


			//////////////////////////////////////////////////////////////////////////
			/// @throws NotFoundException if the system was not found
			static const CoordinatesSystem& GetCoordinatesSystem(db::SRID srid);
		};
	}
}

#endif // SYNTHESE_geography_CoordinatesSystem_hpp__
