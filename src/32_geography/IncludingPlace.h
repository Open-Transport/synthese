
/** IncludingPlace class header.
	@file IncludingPlace.h

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

#ifndef SYNTHESE_ENV_INCLUDINGPLACE_H
#define SYNTHESE_ENV_INCLUDINGPLACE_H

#include "Place.h"

#include "CoordinatesSystem.hpp"

#include <set>
#include <boost/foreach.hpp>
#include <geos/geom/Envelope.h>
#include <geos/geom/Point.h>

#ifdef _MSC_VER
#pragma warning( disable : 4250 )
#endif

namespace synthese
{
	namespace geography
	{
		/** Base class for a place including other places.

			@ingroup m32
		*/
		template<class Included>
		class IncludingPlace:
			public virtual Place
		{
		public:
			typedef std::set<const Included*> IncludedPlaces;

		private:
			IncludedPlaces _includedPlaces;

		protected:
			IncludingPlace(): Place() {}

		public:

			virtual ~IncludingPlace() {}


			//! @name Getters/Setters
			//@{

				/** Gets included places.
				 */
				const IncludedPlaces& getIncludedPlaces() const { return _includedPlaces; }
			//@}


			//! @name Query methods
			//@{

				virtual void getVertexAccessMap(
					graph::VertexAccessMap& result,
					const graph::AccessParameters& accessParameters,
					const GraphTypes& whatToSearch
				) const;

				virtual const boost::shared_ptr<geos::geom::Point>& getPoint() const;

				virtual bool includes(const Included& place) const;
			//@}

			//! @name Update methods.
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Adds a place to the included places list.
				/// @param place Place to include
				///	This methods cancels the caching of the centroid.
				void addIncludedPlace (const Included& place);



				//////////////////////////////////////////////////////////////////////////
				/// Removes a place to the included places list.
				/// @param place place to remove
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				/// This methods cancels the caching of the centroid.
				/// If the place was not included, the method do nothing.
				void removeIncludedPlace(const Included& place);



				//////////////////////////////////////////////////////////////////////////
				/// Removes all places to the included places list.
				/// @author Hugues Romain
				/// @date 2012
				/// @since 3.3.0
				/// This methods cancels the caching of the centroid.
				void clearIncludedPlaces();
			//@}
		};



		template<class Included>
		void IncludingPlace<Included>::getVertexAccessMap(
			graph::VertexAccessMap& result,
			const graph::AccessParameters& accessParameters,
			const GraphTypes& whatToSearch
		) const	{
			BOOST_FOREACH(const Included* place, _includedPlaces)
			{
				place->getVertexAccessMap(
					result,
					accessParameters,
					whatToSearch
				);
			}
		}



		template<class Included>
		const boost::shared_ptr<geos::geom::Point>& IncludingPlace<Included>::getPoint() const
		{
			if (!_isoBarycentre.get())
			{
				geos::geom::Envelope e;
				BOOST_FOREACH(const Included* place, _includedPlaces)
				{
					if(place->getPoint().get() && !place->getPoint()->isEmpty())
					{
						e.expandToInclude(*place->getPoint()->getCoordinate());
					}
				}
				geos::geom::Coordinate c;
				e.centre(c);
				_isoBarycentre.reset(CoordinatesSystem::GetInstanceCoordinatesSystem().getGeometryFactory().createPoint(c));
			}
			return _isoBarycentre;
		}



		template<class Included>
		bool IncludingPlace<Included>::includes( const Included& place ) const
		{
			// A place always includes itself
			if(	dynamic_cast<const Included*>(this) &&
				&place == dynamic_cast<const Included*>(this)
			){
				return true;
			}

			// Tests if the place is present in the included places list
			BOOST_FOREACH(const Included* testedPlace, _includedPlaces)
			{
				if (testedPlace == &place)
				{
					return true;
			}	}

			return false;
		}



		template<class Included>
		void IncludingPlace<Included>::addIncludedPlace( const Included& place )
		{
			_isoBarycentre.reset();
			_includedPlaces.insert(&place);
		}



		template<class Included>
		void synthese::geography::IncludingPlace<Included>::removeIncludedPlace( const Included& place )
		{
			_isoBarycentre.reset();
			_includedPlaces.erase(&place);
		}



		template<class Included>
		void synthese::geography::IncludingPlace<Included>::clearIncludedPlaces()
		{
			_isoBarycentre.reset();
			_includedPlaces.clear();
		}
}	}

#endif
