
/** FrameworkTypes class header.
	@file FrameworkTypes.hpp

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

#ifndef SYNTHESE__FrameworkTypes_hpp__
#define SYNTHESE__FrameworkTypes_hpp__

#include "UtilTypes.h" // TODO : merge this file in the current one

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>
#include <string>
#include <vector>

namespace geos
{
	namespace geom
	{
		class Geometry;
	}
}

namespace synthese
{
	typedef std::vector<util::RegistryKeyType> LinkedObjectsIds;

	typedef std::pair<char*, size_t> Blob;
	typedef boost::variant<
		boost::optional<std::string>,
		bool,
		int,
		size_t,
		double,
		util::RegistryKeyType,
		boost::optional<Blob>,
		boost::shared_ptr<geos::geom::Geometry> // Will be useless when all tables will be migrated in the fusion mode (GeometryField produces directly WKT string)
	> Cell;

	typedef std::vector<Cell> DBContent;
}

#endif // SYNTHESE__FrameworkTypes_hpp__

