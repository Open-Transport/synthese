
/** PublicPlace class implementation.
	@file PublicPlace.cpp

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

#include "PublicPlace.h"
#include "Registry.h"

#include <geos/geom/Point.h>

using namespace std;
using namespace geos::geom;

namespace synthese
{
	using namespace road;
	using namespace geography;

	namespace util
	{
		template<> const string Registry<PublicPlace>::KEY("PublicPlace");
		template<> const string FactorableTemplate<NamedPlace, PublicPlace>::FACTORY_KEY("PublicPlace");
	}

	namespace road
	{
		PublicPlace::PublicPlace (
			util::RegistryKeyType id
		):	Registrable(id),
			NamedPlaceTemplate<PublicPlace>()
		{
		}

		PublicPlace::~PublicPlace ()
		{
		}



		std::string PublicPlace::getNameForAllPlacesMatcher(
			std::string text
		) const	{
			return (text.empty() ? getName() : text);
		}



		void PublicPlace::getVertexAccessMap( graph::VertexAccessMap& result, const graph::AccessParameters& accessParameters, const geography::Place::GraphTypes& whatToSearch ) const
		{
			/// @todo Place entrances
		}



		boost::shared_ptr<Point> PublicPlace::getPoint() const
		{
			/// @todo Envelope of entrances
			return boost::shared_ptr<Point>();
		}
	}
}
