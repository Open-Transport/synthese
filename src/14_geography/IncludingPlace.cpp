
/** IncludingPlace class implementation.
	@file IncludingPlace.cpp

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

#include "IncludingPlace.h"

#include <boost/foreach.hpp>

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace graph;

	namespace geography
	{
		IncludingPlace::IncludingPlace(
		):	Place()			
		{
		}



		IncludingPlace::~IncludingPlace ()
		{
		}



		const IncludingPlace::IncludedPlaces& 
		IncludingPlace::getIncludedPlaces () const
		{
			return _includedPlaces;
		}
		    


		void 
		IncludingPlace::addIncludedPlace (const Place* place)
		{
			_isoBarycentreToUpdate = true;
			_includedPlaces.insert(place);
		}



		void IncludingPlace::getVertexAccessMap(
			VertexAccessMap& result
			, const AccessDirection& accessDirection
			, const AccessParameters& accessParameters
			, const GraphTypes& whatToSearch
		) const	{
			BOOST_FOREACH(
				const Place* place, _includedPlaces
			){
				place->getVertexAccessMap(
					result, accessDirection, accessParameters
					, whatToSearch
				);
			}
		}

		const geometry::Point2D& IncludingPlace::getPoint() const
		{
			if (_isoBarycentreToUpdate)
			{
				_isoBarycentre.clear();
				BOOST_FOREACH(const Place* place, _includedPlaces)
				{
					_isoBarycentre.add(place->getPoint());
				}
				_isoBarycentreToUpdate = false;
			}
			return _isoBarycentre;
		}

		bool IncludingPlace::includes( const Place* place ) const
		{
			if (place == this)
				return true;

			for (IncludedPlaces::const_iterator it(_includedPlaces.begin()); it != _includedPlaces.end(); ++it)
				if (*it == place)
					return true;
			return false;
		}



		void IncludingPlace::removeIncludedPlace( const Place* place )
		{
			_isoBarycentreToUpdate = true;
			_includedPlaces.erase(place);
		}
	}
}
