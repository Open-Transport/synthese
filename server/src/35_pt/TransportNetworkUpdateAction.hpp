
//////////////////////////////////////////////////////////////////////////
/// TransportNetworkUpdateAction class header.
///	@file TransportNetworkUpdateAction.hpp
///	@author Hugues Romain
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_TransportNetworkUpdateAction_H__
#define SYNTHESE_TransportNetworkUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "BaseImportableUpdateAction.hpp"

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace pt
	{
		class TransportNetwork;
		class ReservationContact;

		//////////////////////////////////////////////////////////////////////////
		/// TransportNetworkUpdateAction action class.
		/// @ingroup m53Actions refActions
		class TransportNetworkUpdateAction:
			public util::FactorableTemplate<server::Action, TransportNetworkUpdateAction>,
			public impex::BaseImportableUpdateAction
		{
		public:
			static const std::string PARAMETER_NETWORK_ID;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_DAYS_CALENDARS_PARENT_ID;
			static const std::string PARAMETER_PERIODS_CALENDARS_PARENT_ID;
			static const std::string PARAMETER_IMAGE;
			static const std::string PARAMETER_TIMEZONE;
			static const std::string PARAMETER_LANG;
			static const std::string PARAMETER_CONTACT_ID;
			static const std::string PARAMETER_FARE_CONTACT_ID;
			static const std::string PARAMETER_COUNTRY_CODE;

		private:
			boost::shared_ptr<TransportNetwork> _network;
			boost::optional<std::string> _name;
			boost::optional<boost::shared_ptr<calendar::CalendarTemplate> > _daysCalendarsParent;
			boost::optional<boost::shared_ptr<calendar::CalendarTemplate> > _periodsCalendarsParent;
			boost::optional<std::string> _image;
			boost::optional<std::string> _timeZone;
			boost::optional<std::string> _lang;
			boost::optional<boost::shared_ptr<pt::ReservationContact> > _contact;
			boost::optional<boost::shared_ptr<pt::ReservationContact> > _fareContact;
			boost::optional<std::string> _countryCode;


		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @return The generated parameters map
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);


		public:
			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);



			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;

			void setNetwork(boost::shared_ptr<TransportNetwork> value){ _network = value; }
		};
}	}

#endif // SYNTHESE_TransportNetworkAddAction_H__
