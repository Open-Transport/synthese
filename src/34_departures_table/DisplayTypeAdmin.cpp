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
#include "QueryString.h"
#include "AdminParametersException.h"
#include "DisplayType.h"
#include "DisplayTypeTableSync.h"
#include "PropertiesHTMLTable.h"
#include "UpdateDisplayTypeAction.h"
#include "InterfaceModule.h"
#include "ActionFunctionRequest.h"
#include "AdminRequest.h"
#include "DisplayTypeRemoveAction.h"
#include "Interface.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace departurestable;
	using namespace html;

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
		
		void DisplayTypeAdmin::setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));

			if (id == QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				return;

			try
			{
				_type = DisplayTypeTableSync::GetEditable(id, &_env, UP_LINKS_LOAD_LEVEL);
			}
			catch (ObjectNotFoundException<DisplayType>& e)
			{
				throw AdminParametersException("Display type not found by "+ FACTORY_KEY +" : "+ e.getMessage());
			}
		}
		
		void DisplayTypeAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			// Requests
			ActionFunctionRequest<UpdateDisplayTypeAction,AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage<DisplayTypeAdmin>();
			updateRequest.setObjectId(_type->getKey());
			updateRequest.getAction()->setTypeId(_type->getKey());

			ActionFunctionRequest<DisplayTypeRemoveAction,AdminRequest> deleteRequest(request);
			deleteRequest.getFunction()->setPage<DisplayTypesAdmin>();
			deleteRequest.getAction()->setType(_type);

			// Display
			PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
			stream << "<h1>Modification du type d'affichage</h1>";
			stream << t.open();
			stream << t.title("Propri�t�s");
			stream << t.cell("Nom", t.getForm().getTextInput(UpdateDisplayTypeAction::PARAMETER_NAME, _type->getName()));
			stream << t.title("Param�tres d'affichage");
			stream << t.cell("Interface d'affichage", t.getForm().getSelectInput(UpdateDisplayTypeAction::PARAMETER_INTERFACE_ID, InterfaceModule::getInterfaceLabels(true), _type->getDisplayInterface() ? _type->getDisplayInterface()->getKey() : uid(0)));
			stream << t.cell("Rang�es", t.getForm().getSelectNumberInput(UpdateDisplayTypeAction::PARAMETER_ROWS_NUMBER, 1, 99, _type->getRowNumber()));
			stream << t.cell("Max arr�ts interm�diaires", t.getForm().getSelectNumberInput(UpdateDisplayTypeAction::PARAMETER_MAX_STOPS_NUMBER, UNKNOWN_VALUE, 99, _type->getMaxStopsNumber()));
			stream << t.title("Param�tres sonores");
			stream << t.cell("Interface vocale", t.getForm().getSelectInput(UpdateDisplayTypeAction::PARAMETER_AUDIO_INTERFACE_ID, InterfaceModule::getInterfaceLabels(true), _type->getAudioInterface() ? _type->getAudioInterface()->getKey() : uid(0)));
			stream << t.title("Param�tres de supervision");
			stream << t.cell("Protocole de supervision", t.getForm().getSelectInput(UpdateDisplayTypeAction::PARAMETER_MONITORING_INTERFACE_ID, InterfaceModule::getInterfaceLabels(true), _type->getMonitoringInterface() ? _type->getMonitoringInterface()->getKey() : uid(0)));
			stream << t.cell("Dur�e entre les contr�les", t.getForm().getTextInput(UpdateDisplayTypeAction::PARAMETER_TIME_BETWEEN_CHECKS, Conversion::ToString(_type->getTimeBetweenChecks())));
			stream << t.close();
		}

		bool DisplayTypeAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return true;
		}
		
		AdminInterfaceElement::PageLinks DisplayTypeAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
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
			return _type.get() ? QueryString::PARAMETER_OBJECT_ID : string();
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
