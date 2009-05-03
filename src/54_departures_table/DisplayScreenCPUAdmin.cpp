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
#include "DisplayScreenCPUTableSync.h"
#include "DisplaySearchAdmin.h"
#include "DeparturesTableModule.h"
#include "Request.h"
#include "AdminParametersException.h"
#include "ArrivalDepartureTableRight.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace departurestable;
	using namespace security;

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
			: AdminInterfaceElementTemplate<DisplayScreenCPUAdmin>()
		{ }
		
		
		
		void DisplayScreenCPUAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){
			uid id(map.getUid(Request::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			if (id == Request::UID_WILL_BE_GENERATED_BY_THE_ACTION) return;
			
			try
			{
				_cpu = DisplayScreenCPUTableSync::Get(id, _env);
			}
			catch (ObjectNotFoundException<DisplayScreenCPU>& e)
			{
				throw AdminParametersException("Specified central unit "+ FACTORY_KEY +" not found "+ e.getMessage());
			}
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
			if (openTabContent(stream, TAB_TECHNICAL))
			{
				
			}
			if (openTabContent(stream, TAB_MAINTENANCE))
			{

			}
			if (openTabContent(stream, TAB_DISPLAYS))
			{

			}
			if (openTabContent(stream, TAB_LOG))
			{

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
			if (parentLink.factoryKey == DisplaySearchAdmin::FACTORY_KEY)
			{
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
			_tabs.push_back(Tab(TAB_TECHNICAL, "Technique", writePermission, "cog.png"));
			_tabs.push_back(Tab(TAB_MAINTENANCE, "Maintenance", writePermission, "wrench.png"));
			_tabs.push_back(Tab(TAB_DISPLAYS, "Afficheurs", writePermission, "monitor.png"));
			_tabs.push_back(Tab(TAB_TECHNICAL, "Journal", writePermission, "book.png"));
			
			_tabBuilded = true;
		}
	}
}
