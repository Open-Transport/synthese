
/** BookableCommercialLinesAdmin class implementation.
	@file BookableCommercialLinesAdmin.cpp
	@author Hugues Romain
	@date 2008

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "BookableCommercialLinesAdmin.h"

#include "ResaModule.h"
#include "BookableCommercialLineAdmin.h"
#include "ResaRight.h"

#include "CommercialLine.h"
#include "CommercialLineTableSync.h"

#include "QueryString.h"

#include "AdminParametersException.h"
#include "ModuleAdmin.h"
#include "AdminRequest.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace resa;
	using namespace env;
	using namespace security;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, BookableCommercialLinesAdmin>::FACTORY_KEY("BookableCommercialLinesAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<BookableCommercialLinesAdmin>::ICON("chart_line.png");
		template<> const string AdminInterfaceElementTemplate<BookableCommercialLinesAdmin>::DEFAULT_TITLE("Réservations par ligne");
	}

	namespace resa
	{
		BookableCommercialLinesAdmin::BookableCommercialLinesAdmin()
			: AdminInterfaceElementTemplate<BookableCommercialLinesAdmin>()
		{ }
		
		void BookableCommercialLinesAdmin::setFromParametersMap(const ParametersMap& map)
		{
		}
		
		void BookableCommercialLinesAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			// Search
			Env env;
			CommercialLineTableSync::Search(
				env,
				request->getUser()->getProfile()->getRightsForModuleClass<ResaRight>()
				, request->getUser()->getProfile()->getGlobalPublicRight<ResaRight>() >= READ
				, READ
				, _requestParameters.first
				, _requestParameters.maxSize
				, false, true, true, true
			);
			ResultHTMLTable::ResultParameters rp;
			rp.setFromResult(_requestParameters, env.getEditableRegistry<CommercialLine>());

			// Requests
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage<BookableCommercialLineAdmin>();

			FunctionRequest<AdminRequest> openRequest(request);
			openRequest.getFunction()->setPage<BookableCommercialLineAdmin>();

			// Display
			stream << "<h1>Lignes</h1>";

			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(string(),"Ligne"));
			h.push_back(make_pair(string(),"Description"));
			h.push_back(make_pair(string(),"Liens"));
			ResultHTMLTable t(h, searchRequest.getHTMLForm(), _requestParameters, rp);
			stream << t.open();

			BOOST_FOREACH(shared_ptr<CommercialLine> line, env.getRegistry<CommercialLine>())
			{
				openRequest.setObjectId(line->getKey());

				stream << t.row();
				stream << t.col(1, line->getStyle()) << line->getShortName();
				stream << t.col() << line->getName();
				stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "chart_line.png");
			}
			stream << t.close();

		}

		bool BookableCommercialLinesAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return request->isAuthorized<ResaRight>(READ);
		}
		
		AdminInterfaceElement::PageLinks BookableCommercialLinesAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(parentLink.factoryKey == admin::ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == ResaModule::FACTORY_KEY)
				links.push_back(getPageLink());
			return links;
		}
		
		AdminInterfaceElement::PageLinks BookableCommercialLinesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const {
			AdminInterfaceElement::PageLinks links;

			Env env;
			CommercialLineTableSync::Search(
				env,
				request->getUser()->getProfile()->getRightsForModuleClass<ResaRight>()
				, request->getUser()->getProfile()->getGlobalPublicRight<ResaRight>() >= READ
				, READ
				, 0, 0
				, false, true, true, true
			);
			BOOST_FOREACH(shared_ptr<CommercialLine> line, env.getRegistry<CommercialLine>())
			{
				AdminInterfaceElement::PageLink link;
				link.factoryKey = BookableCommercialLineAdmin::FACTORY_KEY;
				link.icon = "chart_line.png";
				link.name = line->getName();
				link.parameterName = QueryString::PARAMETER_OBJECT_ID;
				link.parameterValue = Conversion::ToString(line->getKey());
				links.push_back(link);
			}

			return links;
		}
	}
}
