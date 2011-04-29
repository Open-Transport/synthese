
//////////////////////////////////////////////////////////////////////////
/// ResaStatisticsMenuAdmin class implementation.
///	@file ResaStatisticsMenuAdmin.cpp
///	@author Hugues
///	@date 2009
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

#include "ResaStatisticsMenuAdmin.h"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "ResaModule.h"
#include "ResaRight.h"
#include "ResaStatisticsAdmin.h"
#include "CallStatisticsAdmin.h"
#include "AdminFunctionRequest.hpp"
#include "CommercialLineTableSync.h"
#include "CommercialLine.h"
#include "HTMLModule.h"
#include "Profile.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace resa;
	using namespace pt;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ResaStatisticsMenuAdmin>::FACTORY_KEY("ResaStatisticsMenuAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ResaStatisticsMenuAdmin>::ICON("chart_pie.png");
		template<> const string AdminInterfaceElementTemplate<ResaStatisticsMenuAdmin>::DEFAULT_TITLE("Statistiques");
	}

	namespace resa
	{
		ResaStatisticsMenuAdmin::ResaStatisticsMenuAdmin()
			: AdminInterfaceElementTemplate<ResaStatisticsMenuAdmin>()
		{ }



		void ResaStatisticsMenuAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			// Search table initialization
			_requestParameters.setFromParametersMap(map.getMap());
		}



		ParametersMap ResaStatisticsMenuAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			return m;
		}



		bool ResaStatisticsMenuAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL, string());
		}



		void ResaStatisticsMenuAdmin::display(
			ostream& stream,
			const AdminRequest& request
		) const	{

			// Display
			AdminFunctionRequest<CallStatisticsAdmin> openCallsRequest(request);
			if(request.getFunction()->getPage()->isAuthorized(*request.getUser()))
			{
				stream << "<h1>Statistiques d'appels</h1>";
				stream << "<p>" << HTMLModule::getLinkButton(openCallsRequest.getURL(), "Statistiques appels", string(), CallStatisticsAdmin::ICON) << "</p>";
			}


			// Search
			CommercialLineTableSync::SearchResult lines(
				CommercialLineTableSync::Search(
					*_env,
					request.getUser()->getProfile()->getRightsForModuleClass<ResaRight>(),
					request.getUser()->getProfile()->getGlobalPublicRight<ResaRight>() >= READ
					, READ
					, _requestParameters.first
					, _requestParameters.maxSize
					, false, true, true, true
			)	);

			// Requests
			AdminFunctionRequest<ResaStatisticsMenuAdmin> searchRequest(request);

			AdminFunctionRequest<ResaStatisticsAdmin> openRequest(request);

			// Display
			stream << "<h1>Statistiques de réservation par lignes</h1>";

			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(string(),"Ligne"));
			h.push_back(make_pair(string(),"Description"));
			h.push_back(make_pair(string(),"Liens"));
			ResultHTMLTable t(h, searchRequest.getHTMLForm(), _requestParameters, lines);
			stream << t.open();

			BOOST_FOREACH(shared_ptr<CommercialLine> line, lines)
			{
				openRequest.getPage()->setCommercialLine(line);

				stream << t.row();
				stream << t.col(1, line->getStyle()) << line->getShortName();
				stream << t.col() << line->getName();
				stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), ResaStatisticsAdmin::ICON);
			}
			stream << t.close();


		}



		AdminInterfaceElement::PageLinks ResaStatisticsMenuAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const AdminRequest& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const ResaModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}

			return links;
		}



		AdminInterfaceElement::PageLinks ResaStatisticsMenuAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const AdminRequest& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			shared_ptr<CallStatisticsAdmin> p(
				getNewPage<CallStatisticsAdmin>()
			);
			if(p->isAuthorized(*request.getUser()))
			{
				links.push_back(p);
			}

			CommercialLineTableSync::SearchResult lines(
				CommercialLineTableSync::Search(
					*_env,
					request.getUser()->getProfile()->getRightsForModuleClass<ResaRight>(),
					request.getUser()->getProfile()->getGlobalPublicRight<ResaRight>() >= READ
					, READ
					, 0,
					optional<size_t>()
					, false, true, true, true
			)	);
			BOOST_FOREACH(shared_ptr<CommercialLine> line, lines)
			{
				shared_ptr<ResaStatisticsAdmin> p(
					getNewPage<ResaStatisticsAdmin>()
				);
				p->setCommercialLine(line);
				links.push_back(p);
			}

			return links;
		}
	}
}
