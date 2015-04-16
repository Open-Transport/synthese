
/** ResaModule class header.
	@file ResaModule.h

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

#ifndef SYNTHESE_ResaModule_h__
#define SYNTHESE_ResaModule_h__

#include "ResaTypes.h"

#include "ModuleClassTemplate.hpp"
#include "Registry.h"

#include <map>
#include <ostream>

#include <boost/thread/recursive_mutex.hpp>

namespace synthese
{
	class Language;

	namespace pt_website
	{
		class PTServiceConfig;
	}

	namespace pt
	{
		class ReservationContact;
	}

	namespace util
	{
		class Env;
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

	namespace graph
	{
		class Service;
	}

	/**	@defgroup m51Actions 51 Actions
		@ingroup m51

		@defgroup m51Pages 51 Pages
		@ingroup m51

		@defgroup m51Functions 51 Functions
		@ingroup m51

		@defgroup m51Exceptions 51 Exceptions
		@ingroup m51

		@defgroup m51Alarm 51 Messages recipient
		@ingroup m51

		@defgroup m51LS 51 Table synchronizers
		@ingroup m51

		@defgroup m51Admin 51 Administration pages
		@ingroup m51

		@defgroup m51Rights 51 Rights
		@ingroup m51

		@defgroup m51Logs 51 DB Logs
		@ingroup m51

		@defgroup m51 51 Reservation
		@ingroup m5

		The reservation module provides the ability to book seats on demand responsive transport lines.

		The features of the reservation module are :
			- BoolingScreenFunction : booking screen, available directly in a route planner journey roadmap
			- BoolingConfirmationFunction : booking confirmation

		A logged standard user uses the administration panel to access to the following features :
			- edit personal informations (in security module)
			- edit favorites journeys (in pt_journey_planner module)
			- display reservations history
				- cancel a reservation
			- display an integrated route planner (in pt_journey_planner module)

		A logged operator uses the administration panel to access to the following features :
			- personal informations (security module)
			- search a customer (security module)
				- edit customer personal informations (security module)
				- display customer reservations history
					- cancel a reservation
			- display the commercial lines with reservations
				- display the reservation list of the line
					- ServiceReservationsRoadMapFunction : display the detailed reservation list of a service / course (pop up : not in the admin panel, optimized for printing)
			- display an integrated route planner (pt_journey_planner module)

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
	/// 51 Reservation Module namespace.
	///	@author Hugues Romain
	///	@date 2008
	//////////////////////////////////////////////////////////////////////////
	namespace resa
	{
		class ReservationTransaction;
		class CancelReservationAction;
		class OnlineReservationRule;
		class Reservation;

		/** 51 Reservation module class.
		*/
		class ResaModule:
			public server::ModuleClassTemplate<ResaModule>
		{
			friend class server::ModuleClassTemplate<ResaModule>;

		private:
			typedef std::map<const server::Session*, util::RegistryKeyType> _SessionsCallIdMap;
			static _SessionsCallIdMap _sessionsCallIds;
			static boost::recursive_mutex _sessionsCallIdsMutex;

			static const std::string _BASIC_PROFILE_NAME;
			static const std::string _AUTORESA_PROFILE_NAME;
			static const std::string _ADMIN_PROFILE_NAME;
			static const std::string _RESERVATION_CONTACT_PARAMETER;
			static const std::string _JOURNEY_PLANNER_WEBSITE;

			static const std::string DATA_SWITCH_CALL_URL;
			static const std::string DATA_CURRENT_CALL_ID;
			static const std::string DATA_CURRENT_CALL_TIMESTAMP;

			static const std::string MODULE_PARAMETER_MAX_SEATS_ALLOWED;

			static boost::shared_ptr<security::Profile>	_basicProfile;
			static boost::shared_ptr<security::Profile>	_autoresaProfile;
			static boost::shared_ptr<security::Profile>	_adminProfile;
			static boost::shared_ptr<OnlineReservationRule> _reservationContact;
			static boost::shared_ptr<pt_website::PTServiceConfig> _journeyPlannerConfig;
			static size_t _maxSeats;

		public:
			typedef std::map<const graph::Service*, std::set<const Reservation*> > ReservationsByService;

		private:
			/// Stores the reservations for services with arrival max 1 hour before now and departure to 6 hours after now
			static ReservationsByService _reservationsByService;
			static boost::recursive_mutex _reservationsByServiceMutex;

		public:
			static void AddReservationByService(const Reservation& reservation);
			static void RemoveReservationByService(const Reservation& reservation);



			//////////////////////////////////////////////////////////////////////////
			/// Gets the reservation on a service, present in the main environment.
			/// @param service the service
			/// @return the reservations
			/// @warning The reservations can be deleted from the environment at any time :
			/// lock the corresponding mutex to avoid crash as long as the obtained reservation are
			/// used. Use GetReservationsByServiceMutex to get the corresponding mutex.
			static const ReservationsByService::mapped_type& GetReservationsByService(const graph::Service& service);



			//////////////////////////////////////////////////////////////////////////
			/// Gets the mutex that protects the reservations by service cache.
			/// @return the mutex that protects the reservations by service cache.
			static boost::recursive_mutex& GetReservationsByServiceMutex() { return _reservationsByServiceMutex; }


			static boost::shared_ptr<security::Profile> GetBasicResaCustomerProfile();
			static boost::shared_ptr<security::Profile> GetAutoResaResaCustomerProfile();

			/** Called whenever a parameter registered by this module is changed
			*/
			static void ParameterCallback(
				const std::string& name,
				const std::string& value
			);

			static void DisplayReservations(
				std::ostream& stream,
				const ReservationTransaction& reservation,
				const Language& language
			);

			static void CallOpen(const server::Session* session);
			static void CallClose(const server::Session* session);
			static util::RegistryKeyType GetCurrentCallId(const server::Session* session);

			static std::string GetStatusIcon(ReservationStatus status);
			static std::string GetStatusText(ReservationStatus status);

			static OnlineReservationRule* GetReservationContact();
			static pt_website::PTServiceConfig* GetJourneyPlannerWebsite();
			static size_t GetMaxSeats();

			//////////////////////////////////////////////////////////////////////////
			/// Adds parameters to send to the display template for admin pages.
			/// The default implementation does nothing. Overload the method to
			/// add such a feature.
			/// @param map the map to update
			/// @param request the current admin request
			/// @author Hugues Romain
			/// @date 2011
			/// @since 3.2.0
			virtual void addAdminPageParameters(
				util::ParametersMap& map,
				const server::Request& request
			) const;
		};
	}
	/** @} */
}

#endif // SYNTHESE_ResaModule_h__
