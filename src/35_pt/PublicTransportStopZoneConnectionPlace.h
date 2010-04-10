
/** PublicTransportStopZoneConnectionPlace class header.
	@file PublicTransportStopZoneConnectionPlace.h

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

#ifndef SYNTHESE_env_PublicTransportStopZoneConnectionPlace_h__
#define SYNTHESE_env_PublicTransportStopZoneConnectionPlace_h__

#include "NamedPlaceTemplate.h"
#include "AddressablePlace.h"
#include "Registry.h"
#include "IsoBarycentre.h"
#include "Importable.h"

#include <map>
#include <utility>
#include <boost/optional.hpp>

namespace synthese
{
	namespace road
	{
		class Address;
	}

	namespace graph
	{
		class Vertex;
	}

	namespace pt
	{
		class PhysicalStop;
	}

	namespace pt
	{		
		/** PublicTransportStopZoneConnectionPlace class.
			@ingroup m35
		*/
		class PublicTransportStopZoneConnectionPlace:
			public road::AddressablePlace,
			public geography::NamedPlaceTemplate<PublicTransportStopZoneConnectionPlace>,
			public impex::Importable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<PublicTransportStopZoneConnectionPlace>	Registry;

			typedef std::map<util::RegistryKeyType,const pt::PhysicalStop*> PhysicalStops;
			typedef std::vector<std::pair<boost::optional<util::RegistryKeyType>, std::string> > PhysicalStopsLabels;

		private:

			//! @name Content
			//@{
				PhysicalStops			_physicalStops; 
			//@}

			//! @name Caching
			//@{
				mutable int _score;
			//@}

			

		public:

			PublicTransportStopZoneConnectionPlace(
				util::RegistryKeyType id = 0
				, bool allowedConnection = false
				, boost::posix_time::time_duration defaultTransferDelay = boost::posix_time::time_duration()
			);

			//! @name Getters
			//@{
				const PhysicalStops&	getPhysicalStops() const;
			//@}

			//! @name Update methods.
			//@{
				void addPhysicalStop(const pt::PhysicalStop& physicalStop);
			//@}


			//! @name Virtual queries
			//@{
				/** Score getter.
					@return int the score of the place
					@author Hugues Romain
					@date 2007

					The vertex score is calculated by the following way :
						- each commercial line gives some points, depending of the number of services which belongs to the line :
							- 1 to 10 services lines gives 2 point
							- 10 to 50 services lines gives 3 points
							- 50 to 100 services lines gives 4 points
							- much than 100 services lines gives 5 points
						- if the score is bigger than 100 points, then the score is 100
				*/
				virtual graph::HubScore getScore() const;

				virtual const geometry::Point2D& getPoint() const;

				virtual bool isConnectionPossible() const;

				virtual std::string getNameForAllPlacesMatcher(
					std::string text = std::string()
				) const;

				virtual void getVertexAccessMap(
					graph::VertexAccessMap& result
					, const graph::AccessDirection& accessDirection
					, graph::GraphIdType whatToSearch,
					const graph::Vertex& origin
				) const;


				virtual void getVertexAccessMap(
					graph::VertexAccessMap& result
					, const graph::AccessDirection& accessDirection
					, const graph::AccessParameters& accessParameters
					, const geography::Place::GraphTypes& whatToSearch
				) const;

				virtual bool containsAnyVertex(graph::GraphIdType graphType) const;
			//@}

			//! @name Queries
			//@{
				/** getPhysicalStopLabels.
					@param withAll
					@return Physical stops labels
					@author Hugues Romain
					@date 2007					
				*/
				PhysicalStopsLabels	getPhysicalStopLabels(bool withAll = false) const;

				/** Labels list for select field containing physical stops, with exclusion list.
					@param noDisplay Physical stops to exclude
					@return Labels list for select field containing physical stops
					@author Hugues Romain
					@date 2007
				*/
				PhysicalStopsLabels	getPhysicalStopLabels(const PhysicalStops& noDisplay) const;
			//@}

		};
	}
}

#endif // SYNTHESE_env_PublicTransportStopZoneConnectionPlace_h__
