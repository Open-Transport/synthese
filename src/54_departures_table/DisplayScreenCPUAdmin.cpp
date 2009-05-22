////////////////////////////////////////////////////////////////////////////////
/// DisplayScreenCPUAdmin class implementation.
///	@file DisplayScreenCPUAdmin.cpp
///	@author Hugues Romain
///	@date 2008-12-26 16:37
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

#include "DisplayScreenCPUAdmin.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenCPUUpdateAction.h"
#include "DisplayScreenCPUTableSync.h"
#include "DisplaySearchAdmin.h"
#include "DeparturesTableModule.h"
#include "AdminRequest.h"
#include "ActionFunctionRequest.h"
#include "AdminParametersException.h"
#include "ArrivalDepartureTableRight.h"
#include "ArrivalDepartureTableLog.h"
#include "PropertiesHTMLTable.h"
#include "ResultHTMLTable.h"
#include "DisplayAdmin.h"
#include "DisplayScreenTableSync.h"
#include "CreateDisplayScreenAction.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;


namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace departurestable;
	using namespace security;
	using namespace html;
	

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, DisplayScreenCPUAdmin>::FACTORY_KEY("DisplayScreenCPUAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DisplayScreenCPUAdmin>::ICON("server.png");
		template<> const string AdminInterfaceElementTemplate<DisplayScreenCPUAdmin>::DEFAULT_TITLE("Unité centrale");
	}

	namespace departurestable
	{
		const string DisplayScreenCPUAdmin::TAB_DISPLAYS("di");
		const string DisplayScreenCPUAdmin::TAB_MAINTENANCE("ma");
		const string DisplayScreenCPUAdmin::TAB_LOG("lo");
		const string DisplayScreenCPUAdmin::TAB_TECHNICAL("te");

		DisplayScreenCPUAdmin::DisplayScreenCPUAdmin()
		:	AdminInterfaceElementTemplate<DisplayScreenCPUAdmin>(),
			_generalLogView(TAB_LOG)
		{ }
		
		
		
		void DisplayScreenCPUAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){
			uid id(_request->getObjectId());
			if (id == Request::UID_WILL_BE_GENERATED_BY_THE_ACTION) return;
			
			try
			{
				_cpu = DisplayScreenCPUTableSync::Get(id, _env);
			}
			catch (ObjectNotFoundException<DisplayScreenCPU>& e)
			{
				throw AdminParametersException("Specified central unit "+ FACTORY_KEY +" not found "+ e.getMessage());
			}

			_generalLogView.set(map, ArrivalDepartureTableLog::FACTORY_KEY, _cpu->getKey());

			if(!doDisplayPreparationActions) return;

			DisplayScreenTableSync::SearchFromCPU(_env, _cpu->getKey());
		}
		
		
		
		server::ParametersMap DisplayScreenCPUAdmin::getParametersMap() const
		{
			ParametersMap m;
			return m;
		}
		
		
		
		void DisplayScreenCPUAdmin::display(
			ostream& stream,
			VariablesMap& variables
		) const	{
			////////////////////////////////////////////////////////////////////
			// TECHNICAL TAB
			if (openTabContent(stream, TAB_TECHNICAL))
			{
				ActionFunctionRequest<DisplayScreenCPUUpdateAction, AdminRequest> updateRequest(_request);
				updateRequest.getFunction()->setSamePage(this);
				updateRequest.getAction()->setCPU(_cpu->getKey());

				stream << "<h1>Propriétés</h1>";

				PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
				stream << t.open();
				stream << t.cell("ID", lexical_cast<string>(_cpu->getKey()));
				stream << t.cell("Nom", t.getForm().getTextInput(DisplayScreenCPUUpdateAction::PARAMETER_NAME, _cpu->getName()));
				stream << t.cell("Adresse MAC", t.getForm().getTextInput(DisplayScreenCPUUpdateAction::PARAMETER_MAC_ADDRESS, _cpu->getMacAddress()));
				stream << t.close();

			}
			if (openTabContent(stream, TAB_MAINTENANCE))
			{

			}
			if (openTabContent(stream, TAB_DISPLAYS))
			{
				FunctionRequest<AdminRequest> displayRequest(_request);
				displayRequest.getFunction()->setPage<DisplayAdmin>();

				ActionFunctionRequest<CreateDisplayScreenAction,AdminRequest> createDisplayRequest(_request);
				createDisplayRequest.getFunction()->setPage<DisplayAdmin>();
				createDisplayRequest.setObjectId(Request::UID_WILL_BE_GENERATED_BY_THE_ACTION);
				createDisplayRequest.getAction()->setCPU(_cpu->getKey());
				
				HTMLTable::ColsVector c;
				c.push_back("Nom");
				c.push_back("Port COM");
				c.push_back("Code BUS");
				c.push_back("ID");
				c.push_back("Actions");
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				BOOST_FOREACH(const DisplayScreen* screen, _cpu->getWiredScreens())
				{
					displayRequest.setObjectId(screen->getKey());
					stream << t.row();
					stream << t.col() << screen->getLocalizationComment();
					stream << t.col() << screen->getComPort();
					stream << t.col() << screen->getWiringCode();
					stream << t.col() << screen->getKey();
					stream << t.col() << displayRequest.getHTMLForm().getLinkButton("Ouvrir", string(), "monitor.png");
				}

				stream << t.close();

				stream << createDisplayRequest.getHTMLForm().getLinkButton("Créer un nouvel afficheur sur l'unité centrale", string(), "monitor_add.png");
			}

			////////////////////////////////////////////////////////////////////
			// LOG TAB
			if (openTabContent(stream, TAB_LOG))
			{
				// Log search
				FunctionRequest<AdminRequest> searchRequest(_request);
				searchRequest.getFunction()->setSamePage(this);

				_generalLogView.display(
					stream,
					searchRequest
				);
			}
			closeTabContent(stream);
		}

		bool DisplayScreenCPUAdmin::isAuthorized() const
		{
			if (_request->getObjectId() == Request::UID_WILL_BE_GENERATED_BY_THE_ACTION) return true;
			if (_cpu.get() == NULL) return false;
			if (_cpu->getPlace() == NULL) return _request->isAuthorized<ArrivalDepartureTableRight>(READ);
			return _request->isAuthorized<ArrivalDepartureTableRight>(READ, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_cpu->getPlace()->getKey()));
		}
		
		AdminInterfaceElement::PageLinks DisplayScreenCPUAdmin::getSubPagesOfParent(
			const PageLink& parentLink,
			const AdminInterfaceElement& currentPage
		) const	{
			PageLinks links;
			if(	parentLink.factoryKey == DisplaySearchAdmin::FACTORY_KEY &&
				parentLink.parameterValue.empty()
			){
				if (currentPage.getFactoryKey() == FACTORY_KEY)
				{
					links.push_back(currentPage.getPageLink());
				}
			}

			return links;
		}
		
		AdminInterfaceElement::PageLinks DisplayScreenCPUAdmin::getSubPages(
			const AdminInterfaceElement& currentPage
		) const {
			AdminInterfaceElement::PageLinks links;
			return links;
		}


		std::string DisplayScreenCPUAdmin::getTitle() const
		{
			return _cpu.get() ? _cpu->getName() : DEFAULT_TITLE;
		}

		std::string DisplayScreenCPUAdmin::getParameterName() const
		{
			return _cpu.get() ? Request::PARAMETER_OBJECT_ID : string();
		}

		std::string DisplayScreenCPUAdmin::getParameterValue() const
		{
			return _cpu.get() ? Conversion::ToString(_cpu->getKey()) : string();
		}

		void DisplayScreenCPUAdmin::_buildTabs(
		) const {
			_tabs.clear();

			bool writePermission(_request->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_cpu->getPlace()->getKey())));
			_tabs.push_back(Tab("Technique", TAB_TECHNICAL, writePermission, "cog.png"));
			_tabs.push_back(Tab("Maintenance", TAB_MAINTENANCE, writePermission, "wrench.png"));
			_tabs.push_back(Tab("Afficheurs", TAB_DISPLAYS, writePermission, "monitor.png"));
			_tabs.push_back(Tab("Journal", TAB_LOG, writePermission, "book.png"));
			
			_tabBuilded = true;
		}
	}
}
