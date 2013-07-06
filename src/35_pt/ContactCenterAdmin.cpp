
//////////////////////////////////////////////////////////////////////////
/// ContactCenterAdmin class implementation.
///	@file ContactCenterAdmin.cpp
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

#include "ContactCenterAdmin.hpp"

#include "AdminParametersException.h"
#include "ObjectUpdateAction.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "ReservationContact.h"
#include "PropertiesHTMLTable.h"
#include "AdminActionFunctionRequest.hpp"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace db;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace html;


	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ContactCenterAdmin>::FACTORY_KEY = "ContactCenter";
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ContactCenterAdmin>::ICON = "help.png";
		template<> const string AdminInterfaceElementTemplate<ContactCenterAdmin>::DEFAULT_TITLE = "Centre de contact";
	}

	namespace pt
	{
		ContactCenterAdmin::ContactCenterAdmin():
			AdminInterfaceElementTemplate<ContactCenterAdmin>()
		{}



		void ContactCenterAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_contactCenter = Env::GetOfficialEnv().get<ReservationContact>(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
				);
			}
			catch(ObjectNotFoundException<ReservationContact>&)
			{
				throw AdminParametersException("No such contact center");
			}
		}



		ParametersMap ContactCenterAdmin::getParametersMap() const
		{
			ParametersMap m;

			if(_contactCenter.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _contactCenter->getKey());
			}

			return m;
		}



		bool ContactCenterAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void ContactCenterAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			//////////////////////////////////////////////////////////////////////////
			// Properties update form
			stream << "<h1>Propriétés</h1>";
			AdminActionFunctionRequest<ObjectUpdateAction, ContactCenterAdmin> updateRequest(request, *this);
			updateRequest.getAction()->setObject(*_contactCenter);
			PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
			stream << t.open();

			// ID
			stream << t.cell(
				"ID",
				lexical_cast<string>(_contactCenter->getKey())
			);

			// Name
			stream << t.cell(
				"Nom",
				t.getForm().GetTextInput(
					ObjectUpdateAction::GetInputName<Name>(),
					_contactCenter->get<Name>()
			)	);

			// Phone number
			stream << t.cell(
				"Numéro de téléphone",
				t.getForm().GetTextInput(
					ObjectUpdateAction::GetInputName<PhoneExchangeNumber>(),
					_contactCenter->get<PhoneExchangeNumber>()
			)	);

			// Phone opening hours
			stream << t.cell(
				"Heures d'ouverture",
				t.getForm().GetTextInput(
					ObjectUpdateAction::GetInputName<PhoneExchangeOpeningHours>(),
					_contactCenter->get<PhoneExchangeOpeningHours>()
			)	);

			// Website
			stream << t.cell(
				"URL site de réservation",
				t.getForm().GetTextInput(
					ObjectUpdateAction::GetInputName<WebsiteURL>(),
					_contactCenter->get<WebsiteURL>()
			)	);

			// Description
			stream << t.cell(
				"Description",
				t.getForm().GetTextInput(
					ObjectUpdateAction::GetInputName<Description>(),
					_contactCenter->get<Description>()
			)	);

			// Closing
			stream << t.close();
		}



		std::string ContactCenterAdmin::getTitle() const
		{
			return _contactCenter.get() ? _contactCenter->getName() : DEFAULT_TITLE;
		}



		bool ContactCenterAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _contactCenter->getKey() == static_cast<const ContactCenterAdmin&>(other)._contactCenter->getKey();
		}
}	}
