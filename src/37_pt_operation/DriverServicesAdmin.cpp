
//////////////////////////////////////////////////////////////////////////
/// DriverServicesAdmin class implementation.
///	@file DriverServicesAdmin.cpp
///	@author Hugues Romain
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "DriverServicesAdmin.hpp"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "PTOperationModule.hpp"
#include "User.h"
#include "GlobalRight.h"
#include "RemoveObjectAction.hpp"
#include "DriverServiceTableSync.hpp"
#include "DriverService.hpp"
#include "CommercialLine.h"
#include "ScheduledService.h"
#include "JourneyPattern.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "ActionResultHTMLTable.h"
#include "DriverServiceAdmin.hpp"
#include "DriverServiceUpdateAction.hpp"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt_operation;
	using namespace db;
	using namespace pt;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, DriverServicesAdmin>::FACTORY_KEY("DriverServices");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DriverServicesAdmin>::ICON("user.png");
		template<> const string AdminInterfaceElementTemplate<DriverServicesAdmin>::DEFAULT_TITLE("Services agents");
	}

	namespace pt_operation
	{
		const string DriverServicesAdmin::PARAMETER_SEARCH_NAME("search_name");



		DriverServicesAdmin::DriverServicesAdmin()
			: AdminInterfaceElementTemplate<DriverServicesAdmin>()
		{ }



		void DriverServicesAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_searchName = map.getOptional<string>(PARAMETER_SEARCH_NAME);

			// Search table initialization
			_requestParameters.setFromParametersMap(map, PARAMETER_SEARCH_NAME, 100);
		}



		ParametersMap DriverServicesAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());

			if(_searchName)
			{
				m.insert(PARAMETER_SEARCH_NAME, *_searchName);
			}

			return m;
		}



		bool DriverServicesAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<GlobalRight>(READ);
		}



		void DriverServicesAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			AdminFunctionRequest<DriverServiceAdmin> openRequest(request);

			AdminFunctionRequest<DriverServicesAdmin> searchRequest(request, *this);

			AdminActionFunctionRequest<DriverServiceUpdateAction, DriverServiceAdmin> createRequest(request);
			createRequest.setActionFailedPage<DriverServicesAdmin>();
			createRequest.setActionWillCreateObject();

			AdminActionFunctionRequest<RemoveObjectAction, DriverServicesAdmin> removeRequest(request, *this);

			// Search
			DriverServiceTableSync::SearchResult driverServices(
				DriverServiceTableSync::Search(
					Env::GetOfficialEnv(),
					_searchName,
					_requestParameters.first,
					_requestParameters.maxSize,
					_requestParameters.orderField == PARAMETER_SEARCH_NAME,
					_requestParameters.raisingOrder
			)	);

			ActionResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(string(), string()));
			h.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
			h.push_back(make_pair(string(), "Lignes"));
			h.push_back(make_pair(string(), "Code opérateur"));
			h.push_back(make_pair(string(), string()));

			ActionResultHTMLTable t(
				h,
				searchRequest.getHTMLForm(),
				_requestParameters,
				driverServices,
				createRequest.getHTMLForm("add")
			);

			stream << t.open();

			BOOST_FOREACH(const boost::shared_ptr<DriverService>& driverService, driverServices)
			{
				stream << t.row(lexical_cast<string>(driverService->getKey()));

				// Link
				stream << t.col();
				openRequest.getPage()->setDriverService(const_pointer_cast<const DriverService>(driverService));
				stream << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "/admin/img/" + DriverServiceAdmin::ICON);

				// Name
				stream << t.col() << driverService->getName();

				// Lines
				stream << t.col();
				set<CommercialLine*> lines;
				BOOST_FOREACH(const DriverService::Chunk& chunk, driverService->getChunks())
				{
					BOOST_FOREACH(const DriverService::Chunk::Element& element, chunk.elements)
					{
						if(!dynamic_cast<ScheduledService*>(element.service))
						{
							continue;
						}
						lines.insert(static_cast<ScheduledService*>(element.service)->getRoute()->getCommercialLine());
				}	}
				BOOST_FOREACH(CommercialLine* line, lines)
				{
					stream << "<span class=\"line " << line->getStyle() << "\">" << line->getShortName() << "</span>";
				}

				// Operator code
				stream << t.col() << driverService->getCodeBySources();

				// Delete link
				stream << t.col();
				removeRequest.getAction()->setObjectId(driverService->getKey());
				stream << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le service agent "+ driverService->getName() + " ?");
			}

			stream << t.row(string());
			stream << t.col() << t.getActionForm().getTextInput(DriverServiceUpdateAction::PARAMETER_NAME, "", "Entrez le nom du service agent ici");
			stream << t.col();
			stream << t.col();
			stream << t.col();
			stream << t.col() << t.getActionForm().getSubmitButton("Ajouter");
			stream << t.col();
			stream << t.close();
		}



		AdminInterfaceElement::PageLinks DriverServicesAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const PTOperationModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}

			return links;
		}



		AdminInterfaceElement::PageLinks DriverServicesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	currentPage == *this ||
				currentPage.getCurrentTreeBranch().find(*this)
			){
				BOOST_FOREACH(const DriverService::Registry::value_type& service, Env::GetOfficialEnv().getRegistry<DriverService>())
				{
					boost::shared_ptr<DriverServiceAdmin> p(
						getNewPage<DriverServiceAdmin>()
					);
					p->setDriverService(service.second);
					links.push_back(p);
				}
			}
			return links;
		}
}	}
