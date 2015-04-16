
//////////////////////////////////////////////////////////////////////////
/// PTUseRulesAdmin class implementation.
///	@file PTUseRulesAdmin.cpp
///	@author Hugues Romain
///	@date 2010
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

#include "PTUseRulesAdmin.h"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "PTUseRuleTableSync.h"
#include "ActionResultHTMLTable.h"
#include "PTUseRuleUpdateAction.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "PTUseRuleAdmin.h"
#include "SearchFormHTMLTable.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, PTUseRulesAdmin>::FACTORY_KEY("PTUseRulesAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTUseRulesAdmin>::ICON("resa_optional.png");
		template<> const string AdminInterfaceElementTemplate<PTUseRulesAdmin>::DEFAULT_TITLE("Conditions de transport");
	}

	namespace pt
	{
		const string PTUseRulesAdmin::PARAMETER_SEARCH_NAME("na");



		PTUseRulesAdmin::PTUseRulesAdmin()
			: AdminInterfaceElementTemplate<PTUseRulesAdmin>()
		{ }



		void PTUseRulesAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_searchName = map.getDefault<string>(PARAMETER_SEARCH_NAME);

			// Search table initialization
			_requestParameters.setFromParametersMap(map, PARAMETER_SEARCH_NAME, 500);
		}



		ParametersMap PTUseRulesAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());

			if(_searchName.empty())
			{
				m.insert(PARAMETER_SEARCH_NAME, _searchName);
			}

			return m;
		}



		bool PTUseRulesAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void PTUseRulesAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{

			stream << "<h1>Conditions de transport</h1>";

			AdminActionFunctionRequest<PTUseRuleUpdateAction, PTUseRuleAdmin> creationRequest(_request);
			creationRequest.setActionWillCreateObject();
			creationRequest.setActionFailedPage(getNewCopiedPage());

			AdminFunctionRequest<PTUseRuleAdmin> openRequest(_request);

			AdminFunctionRequest<PTUseRulesAdmin> searchRequest(_request, *this);

			SearchFormHTMLTable st(searchRequest.getHTMLForm());
			stream << st.open();
			stream << st.cell("Nom", st.getForm().getTextInput(PARAMETER_SEARCH_NAME, _searchName));
			stream << st.close();

			PTUseRuleTableSync::SearchResult ptrules(
				PTUseRuleTableSync::Search(
					_getEnv(),
					"%"+ _searchName +"%",
					_requestParameters.first,
					_requestParameters.maxSize,
					_requestParameters.orderField == PARAMETER_SEARCH_NAME,
					_requestParameters.raisingOrder
			)	);

			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
			h.push_back(make_pair(string(),"Type"));
			h.push_back(make_pair(string(),"Actions"));
			h.push_back(make_pair(string(),"Actions"));

			ActionResultHTMLTable t(h, st.getForm(), _requestParameters, ptrules, creationRequest.getHTMLForm(), PTUseRuleUpdateAction::PARAMETER_TEMPLATE_ID);
			stream << t.open();

			BOOST_FOREACH(const boost::shared_ptr<PTUseRule>& ptrule, ptrules)
			{
				openRequest.getPage()->setRule(ptrule);
				stream << t.row(lexical_cast<string>(ptrule->getKey()));
				stream << t.col() << ptrule->getName();
				stream << t.col() << ptrule->getReservationType();
				stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "/admin/img/" + PTUseRuleAdmin::ICON);
				stream << t.col() << "Supprimer";
			}

			stream << t.row();
			stream << t.col(2) << t.getActionForm().getTextInput(PTUseRuleUpdateAction::PARAMETER_NAME, string());
			stream << t.col(2) << t.getActionForm().getSubmitButton("Ajouter");

			stream << t.close();
		}



		AdminInterfaceElement::PageLinks PTUseRulesAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& _request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const PTModule*>(&module) &&
				_request.getUser() &&
				_request.getUser()->getProfile() &&
				isAuthorized(*_request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}

			return links;
		}



		AdminInterfaceElement::PageLinks PTUseRulesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& _request
		) const	{

			AdminInterfaceElement::PageLinks links;

			PTUseRuleTableSync::SearchResult ptrules(PTUseRuleTableSync::Search(Env::GetOfficialEnv()));

			if(	currentPage == *this ||
				currentPage.getCurrentTreeBranch().find(*this)
			){
				BOOST_FOREACH(const boost::shared_ptr<PTUseRule>& ptrule, ptrules)
				{
					boost::shared_ptr<PTUseRuleAdmin> p(getNewPage<PTUseRuleAdmin>());
					p->setRule(ptrule);
					links.push_back(p);
			}	}

			return links;
		}
	}
}
