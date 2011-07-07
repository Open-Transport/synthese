////////////////////////////////////////////////////////////////////////////////
/// TransportNetworkAdmin class implementation.
///	@file TransportNetworkAdmin.cpp
///	@author Hugues Romain
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

#include "TransportNetworkAdmin.h"
#include "PTModule.h"
#include "Profile.h"
#include "AdminFunctionRequest.hpp"
#include "TransportNetwork.h"
#include "TransportNetworkTableSync.h"
#include "CommercialLine.h"
#include "CommercialLineAdmin.h"
#include "CommercialLineTableSync.h"
#include "TransportNetworkRight.h"
#include "AdminInterfaceElement.h"
#include "ModuleAdmin.h"
#include "AdminParametersException.h"
#include "ScheduledService.h"
#include "SearchFormHTMLTable.h"
#include "AdminActionFunctionRequest.hpp"
#include "CommercialLineAddAction.h"
#include "ImportableAdmin.hpp"
#include "TransportNetworkUpdateAction.hpp"
#include "PropertiesHTMLTable.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace pt;
	using namespace security;
	using namespace html;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, TransportNetworkAdmin>::FACTORY_KEY("TransportNetworkAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<TransportNetworkAdmin>::ICON("chart_organisation.png");
		template<> const string AdminInterfaceElementTemplate<TransportNetworkAdmin>::DEFAULT_TITLE("Réseau inconnu");
	}

	namespace pt
	{
		const string TransportNetworkAdmin::PARAMETER_SEARCH_NAME("sn");
		const string TransportNetworkAdmin::PARAMETER_SEARCH_NETWORK_ID("sw");


		void TransportNetworkAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_searchName = map.getDefault<string>(PARAMETER_SEARCH_NAME);
			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_SEARCH_NAME, 100);

			try
			{
				_network = TransportNetworkTableSync::Get(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
					Env::GetOfficialEnv(),
					UP_LINKS_LOAD_LEVEL
				);
			}
			catch (...)
			{
				throw AdminParametersException("No such network");
			}
		}



		util::ParametersMap TransportNetworkAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			m.insert(PARAMETER_SEARCH_NAME,_searchName);
			if(_network.get()) m.insert(Request::PARAMETER_OBJECT_ID, _network->getKey());
			return m;
		}



		void TransportNetworkAdmin::display(
			ostream& stream,
			const admin::AdminRequest& _request
		) const	{

			// Search form
			stream << "<h1>Recherche</h1>";
			AdminFunctionRequest<TransportNetworkAdmin> searchRequest(_request);
			HTMLForm sortedForm(searchRequest.getHTMLForm("search"));

			getHTMLLineSearchForm(
				stream,
				sortedForm,
				optional<RegistryKeyType>(),
				_searchName
			);

			// Results display
			stream << "<h1>Lignes du réseau</h1>";

			CommercialLineTableSync::SearchResult lines(
				CommercialLineTableSync::Search(
					Env::GetOfficialEnv(),
					_network->getKey(),
					string("%"+_searchName+"%"),
					optional<string>(),
					_requestParameters.first,
					_requestParameters.maxSize,
					false,
					_requestParameters.orderField == PARAMETER_SEARCH_NAME,
					_requestParameters.raisingOrder
			)	);

			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(string(), "N°"));
			h.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
			h.push_back(make_pair(string(), "Actions"));
			ResultHTMLTable t(h,sortedForm,_requestParameters, lines);

			stream << t.open();
			AdminFunctionRequest<CommercialLineAdmin> lineOpenRequest(_request);
			BOOST_FOREACH(shared_ptr<CommercialLine> line, lines)
			{
				lineOpenRequest.getPage()->setCommercialLine(line);
				stream << t.row();
				stream << t.col(1, line->getStyle(), true);
				stream << line->getShortName();
				stream << t.col();
				stream << line->getName();
				stream << t.col();
				stream << HTMLModule::getLinkButton(lineOpenRequest.getURL(), "Ouvrir", string(), "chart_line_edit.png");
			}

			AdminActionFunctionRequest<CommercialLineAddAction,CommercialLineAdmin> creationRequest(_request);
			creationRequest.getFunction()->setActionFailedPage(getNewCopiedPage());
			creationRequest.setActionWillCreateObject();
			creationRequest.getAction()->setNetwork(_network);

			stream << t.row();
			stream << t.col(2) << "Création de ligne";
			stream << t.col() << HTMLModule::getLinkButton(creationRequest.getURL(), "Créer");
			stream << t.close();

			// Properties
			stream << "<h1>Propriétés</h1>";
			AdminActionFunctionRequest<TransportNetworkUpdateAction,TransportNetworkAdmin> updateRequest(_request);
			updateRequest.getAction()->setNetwork(const_pointer_cast<TransportNetwork>(_network));
			PropertiesHTMLTable p(updateRequest.getHTMLForm("update"));
			stream << p.open();
			stream << p.cell("ID", lexical_cast<string>(_network->getKey()));
			stream << p.cell("Nom", p.getForm().getTextInput(TransportNetworkUpdateAction::PARAMETER_NAME, _network->getName()));
			stream << p.close();

			// Source id
			StaticActionRequest<TransportNetworkUpdateAction> updateOnlyRequest(_request);
			updateOnlyRequest.getAction()->setNetwork(const_pointer_cast<TransportNetwork>(_network));
			ImportableAdmin::DisplayDataSourcesTab(stream, *_network, updateOnlyRequest);
		}

		bool TransportNetworkAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		std::string TransportNetworkAdmin::getTitle() const
		{
			return _network.get() ? _network->getName() : DEFAULT_TITLE;
		}


		AdminInterfaceElement::PageLinks TransportNetworkAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			if(	currentPage == *this ||
				currentPage.getCurrentTreeBranch().find(*this)
			){
				CommercialLineTableSync::SearchResult lines(
					CommercialLineTableSync::Search(
						Env::GetOfficialEnv(),
						_network->getKey(),
						optional<string>(),
						optional<string>(),
						0,
						optional<size_t>(),
						true, false, true,
						UP_LINKS_LOAD_LEVEL
				)	);
				BOOST_FOREACH(shared_ptr<CommercialLine> line, lines)
				{
					shared_ptr<CommercialLineAdmin> p(
						getNewPage<CommercialLineAdmin>()
					);
					p->setCommercialLine(line);
					links.push_back(p);
				}
			}
			return links;
		}


		bool TransportNetworkAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _network->getKey() == static_cast<const TransportNetworkAdmin&>(other)._network->getKey();
		}



		void TransportNetworkAdmin::setNetwork( boost::shared_ptr<const pt::TransportNetwork> value )
		{
			_network = value;
		}



		void TransportNetworkAdmin::getHTMLLineSearchForm(
			std::ostream& stream,
			const html::HTMLForm& form,
			boost::optional<util::RegistryKeyType> networkId,
			boost::optional<const std::string&> lineName
		){

			SearchFormHTMLTable s(form);
			stream << s.open();
			if(networkId)
			{
				stream << s.cell("Réseau", s.getForm().getTextInput(PARAMETER_SEARCH_NETWORK_ID, lexical_cast<string>(*networkId)));
			}
			if(lineName)
			{
				stream << s.cell("Nom", s.getForm().getTextInput(PARAMETER_SEARCH_NAME, *lineName));
			}
			HTMLForm sortedForm(s.getForm());
			stream << s.close();

		}
	}
}
