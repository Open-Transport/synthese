
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

#include "15_env/ConnectionPlace.h"

#include "06_geometry/IsoBarycentre.h"

#include "01_util/Registrable.h"
#include "01_util/UId.h"

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
			: public util::Registrable<uid,PublicTransportStopZoneConnectionPlace>
			, public ConnectionPlace
		{
		private:
			typedef std::map< std::pair<uid, uid>, int > TransferDelaysMap;

			//! @name Data
			//@{
				PhysicalStops		_physicalStops; 
				TransferDelaysMap	_transferDelays; //!< Transfer delays between vertices (in minutes)
				int					_defaultTransferDelay;
			//@}
			
			//! @name Caching
			//@{
				mutable int _score;
				mutable int _minTransferDelay;
			//@}
			

		public:
			typedef std::vector<std::pair<uid, std::string> > PhysicalStopsLabels;

			PublicTransportStopZoneConnectionPlace(
				uid id = UNKNOWN_VALUE
				, std::string name = std::string()
				, const City* city = NULL
				, ConnectionType connectionType = CONNECTION_TYPE_FORBIDDEN
				, int defaultTransferDelay = FORBIDDEN_TRANSFER_DELAY
			);

			//! @name Getters
			//@{
				const PhysicalStops&			getPhysicalStops() const;
				int								getDefaultTransferDelay() const;
				virtual int						getMinTransferDelay() const;
				virtual uid						getId() const;
			//@}

			//! @name Setters
			//@{
				void	setDefaultTransferDelay (int defaultTransferDelay);
				void	setConnectionType (const ConnectionType& connectionType);
			//@}


			//! @name Update methods.
			//@{
				void addPhysicalStop (const PhysicalStop* physicalStop);
				void addTransferDelay (uid departureId, uid arrivalId, int transferDelay);
				void clearTransferDelays ();
			//@}


			//! @name Virtual queries
			//@{
				virtual bool isConnectionAllowed(
					const Vertex* fromVertex
					, const Vertex* toVertex
				) const;

				virtual int getTransferDelay(
					const Vertex* fromVertex
					, const Vertex* toVertex
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
				virtual int getScore() const;

				virtual const geometry::Point2D& getPoint() const;

				virtual void getImmediateVertices(
					VertexAccessMap& result
					, const AccessDirection& accessDirection
					, const AccessParameters& accessParameters
					, SearchAddresses returnAddresses
					, SearchPhysicalStops returnPhysicalStops
					, const Vertex* origin = NULL
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
