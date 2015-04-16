
/** BookableCommercialLinesAdmin class implementation.
	@file BookableCommercialLinesAdmin.cpp
	@author Hugues Romain
	@date 2008

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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
#include "Profile.h"
#include "User.h"
#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "ModuleAdmin.h"
#include "AdminInterfaceElement.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace resa;
	using namespace pt;
	using namespace security;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, BookableCommercialLinesAdmin>::FACTORY_KEY("BookableCommercialLinesAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<BookableCommercialLinesAdmin>::ICON("resa_compulsory.png");
		template<> const string AdminInterfaceElementTemplate<BookableCommercialLinesAdmin>::DEFAULT_TITLE("Réservations par ligne");
	}

	namespace resa
	{
		BookableCommercialLinesAdmin::BookableCommercialLinesAdmin()
			: AdminInterfaceElementTemplate<BookableCommercialLinesAdmin>()
		{ }

		void BookableCommercialLinesAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_requestParameters.setFromParametersMap(map);
		}


		util::ParametersMap BookableCommercialLinesAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			return m;
		}



		void BookableCommercialLinesAdmin::display(
			ostream& stream,
			const Request& _request
		) const {
			// Search
			CommercialLineTableSync::SearchResult lines(
				CommercialLineTableSync::Search(
					*_env,
					_request.getUser()->getProfile()->getRightsForModuleClass<ResaRight>()
					, _request.getUser()->getProfile()->getGlobalPublicRight<ResaRight>() >= READ
					, READ
					, _requestParameters.first
					, _requestParameters.maxSize
					, false, true, true, true
			)	);
			std::sort(lines.begin(), lines.end(), CommercialLine::PointerComparator());

			// Requests
			AdminFunctionRequest<BookableCommercialLinesAdmin> searchRequest(_request, *this);

			AdminFunctionRequest<BookableCommercialLineAdmin> openRequest(_request);

			// Display
			stream << "<h1>Lignes</h1>";

			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(string(),"Ligne"));
			h.push_back(make_pair(string(),"Description"));
			h.push_back(make_pair(string(),"Liens"));
			ResultHTMLTable t(h, searchRequest.getHTMLForm(), _requestParameters, lines);
			stream << t.open();

			BOOST_FOREACH(const boost::shared_ptr<CommercialLine>& line, lines)
			{
				openRequest.getPage()->setCommercialLine(line);

				stream << t.row();
				stream << t.col(1, line->getStyle()) << line->getShortName();
				stream << t.col() << line->getName();
				stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "/admin/img/chart_line.png");
			}
			stream << t.close();

		}



		bool BookableCommercialLinesAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL, string());
		}



		AdminInterfaceElement::PageLinks BookableCommercialLinesAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
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

		AdminInterfaceElement::PageLinks BookableCommercialLinesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const {
			AdminInterfaceElement::PageLinks links;

			CommercialLineTableSync::SearchResult lines(
				CommercialLineTableSync::Search(
					Env::GetOfficialEnv(),
					request.getUser()->getProfile()->getRightsForModuleClass<ResaRight>(),
					request.getUser()->getProfile()->getGlobalPublicRight<ResaRight>() >= READ
					, READ
					, 0,
					optional<size_t>()
					, false, true, true, true
			)	);
			std::sort(lines.begin(), lines.end(), CommercialLine::PointerComparator());

			BOOST_FOREACH(const boost::shared_ptr<CommercialLine>& line, lines)
			{
				boost::shared_ptr<BookableCommercialLineAdmin> p(
					getNewPage<BookableCommercialLineAdmin>()
				);
				p->setCommercialLine(line);
				links.push_back(p);
			}

			return links;
		}
	}
}
