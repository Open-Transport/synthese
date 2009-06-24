
/** ResaModule class header.
	@file ResaModule.h

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

#ifndef SYNTHESE_ResaModule_h__
#define SYNTHESE_ResaModule_h__

#include "31_resa/Types.h"

#include "ModuleClassTemplate.hpp"
#include "Registry.h"

#include <map>
#include <ostream>

namespace synthese
{
	namespace util
	{
		class Env;
	}

	namespace admin
	{
		class AdminRequest;
	}

	namespace dblog
	{
		class DBLogEntry;
	}

	namespace security
	{
		class Profile;
	}

	namespace html
	{
		class HTMLTable;
	}

	namespace server
	{
		class Session;
	}

	/**	@defgroup m31Actions 31 Actions
		@ingroup m31

		@defgroup m31Pages 31 Pages
		@ingroup m31

		@defgroup m31Functions 31 Functions
		@ingroup m31

		@defgroup m31Exceptions 31 Exceptions
		@ingroup m31

		@defgroup m31Alarm 31 Messages recipient
		@ingroup m31

		@defgroup m31LS 31 Table synchronizers
		@ingroup m31

		@defgroup m31Admin 31 Administration pages
		@ingroup m31

		@defgroup m31Rights 31 Rights
		@ingroup m31

		@defgroup m31Logs 31 DB Logs
		@ingroup m31

		@defgroup m31 31 Reservation
		@ingroup m3

		The reservation module provides the ability to book seats on transport on demand lines.

		The features of the reservation module are :
			- BoolingScreenFunction : booking screen, available directly in a route planner journey roadmap
			- BoolingConfirmationFunction : booking confirmation
			
		A logged standard user uses the administration panel to access to the following features :
			- edit personal informations (in security module)
			- edit favorites journeys (in routeplanner module)
			- display reservations history
				- cancel a reservation
			- display an integrated route planner (in routeplanner module)

		A logged operator uses the administration panel to access to the following features :
			- personal informations (security module)
			- search a customer (security module)
				- edit customer personal informations (security module)
				- display customer reservations history
					- cancel a reservation
			- display the commercial lines with reservations
				- display the reservation list of the line
					- ServiceReservationsRoadMapFunction : display the detailed reservation list of a service / course (pop up : not in the admin panel, optimized for printing)
			- display an integrated route planner (routeplanner module)
			
			Move the following features in a call center module :
			- display the call center planning
			- display the calls list
				- display a call log

		A logged driver uses the administration panel to access to the following features :
			- display the commercial lines
				- display the reservation list of the line
					- display the detailed reservation list of a service / course

	@{
	*/

	//////////////////////////////////////////////////////////////////////////
	/// 31 Reservation Module namespace.
	///	@author Hugues Romain
	///	@date 2008
	//////////////////////////////////////////////////////////////////////////
	namespace resa
	{
		class ReservationTransaction;
		class CancelReservationAction;

		/** 31 Reservation module class.
		*/
		class ResaModule:
			public server::ModuleClassTemplate<ResaModule>
		{
			friend class server::ModuleClassTemplate<ResaModule>;
			
		private:
			typedef std::map<const server::Session*, util::RegistryKeyType> _SessionsCallIdMap;
			static _SessionsCallIdMap _sessionsCallIds;

			static const std::string _BASIC_PROFILE_NAME;
			static const std::string _AUTORESA_PROFILE_NAME;
			static const std::string _ADMIN_PROFILE_NAME;

			static boost::shared_ptr<security::Profile>	_basicProfile;
			static boost::shared_ptr<security::Profile>	_autoresaProfile;
			static boost::shared_ptr<security::Profile>	_adminProfile;

		public:
			static boost::shared_ptr<security::Profile> GetBasicResaCustomerProfile();
			static boost::shared_ptr<security::Profile> GetAutoResaResaCustomerProfile();

			static void DisplayReservations(
				std::ostream& stream,
				const ReservationTransaction& reservation
			);

			static void CallOpen(const server::Session* session);
			static void CallClose(const server::Session* session);
			static util::RegistryKeyType GetCurrentCallId(const server::Session* session);

			static std::string GetStatusIcon(ReservationStatus status);
			static std::string GetStatusText(ReservationStatus status);
		};
	}
	/** @} */
}

#endif // SYNTHESE_ResaModule_h__
