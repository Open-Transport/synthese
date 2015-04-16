
/** FareTicket class header.
	@file FareTicket.hpp

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

#ifndef SYNTHESE_ENV_FARETICKET_H
#define SYNTHESE_ENV_FARETICKET_H

#include "FareTicket.hpp"
#include "Fare.hpp"
#include "ServicePointer.h"

#include <string>
#include <boost/date_time/posix_time/ptime.hpp>

namespace synthese
{
	namespace fare
	{
		/** FareTicket handling class
			@ingroup m30
		*/
		class FareTicket
		{
		private:
			const Fare* _fare; //!< Fare of the ticket
			double _price; //!< Total price
			boost::posix_time::ptime _startTime; //!< start time with this ticket
			boost::posix_time::ptime _endTime; //!< end time with this ticket
			int _numberOfConnections; //!< number of connections made
			const graph::ServicePointer* _firstService; //!< first service used with this ticket
			const graph::ServicePointer* _lastService; //!< last service used with this ticket
			bool _available; //!< if this ticket is available or not

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor
			///	@param firstService the first service used with the ticket
			///	@param fare the fare of the ticket
			/// @author Gaël Sauvanet
			/// @date 2011
			FareTicket(const graph::ServicePointer* firstService, const Fare* fare);



			//////////////////////////////////////////////////////////////////////////
			/// Test if the ticket is valid for a specific service and according to the last service used
			///	@param newService the new service
			///	@param lasService the last service used
			/// @author Gaël Sauvanet
			/// @date 2011
			bool isAvailable(const graph::ServicePointer* newService, const graph::ServicePointer* lastService);



			//////////////////////////////////////////////////////////////////////////
			/// Update the ticket properties based on the service used
			///	@param newService the new service
			///	@param lasService the last service used
			/// @author Gaël Sauvanet
			/// @date 2011
			void useService(const graph::ServicePointer* service);



			//! @name Getters
			//@{
				const Fare* getFare() const { return _fare; }
				double getPrice() const { return _price; }
				const boost::posix_time::ptime& getStartTime() const { return _startTime; }
				const boost::posix_time::ptime& getEndTime() const { return _endTime; }
				int getNumberOfConnections() const { return _numberOfConnections; }
				const graph::ServicePointer* getFirstService() const { return _firstService; }
				const graph::ServicePointer* getLastService() const { return _lastService; }
			//@}



			//! @name Setters
			//@{
				void setFare(const Fare* fare) { _fare = fare; }
			//@}
		};
	}
}

#endif
