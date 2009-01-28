
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

#include "AddressablePlace.h"
#include "Registry.h"
#include "IsoBarycentre.h"

#include <map>
#include <utility>

namespace synthese
{
	namespace time
	{
		class DateTime;
	}

	namespace geometry
	{
		class SquareDistance; 
	}

	namespace env
	{
		class Address;

		/** PublicTransportStopZoneConnectionPlace class.
			@ingroup m35
		*/
		class PublicTransportStopZoneConnectionPlace
		:	public AddressablePlace
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<PublicTransportStopZoneConnectionPlace>	Registry;

		private:
			bool		_allowedConnection;

			typedef std::map< std::pair<uid, uid>, graph::MinutesDuration> TransferDelaysMap;

			//! @name Data
			//@{
				PhysicalStops			_physicalStops; 
				TransferDelaysMap		_transferDelays; //!< Transfer delays between vertices (in minutes)
				graph::MinutesDuration	_defaultTransferDelay;
			//@}
			
			//! @name Caching
			//@{
				mutable int _score;
				mutable int _minTransferDelay;
			//@}
			

		public:
			typedef std::vector<std::pair<uid, std::string> > PhysicalStopsLabels;

			PublicTransportStopZoneConnectionPlace(
				util::RegistryKeyType id = UNKNOWN_VALUE
				, std::string name = std::string()
				, const City* city = NULL
				, bool allowedConnection = false
				, graph::MinutesDuration defaultTransferDelay = FORBIDDEN_TRANSFER_DELAY
			);

			//! @name Getters
			//@{
				const PhysicalStops&	getPhysicalStops() const;
				graph::MinutesDuration			getDefaultTransferDelay() const;
				virtual graph::MinutesDuration	getMinTransferDelay() const;
			//@}

			//! @name Setters
			//@{
				void	setDefaultTransferDelay(
					graph::MinutesDuration defaultTransferDelay
				);
				void	setAllowedConnection(bool value);
			//@}


			//! @name Update methods.
			//@{
				void addPhysicalStop (const PhysicalStop* physicalStop);
				void addTransferDelay(uid departureId, uid arrivalId, graph::MinutesDuration transferDelay);
				void clearTransferDelays ();
			//@}


			//! @name Virtual queries
			//@{
				virtual bool isConnectionAllowed(
					const graph::Vertex* fromVertex
					, const graph::Vertex* toVertex
				) const;

				virtual graph::MinutesDuration getTransferDelay(
					const graph::Vertex* fromVertex
					, const graph::Vertex* toVertex
				) const;

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

				virtual void getImmediateVertices(
					graph::VertexAccessMap& result
					, const graph::AccessDirection& accessDirection
					, const AccessParameters& accessParameters
					, SearchAddresses returnAddresses
					, SearchPhysicalStops returnPhysicalStops
					, const graph::Vertex* origin = NULL
				) const;

				graph::VertexAccess getVertexAccess(
					const graph::AccessDirection& accessDirection,
					const AccessParameters& accessParameters,
					const graph::Vertex* destination,
					const graph::Vertex* origin
				) const;

				virtual bool hasPhysicalStops()	const;
			//@}

			//! @name Queries
			//@{
				/** getPhysicalStopLabels.
					@param withAll
					@return std::vector<std::pair<uid, std::string> >
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
