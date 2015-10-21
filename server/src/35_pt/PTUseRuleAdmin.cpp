
//////////////////////////////////////////////////////////////////////////
/// PTUseRuleAdmin class implementation.
///	@file PTUseRuleAdmin.cpp
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

#include "PTUseRuleAdmin.h"

#include "Fare.hpp"
#include "Profile.h"
#include "PTUseRulesAdmin.h"
#include "User.h"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "PropertiesHTMLTable.h"
#include "PTUseRuleTableSync.h"
#include "AdminActionFunctionRequest.hpp"
#include "PTUseRuleUpdateAction.hpp"
#include "FareTableSync.hpp"

#include <boost/optional.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace fare;
	using namespace html;
	using namespace pt;
	using namespace security;
	using namespace server;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, PTUseRuleAdmin>::FACTORY_KEY("PTUseRuleAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTUseRuleAdmin>::ICON("resa_optional.png");
		template<> const string AdminInterfaceElementTemplate<PTUseRuleAdmin>::DEFAULT_TITLE("Condition de transport");
	}

	namespace pt
	{
		PTUseRuleAdmin::PTUseRuleAdmin()
			: AdminInterfaceElementTemplate<PTUseRuleAdmin>()
		{ }



		void PTUseRuleAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_rule = PTUseRuleTableSync::Get(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID), _getEnv());
			}
			catch (ObjectNotFoundException<PTUseRule>&)
			{
				throw AdminParametersException("No such use rule");
			}
		}



		ParametersMap PTUseRuleAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_rule.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _rule->getKey());
			}
			return m;
		}



		bool PTUseRuleAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void PTUseRuleAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{

			AdminActionFunctionRequest<PTUseRuleUpdateAction,PTUseRuleAdmin> updateRequest(_request, *this);
			updateRequest.getAction()->setRule(const_pointer_cast<PTUseRule>(_rule));

			stream << "<h1>Propriétés</h1>";

			PropertiesHTMLTable t(updateRequest.getHTMLForm());
			stream << t.open();
			stream << t.title("Dénomination");
			stream << t.cell("ID", lexical_cast<string>(_rule->getKey()));
			stream << t.cell("Nom", t.getForm().getTextInput(PTUseRuleUpdateAction::PARAMETER_NAME, _rule->getName()));
			stream << t.title("Réservation");
			stream << t.cell(
				"Type de réservation",
				t.getForm().getSelectInput(
					PTUseRuleUpdateAction::PARAMETER_TYPE,
					PTUseRule::GetTypesList(),
					optional<pt::ReservationRuleType>(_rule->getReservationType())
			)	);
			if(_rule->getReservationType() != pt::RESERVATION_RULE_FORBIDDEN)
			{
				stream <<
					t.cell(
						"Heure limite de réservation",
						t.getForm().getTextInput(
							PTUseRuleUpdateAction::PARAMETER_HOUR_DEADLINE,
							_rule->getHourDeadLine().is_not_a_date_time() ? string() : to_simple_string(_rule->getHourDeadLine())
					)	)
				;
				stream <<
					t.cell(
						"Délai maximal en jours",
						t.getForm().getTextInput(
							PTUseRuleUpdateAction::PARAMETER_MAX_DELAY_DAYS,
							_rule->getMaxDelayDays() ? lexical_cast<string>(_rule->getMaxDelayDays()->days()) : string()
					)	)
				;
				stream <<
					t.cell(
						"Délai minimal en jours",
						t.getForm().getTextInput(
							PTUseRuleUpdateAction::PARAMETER_MIN_DELAY_DAYS,
							lexical_cast<string>(_rule->getMinDelayDays().days())
					)	)
				;
				stream <<
					t.cell(
						"Délai minimal en minutes (interne)",
						t.getForm().getTextInput(
							PTUseRuleUpdateAction::PARAMETER_MIN_DELAY_MINUTES,
							lexical_cast<string>(_rule->getMinDelayMinutes().total_seconds() / 60)
					)	)
				;
				stream <<
					t.cell(
						"Délai minimal en minutes (externe)",
						t.getForm().getTextInput(
							PTUseRuleUpdateAction::PARAMETER_MIN_DELAY_MINUTES_EXTERNAL,
							lexical_cast<string>(_rule->getMinDelayMinutesExternal().total_seconds() / 60)
					)	)
				;
				stream <<
					t.cell(
						"Heure au départ est la référence",
						t.getForm().getOuiNonRadioInput(
							PTUseRuleUpdateAction::PARAMETER_ORIGIN_IS_REFERENCE,
							_rule->getOriginIsReference()
					)	)
				;
				stream <<
					t.cell(
						"Heure min de départ pour réservation le jour même",
						t.getForm().getTextInput(
							PTUseRuleUpdateAction::PARAMETER_RESERVATION_MIN_DEPARTURE_TIME,
							_rule->getReservationMinDepartureTime().is_not_a_date_time() ?
								string() :
								to_simple_string(_rule->getReservationMinDepartureTime())
					)	)
				;
				stream <<
					t.cell(
						"Jours de réservation interdits (0=dimanche, 6=samedi)",
						t.getForm().getTextInput(
							PTUseRuleUpdateAction::PARAMETER_RESERVATION_FORBIDDEN_DAYS,
							PTUseRule::SerializeForbiddenDays(_rule->getReservationForbiddenDays())
					)	)
				;
			}
			stream << t.title("Autres propriétés");
			stream << t.cell("Capacité maximale (vide=illimité, 0=interdit)", t.getForm().getTextInput(PTUseRuleUpdateAction::PARAMETER_CAPACITY, _rule->getAccessCapacity() ? lexical_cast<string>(*_rule->getAccessCapacity()) : string()));
			stream << t.cell("Tarification principale", t.getForm().getSelectInput(PTUseRuleUpdateAction::PARAMETER_FARE_ID, FareTableSync::GetList(_getEnv()), optional<RegistryKeyType>(_rule->getDefaultFare() ? _rule->getDefaultFare()->getKey() : 0)));
			stream << t.title("Autres propriétés");
			stream << t.cell("Ignorer dans tableaux de départs", t.getForm().getOuiNonRadioInput(PTUseRuleUpdateAction::PARAMETER_FORBIDDEN_IN_DEPARTURE_BOARDS, _rule->getForbiddenInDepartureBoards()));
			stream << t.cell("Ignorer dans fiches horaires", t.getForm().getOuiNonRadioInput(PTUseRuleUpdateAction::PARAMETER_FORBIDDEN_IN_TIMETABLES, _rule->getForbiddenInTimetables()));
			stream << t.cell("Ignorer dans calcul d'itinéraires", t.getForm().getOuiNonRadioInput(PTUseRuleUpdateAction::PARAMETER_FORBIDDEN_IN_JOURNEY_PLANNER, _rule->getForbiddenInJourneyPlanning()));
			stream << t.close();
		}



		std::string PTUseRuleAdmin::getTitle() const
		{
			return _rule.get() ? _rule->getName() : DEFAULT_TITLE;
		}



		bool PTUseRuleAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _rule->getKey() == static_cast<const PTUseRuleAdmin&>(other)._rule->getKey();
			return true;
		}



		AdminInterfaceElement::PageLinks PTUseRuleAdmin::_getCurrentTreeBranch() const
		{
			PageLinks links;
			links.push_back(getNewPage<PTUseRulesAdmin>());
			return links;
		}
	}
}
