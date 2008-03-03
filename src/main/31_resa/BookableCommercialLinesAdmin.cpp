
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

#include "31_resa/BookableCommercialLineAdmin.h"
#include "31_resa/ResaRight.h"

#include "15_env/CommercialLine.h"
#include "15_env/CommercialLineTableSync.h"

#include "30_server/QueryString.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/ModuleAdmin.h"

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
			stream << "Not implemented yet, use treeview instead.";
		}

		bool BookableCommercialLinesAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			/// @todo Implement the right control;
			return true;
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
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const {
			AdminInterfaceElement::PageLinks links;

			vector<shared_ptr<CommercialLine> > lines(CommercialLineTableSync::search(
				request->getUser()->getProfile()->getRightsForModuleClass<ResaRight>()
				, request->getUser()->getProfile()->getGlobalPublicRight<ResaRight>() >= READ
				, READ
				, 0, 0
				, false, true, true, true
			));

			for (vector<shared_ptr<CommercialLine> >::const_iterator it(lines.begin()); it != lines.end(); ++it)
			{
				AdminInterfaceElement::PageLink link;
				link.factoryKey = BookableCommercialLineAdmin::FACTORY_KEY;
				link.icon = "chart_line.png";
				link.name = (*it)->getName();
				link.parameterName = QueryString::PARAMETER_OBJECT_ID;
				link.parameterValue = (*it)->getKey();
				links.push_back(link);
			}

			return links;
		}
	}
}
