
/** ConnectionPlace class header.
	@file ConnectionPlace.h

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

#ifndef SYNTHESE_ENV_CONNECTIONPLACE_H
#define SYNTHESE_ENV_CONNECTIONPLACE_H


#include <map>
#include <set>
#include <string>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "15_env/AddressablePlace.h"
#include "15_env/Types.h"


namespace synthese
{
	namespace time
	{
		class DateTime;
	}

	namespace env
	{
		class Address;
		class Edge;
		class Path;
		class SquareDistance; 


		/** A connection place indicates that there are possible
			connections between different network vertices.

			Each connection is associated with a type (authorized, 
			forbidden, recommended...) and a transfer delay.

			@ingroup m15
		*/
		class ConnectionPlace : 
			public synthese::util::Registrable<uid,ConnectionPlace>, 
			public AddressablePlace
		{
		public:

			static const int UNKNOWN_TRANSFER_DELAY;
			static const int FORBIDDEN_TRANSFER_DELAY;
			static const int SQUAREDISTANCE_SHORT_LONG;

		private:

			PhysicalStopsSet _physicalStops; 

			std::map< std::pair<uid, uid>, int > _transferDelays; //!< Transfer delays between vertices
			int _defaultTransferDelay;
			int _minTransferDelay;
			int _maxTransferDelay;

			ConnectionType _connectionType;

		protected:


		public:

			ConnectionPlace (const uid& id,
					const std::string& name,
					const City* city,
					const ConnectionType& connectionType,
					int defaultTransferDelay = FORBIDDEN_TRANSFER_DELAY);

			~ConnectionPlace ();


			//! @name Getters/Setters
			//@{
				int							getDefaultTransferDelay () const;
				void						setDefaultTransferDelay (int defaultTransferDelay);

				int							getMinTransferDelay () const;
				int							getMaxTransferDelay () const;

				const PhysicalStopsSet&		getPhysicalStops () const;

				const ConnectionType		getConnectionType () const;
				void						setConnectionType (const ConnectionType& connectionType);

			//@}


			//! @name Query methods.
			//@{
				bool isConnectionAllowed (const Vertex* fromVertex, 
							const Vertex* toVertex) const;

				ConnectionType getRecommendedConnectionType (const SquareDistance& squareDistance) const;


				int getTransferDelay (const Vertex* fromVertex, 
						const Vertex* toVertex) const;


				VertexAccess getVertexAccess (const AccessDirection& accessDirection,
							const AccessParameters& accessParameters,
							const Vertex* destination,
							const Vertex* origin = 0) const;
			    
				void getImmediateVertices (VertexAccessMap& result, 
							const AccessDirection& accessDirection,
							const AccessParameters& accessParameters,
							const Vertex* origin = 0,
							bool returnAddresses = true,
							bool returnPhysicalStops = true) const;

				std::vector<std::pair<uid, std::string> >	getPhysicalStopLabels(bool withAll = false) const;
				/** Labels list for select field containing physical stops, with exclusion list.
					@param noDisplay Physical stops to exclude
					@return Labels list for select field containing physical stops
					@author Hugues Romain
					@date 2007
				*/
				std::vector<std::pair<uid, std::string> >	getPhysicalStopLabels(const PhysicalStopsSet& noDisplay) const;
			//@}


			//! @name Update methods.
			//@{
				void addPhysicalStop (const PhysicalStop* physicalStop);
				void addTransferDelay (uid departureId, uid arrivalId, int transferDelay);
				void clearTransferDelays ();
			//@}

		};
	}
}

#endif 	    
