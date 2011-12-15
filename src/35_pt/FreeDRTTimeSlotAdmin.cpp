
//////////////////////////////////////////////////////////////////////////
/// FreeDRTTimeSlotAdmin class implementation.
///	@file FreeDRTTimeSlotAdmin.cpp
///	@author Hugues Romain
///	@date 2011
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

#include "FreeDRTTimeSlotAdmin.hpp"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "StaticActionRequest.h"
#include "AdminActionFunctionRequest.hpp"
#include "AjaxVectorFieldEditor.hpp"
#include "RequestException.h"
#include "FreeDRTTimeSlot.hpp"
#include "PropertiesHTMLTable.h"
#include "FreeDRTTimeSlotUpdateAction.hpp"
#include "CityListFunction.h"
#include "PTRuleUserAdmin.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, FreeDRTTimeSlotAdmin>::FACTORY_KEY("FreeDRTTimeSlotAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<FreeDRTTimeSlotAdmin>::ICON("car.png");
		template<> const string AdminInterfaceElementTemplate<FreeDRTTimeSlotAdmin>::DEFAULT_TITLE("Service");
	}

	namespace pt
	{
		FreeDRTTimeSlotAdmin::FreeDRTTimeSlotAdmin()
			: AdminInterfaceElementTemplate<FreeDRTTimeSlotAdmin>()
		{ }


		
		void FreeDRTTimeSlotAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_timeSlot = Env::GetOfficialEnv().get<FreeDRTTimeSlot>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch (ObjectNotFoundException<FreeDRTTimeSlot>&)
			{
				throw RequestException("No such service");
			}
		}



		ParametersMap FreeDRTTimeSlotAdmin::getParametersMap() const
		{
			ParametersMap m;

			if(_timeSlot.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _timeSlot->getKey());
			}

			return m;
		}


		
		bool FreeDRTTimeSlotAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void FreeDRTTimeSlotAdmin::display(
			ostream& stream,
			const admin::AdminRequest& request
		) const	{

			stream << "<h1>Propriétés</h1>";

			AdminActionFunctionRequest<FreeDRTTimeSlotUpdateAction, FreeDRTTimeSlotAdmin> updateRequest(request);
			updateRequest.getAction()->setTimeSlot(const_pointer_cast<FreeDRTTimeSlot>(_timeSlot));

			// Properties editor
			PropertiesHTMLTable propertiesTable(
				updateRequest.getHTMLForm("properties")
			);
			stream << propertiesTable.open();
			stream << propertiesTable.cell("ID", lexical_cast<string>(_timeSlot->getKey()));
			stream << propertiesTable.cell("Numéro service", propertiesTable.getForm().getTextInput(FreeDRTTimeSlotUpdateAction::PARAMETER_SERVICE_NUMBER, _timeSlot->getServiceNumber()));
			stream << propertiesTable.cell("Heure début", propertiesTable.getForm().getTextInput(FreeDRTTimeSlotUpdateAction::PARAMETER_FIRST_DEPARTURE, to_simple_string(_timeSlot->getFirstDeparture())));
			stream << propertiesTable.cell("Heure fin", propertiesTable.getForm().getTextInput(FreeDRTTimeSlotUpdateAction::PARAMETER_LAST_ARRIVAL, to_simple_string(_timeSlot->getLastArrival())));
			stream << propertiesTable.cell("Capacité max", propertiesTable.getForm().getTextInput(FreeDRTTimeSlotUpdateAction::PARAMETER_MAX_CAPACITY, _timeSlot->getMaxCapacity() ? lexical_cast<string>(*_timeSlot->getMaxCapacity()) : string()));
			stream << propertiesTable.cell("Vitesse commerciale", propertiesTable.getForm().getTextInput(FreeDRTTimeSlotUpdateAction::PARAMETER_COMMERCIAL_SPEED, lexical_cast<string>(_timeSlot->getCommercialSpeed())));
			stream << propertiesTable.cell("Vitesse maximale", propertiesTable.getForm().getTextInput(FreeDRTTimeSlotUpdateAction::PARAMETER_MAX_SPEED, lexical_cast<string>(_timeSlot->getMaxSpeed())));
			stream << propertiesTable.close();

			// Use rule editor
			stream << "<h1>Conditions d'accès</h1>";
			PTRuleUserAdmin<FreeDRTTimeSlot, FreeDRTTimeSlotAdmin>::Display(stream, _timeSlot, request);
		}


		
		std::string FreeDRTTimeSlotAdmin::getTitle() const
		{
			return (_timeSlot.get() && _timeSlot->getServiceNumber().empty()) ? _timeSlot->getServiceNumber() : DEFAULT_TITLE;
		}



		bool FreeDRTTimeSlotAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _timeSlot == static_cast<const FreeDRTTimeSlotAdmin&>(other)._timeSlot;
		}
}	}
