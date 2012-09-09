////////////////////////////////////////////////////////////////////////////////
/// DisplayTypeAdmin class implementation.
///	@file DisplayTypeAdmin.cpp
///	@author Hugues Romain
///	@date 2008-12-19 17:58
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "DisplayTypeAdmin.h"

#include "DisplayTypesAdmin.h"
#include "DeparturesTableModule.h"
#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "DisplayType.h"
#include "DisplayTypeTableSync.h"
#include "Profile.h"
#include "PropertiesHTMLTable.h"
#include "User.h"
#include "UpdateDisplayTypeAction.h"
#include "AdminInterfaceElement.h"
#include "Interface.h"
#include "ArrivalDepartureTableRight.h"
#include "DeparturesTableInterfacePage.h"
#include "ParseDisplayReturnInterfacePage.h"
#include "InterfaceTableSync.h"
#include "Webpage.h"
#include "WebPageTableSync.h"
#include "RemoveObjectAction.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace departure_boards;
	using namespace html;
	using namespace security;
	using namespace cms;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, DisplayTypeAdmin>::FACTORY_KEY("DisplayTypeAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DisplayTypeAdmin>::ICON("monitor.png");
		template<> const string AdminInterfaceElementTemplate<DisplayTypeAdmin>::DEFAULT_TITLE("Type d'affichage");
	}

	namespace departure_boards
	{
		DisplayTypeAdmin::DisplayTypeAdmin()
			: AdminInterfaceElementTemplate<DisplayTypeAdmin>()
		{}



		void DisplayTypeAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_type = DisplayTypeTableSync::GetEditable(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
					_getEnv(),
					UP_LINKS_LOAD_LEVEL
				);
			}
			catch (ObjectNotFoundException<DisplayType>& e)
			{
				throw AdminParametersException("Display type not found by "+ FACTORY_KEY +" : "+ e.getMessage());
			}
		}



		ParametersMap DisplayTypeAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_type.get()) m.insert(Request::PARAMETER_OBJECT_ID, _type->getKey());
			return m;
		}



		void DisplayTypeAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{
			// Requests
			AdminActionFunctionRequest<UpdateDisplayTypeAction,DisplayTypeAdmin> updateRequest(_request, *this);
			updateRequest.getAction()->setTypeId(_type->getKey());

			AdminActionFunctionRequest<RemoveObjectAction,DisplayTypesAdmin> deleteRequest(_request);
			deleteRequest.getAction()->setObjectId(_type->getKey());

			// Display
			PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
			stream << "<h1>Modification du type d'affichage</h1>";
			stream << t.open();
			stream << t.title("Propriétés");
			stream << t.cell("Nom", t.getForm().getTextInput(UpdateDisplayTypeAction::PARAMETER_NAME, _type->getName()));
			stream << t.title("Paramètres d'affichage");
			stream <<
				t.cell(
					"Interface d'affichage",
					t.getForm().getSelectInput(
						UpdateDisplayTypeAction::PARAMETER_INTERFACE_ID,
						InterfaceTableSync::GetInterfaceLabels<DeparturesTableInterfacePage>(optional<string>()),
						optional<RegistryKeyType>(_type->getDisplayInterface() ? _type->getDisplayInterface()->getKey() : 0)
				)	)
			;
			stream <<
				t.cell(
					"Page CMS principale",
					t.getForm().getTextInputAutoCompleteFromTableSync(
						UpdateDisplayTypeAction::PARAMETER_DISPLAY_MAIN_PAGE_ID,
						_type->getDisplayMainPage() ? lexical_cast<string>(_type->getDisplayMainPage()->getKey()) : string(),
						_type->getDisplayMainPage() ? lexical_cast<string>(_type->getDisplayMainPage()->getName()) : string(),
						lexical_cast<string>(WebPageTableSync::TABLE.ID),
						string(),string(),
						true, true, true, true
				)	)
			;
			stream <<
				t.cell(
					"Page CMS pour rangée",
					t.getForm().getTextInputAutoCompleteFromTableSync(
						UpdateDisplayTypeAction::PARAMETER_DISPLAY_ROW_PAGE_ID,
						_type->getDisplayRowPage() ? lexical_cast<string>(_type->getDisplayRowPage()->getKey()) : string(),
						_type->getDisplayRowPage() ? lexical_cast<string>(_type->getDisplayRowPage()->getName()) : string(),
						lexical_cast<string>(WebPageTableSync::TABLE.ID),
						string(),string(),
						true, true, true, true
				)	)
			;
			stream <<
				t.cell(
					"Page CMS pour destination",
					t.getForm().getTextInputAutoCompleteFromTableSync(
						UpdateDisplayTypeAction::PARAMETER_DISPLAY_DESTINATION_PAGE_ID,
						_type->getDisplayDestinationPage() ? lexical_cast<string>(_type->getDisplayDestinationPage()->getKey()) : string(),
						_type->getDisplayDestinationPage() ? lexical_cast<string>(_type->getDisplayDestinationPage()->getName()) : string(),
						lexical_cast<string>(WebPageTableSync::TABLE.ID),
						string(),string(),
						true, true, true, true
				)	)
			;
			stream <<
				t.cell(
					"Page CMS pour destination en correspondance",
					t.getForm().getTextInputAutoCompleteFromTableSync(
						UpdateDisplayTypeAction::PARAMETER_DISPLAY_TRANSFER_DESTINATION_PAGE_ID,
						_type->getDisplayTransferDestinationPage() ? lexical_cast<string>(_type->getDisplayTransferDestinationPage()->getKey()) : string(),
						_type->getDisplayTransferDestinationPage() ? lexical_cast<string>(_type->getDisplayTransferDestinationPage()->getName()) : string(),
						lexical_cast<string>(WebPageTableSync::TABLE.ID),
						string(),string(),
						true, true, true, true
				)	)
			;
			stream <<
				t.cell(
					"Page CMS pour parser les résultats de monitoring",
					t.getForm().getTextInputAutoCompleteFromTableSync(
						UpdateDisplayTypeAction::PARAMETER_MONITORING_PARSER_PAGE_ID,
						_type->getMonitoringParserPage() ? lexical_cast<string>(_type->getMonitoringParserPage()->getKey()) : string(),
						_type->getMonitoringParserPage() ? lexical_cast<string>(_type->getMonitoringParserPage()->getName()) : string(),
						lexical_cast<string>(WebPageTableSync::TABLE.ID),
						string(),string(),
						true, true, true, true
				)	)
			;
			stream <<
				t.cell(
					"Rangées",
					t.getForm().getSelectNumberInput(
						UpdateDisplayTypeAction::PARAMETER_ROWS_NUMBER,
						1, 99,
						optional<size_t>(_type->getRowNumber())
				)	)
			;
			stream << t.cell(
				"Max arrêts intermédiaires",
				t.getForm().getSelectNumberInput(
					UpdateDisplayTypeAction::PARAMETER_MAX_STOPS_NUMBER,
					0, 99,
					_type->getMaxStopsNumber(),
					1,
					"(pas de limite)"
			)	);
			stream << t.title("Paramètres sonores");
			stream << t.cell("Interface vocale", t.getForm().getSelectInput(
					UpdateDisplayTypeAction::PARAMETER_AUDIO_INTERFACE_ID,
					InterfaceTableSync::_GetInterfaceLabels(optional<string>(), string("(aucune)"), string("undefined")),
					optional<RegistryKeyType>(_type->getAudioInterface() ? _type->getAudioInterface()->getKey() : 0)
			)	);
			stream << t.title("Paramètres de supervision");
			stream <<
				t.cell(
					"Protocole de supervision",
					t.getForm().getSelectInput(
						UpdateDisplayTypeAction::PARAMETER_MONITORING_INTERFACE_ID,
						InterfaceTableSync::GetInterfaceLabels<ParseDisplayReturnInterfacePage>(optional<string>()),
						optional<RegistryKeyType>(_type->getMonitoringInterface() ? _type->getMonitoringInterface()->getKey() : 0)
				)	)
			;
			stream <<
				t.cell(
					"Durée entre les contrôles",
					t.getForm().getSelectNumberInput(
						UpdateDisplayTypeAction::PARAMETER_TIME_BETWEEN_CHECKS,
						1, 120,
						optional<size_t>(_type->getTimeBetweenChecks().total_seconds() / 60),
						1
					)+ " minutes"
				);
			stream << t.close();
		}

		bool DisplayTypeAdmin::isAuthorized(
			const security::User& user
		) const	{
			if (_type.get() == NULL) return false;
			return user.getProfile()->isAuthorized<ArrivalDepartureTableRight>(READ);
		}



		std::string DisplayTypeAdmin::getTitle() const
		{
			return _type.get() ? _type->getName() : DEFAULT_TITLE;
		}



		void DisplayTypeAdmin::setType(
			boost::shared_ptr<const DisplayType> value
		){
			_type = value;
		}


		boost::shared_ptr<const DisplayType> DisplayTypeAdmin::getType() const
		{
			return _type;
		}


		bool DisplayTypeAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _type == static_cast<const DisplayTypeAdmin&>(other)._type;
		}
	}
}
