////////////////////////////////////////////////////////////////////////////////
/// DisplayTypeAdmin class implementation.
///	@file DisplayTypeAdmin.cpp
///	@author Hugues Romain
///	@date 2008-12-19 17:58
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
#include "Request.h"
#include "AdminParametersException.h"
#include "DisplayType.h"
#include "DisplayTypeTableSync.h"
#include "PropertiesHTMLTable.h"
#include "UpdateDisplayTypeAction.h"
#include "ActionFunctionRequest.h"
#include "AdminRequest.h"
#include "DisplayTypeRemoveAction.h"
#include "Interface.h"
#include "ArrivalDepartureTableRight.h"
#include "DeparturesTableInterfacePage.h"
#include "ParseDisplayReturnInterfacePage.h"
#include "InterfaceTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace departurestable;
	using namespace html;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, DisplayTypeAdmin>::FACTORY_KEY("DisplayTypeAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DisplayTypeAdmin>::ICON("monitor.png");
		template<> const string AdminInterfaceElementTemplate<DisplayTypeAdmin>::DEFAULT_TITLE("Type d'affichage");
	}

	namespace departurestable
	{
		DisplayTypeAdmin::DisplayTypeAdmin()
			: AdminInterfaceElementTemplate<DisplayTypeAdmin>()
		{ }
		
		void DisplayTypeAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){
			uid id(map.getUid(Request::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			if (id == Request::UID_WILL_BE_GENERATED_BY_THE_ACTION) return;

			try
			{
				_type = DisplayTypeTableSync::GetEditable(id, _env, UP_LINKS_LOAD_LEVEL);
			}
			catch (ObjectNotFoundException<DisplayType>& e)
			{
				throw AdminParametersException("Display type not found by "+ FACTORY_KEY +" : "+ e.getMessage());
			}
		}
		
		
		
		ParametersMap DisplayTypeAdmin::getParametersMap() const
		{
			ParametersMap m;
			return m;
		}
		
		
		
		void DisplayTypeAdmin::display(ostream& stream, VariablesMap& variables) const
		{
			// Requests
			ActionFunctionRequest<UpdateDisplayTypeAction,AdminRequest> updateRequest(_request);
			updateRequest.getFunction()->setPage<DisplayTypeAdmin>();
			updateRequest.setObjectId(_type->getKey());
			updateRequest.getAction()->setTypeId(_type->getKey());

			ActionFunctionRequest<DisplayTypeRemoveAction,AdminRequest> deleteRequest(_request);
			deleteRequest.getFunction()->setPage<DisplayTypesAdmin>();
			deleteRequest.getAction()->setType(_type);

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
						_type->getDisplayInterface() ? _type->getDisplayInterface()->getKey() : uid(0)
				)	)
			;
			stream <<
				t.cell(
					"Rangées",
					t.getForm().getSelectNumberInput(
						UpdateDisplayTypeAction::PARAMETER_ROWS_NUMBER,
						1, 99,
						_type->getRowNumber()
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
					_type->getAudioInterface() ? _type->getAudioInterface()->getKey() : uid(0)
			)	);
			stream << t.title("Paramètres de supervision");
			stream <<
				t.cell(
					"Protocole de supervision",
					t.getForm().getSelectInput(
						UpdateDisplayTypeAction::PARAMETER_MONITORING_INTERFACE_ID,
						InterfaceTableSync::GetInterfaceLabels<ParseDisplayReturnInterfacePage>(optional<string>()),
						_type->getMonitoringInterface() ? _type->getMonitoringInterface()->getKey() : uid(0)
				)	)
			;
			stream <<
				t.cell(
					"Durée entre les contrôles",
					t.getForm().getSelectNumberInput(
						UpdateDisplayTypeAction::PARAMETER_TIME_BETWEEN_CHECKS,
						1, 120,
						_type->getTimeBetweenChecks(),
						1
					)+ " minutes"	
				);
			stream << t.close();
		}

		bool DisplayTypeAdmin::isAuthorized() const
		{
			if (_type.get() == NULL) return false;
			return _request->isAuthorized<ArrivalDepartureTableRight>(READ);
		}
		
		AdminInterfaceElement::PageLinks DisplayTypeAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(parentLink.factoryKey == DisplayTypesAdmin::FACTORY_KEY && parentLink.parameterValue == Conversion::ToString(_type->getKey()))
			{
				links.push_back(getPageLink());
			}
			return links;
		}
		


		std::string DisplayTypeAdmin::getTitle() const
		{
			return _type.get() ? _type->getName() : DEFAULT_TITLE;
		}

		std::string DisplayTypeAdmin::getParameterName() const
		{
			return _type.get() ? Request::PARAMETER_OBJECT_ID : string();
		}

		std::string DisplayTypeAdmin::getParameterValue() const
		{
			return _type.get() ? Conversion::ToString(_type->getKey()) : string();
		}



		void DisplayTypeAdmin::setType(
			boost::shared_ptr<DisplayType> value
		) {
			_type = value;
		}
	}
}
