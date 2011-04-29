
//////////////////////////////////////////////////////////////////////////
/// PTQualityControlAdmin class implementation.
///	@file PTQualityControlAdmin.cpp
///	@author RCSobility
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "PTQualityControlAdmin.hpp"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "StopPoint.hpp"
#include "ResultHTMLTable.h"
#include "StopArea.hpp"
#include "City.h"
#include "HTMLModule.h"
#include "AdminFunctionRequest.hpp"
#include "StopPointAdmin.hpp"
#include "PTPlacesAdmin.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace html;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, PTQualityControlAdmin>::FACTORY_KEY("PTQualityControlAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTQualityControlAdmin>::ICON("cog.png");
		template<> const string AdminInterfaceElementTemplate<PTQualityControlAdmin>::DEFAULT_TITLE("Contrôle qualité");
	}

	namespace pt
	{
		const string PTQualityControlAdmin::PARAM_RUN_CONTROL("rc");

		const string PTQualityControlAdmin::TAB_STOPS_WITHOUT_COORDINATE("swc");
		const string PTQualityControlAdmin::TAB_CITIES_WITHOUT_MAIN_STOP("cwm");



		PTQualityControlAdmin::PTQualityControlAdmin()
			: AdminInterfaceElementTemplate<PTQualityControlAdmin>(),
			_runControl(false)
		{ }



		void PTQualityControlAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_runControl = map.getDefault<bool>(PARAM_RUN_CONTROL);
		}



		ParametersMap PTQualityControlAdmin::getParametersMap() const
		{
			ParametersMap m;
			m.insert(PARAM_RUN_CONTROL, _runControl);
			return m;
		}



		bool PTQualityControlAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void PTQualityControlAdmin::display(
			ostream& stream,
			const admin::AdminRequest& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// STOPS WITHOUT COORDINATE TAB
			if (openTabContent(stream, TAB_STOPS_WITHOUT_COORDINATE))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					AdminFunctionRequest<StopPointAdmin> openRequest(request);

					HTMLTable::ColsVector c;
					c.push_back("Commune");
					c.push_back("Zone d'arrêt");
					c.push_back("Arrêt");
					c.push_back("Action");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();
					BOOST_FOREACH(const StopPoint::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<StopPoint>())
					{
						if(it.second->getGeometry().get() && !it.second->getGeometry()->isEmpty())
						{
							continue;
						}
						openRequest.getPage()->setStop(it.second);
						stream << t.row();
						stream << t.col() << it.second->getConnectionPlace()->getCity()->getName();
						stream << t.col() << it.second->getConnectionPlace()->getName();
						stream << t.col() << it.second->getName();
						stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), StopPointAdmin::ICON);
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request);
					runRequest.getPage()->setRunControl(true);

					stream <<
						"<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />" <<
						HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), ICON) <<
						"</p>"
					;
				}
			}

			if (openTabContent(stream, TAB_CITIES_WITHOUT_MAIN_STOP))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					AdminFunctionRequest<PTPlacesAdmin> openRequest(request);

					HTMLTable::ColsVector c;
					c.push_back("Commune");
					c.push_back("Action");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();
					BOOST_FOREACH(const City::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<City>())
					{
						if(it.second->getLexicalMatcher(StopArea::FACTORY_KEY).size() == 0 || !it.second->getIncludedPlaces().empty())
						{
							continue;
						}
						openRequest.getPage()->setCity(it.second);
						stream << t.row();
						stream << t.col() << it.second->getName();
						stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), PTPlacesAdmin::ICON);
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request);
					runRequest.getPage()->setRunControl(true);

					stream <<
						"<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />" <<
						HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), ICON) <<
						"</p>"
					;
				}
			}

			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		AdminInterfaceElement::PageLinks PTQualityControlAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const PTModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}

			return links;
		}



		std::string PTQualityControlAdmin::getTitle() const
		{
			return DEFAULT_TITLE + (_runControl ? (" (" + _activeTab +")") : string());
		}



		void PTQualityControlAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();

			_tabs.push_back(Tab("Arrêts non localisés", TAB_STOPS_WITHOUT_COORDINATE, false));
			_tabs.push_back(Tab("Localités sans arrêt principal", TAB_CITIES_WITHOUT_MAIN_STOP, false));

			_tabBuilded = true;
		}
	}
}
