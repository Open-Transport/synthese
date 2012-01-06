
//////////////////////////////////////////////////////////////////////////
/// ContactCenterUpdateAction class header.
///	@file ContactCenterUpdateAction.hpp
///	@author Hugues Romain
///	@date 2012
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

#ifndef SYNTHESE_ContactCenterUpdateAction_H__
#define SYNTHESE_ContactCenterUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace pt
	{
		class ReservationContact;

		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : ContactCenterUpdateAction.
		/// @ingroup m35Actions refActions
		///	@author Hugues Romain
		///	@date 2012
		/// @since 3.3.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : ContactCenterUpdateAction
		///
		/// Parameters :
		///	<dl>
		///	<dt>actionParamid</dt><dd>id of the object to update</dd>
		///	</dl>
		class ContactCenterUpdateAction:
			public util::FactorableTemplate<server::Action, ContactCenterUpdateAction>
		{
		public:
			static const std::string PARAMETER_CONTACT_CENTER_ID;
			static const std::string PARAMETER_DESCRIPTION;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_PHONE_NUMBER;
			static const std::string PARAMETER_PHONE_OPENING_HOURS;
			static const std::string PARAMETER_WEBSITE_URL;
			
		private:
			boost::shared_ptr<ReservationContact> _contactCenter;
			
			boost::optional<std::string> _name;
			boost::optional<std::string> _phoneNumber;
			boost::optional<std::string> _phoneOpeningHours;
			boost::optional<std::string> _webSiteURL;
			boost::optional<std::string> _description;

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



			//! @name Setters
			//@{
				void setContactCenter(boost::shared_ptr<ReservationContact> value) { _contactCenter = value; }
			//@}
		};
}	}

#endif // SYNTHESE_ContactCenterUpdateAction_H__
