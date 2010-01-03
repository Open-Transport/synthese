
//////////////////////////////////////////////////////////////////////////
/// PTCitiesAdmin class implementation.
///	@file PTCitiesAdmin.cpp
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

#include "PTCitiesAdmin.h"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "GeographyModule.h"
#include "PropertiesHTMLTable.h"
#include "ResultHTMLTable.h"
#include "AdminFunctionRequest.hpp"
#include "PTPlacesAdmin.h"
#include "FrenchSentence.h"
#include "Profile.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace geography;
	using namespace html;
	using namespace lexmatcher;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, PTCitiesAdmin>::FACTORY_KEY("PTCitiesAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTCitiesAdmin>::ICON("building.png");
		template<> const string AdminInterfaceElementTemplate<PTCitiesAdmin>::DEFAULT_TITLE("Localités");
	}

	namespace pt
	{
		const string PTCitiesAdmin::PARAM_SEARCH_NAME("na");
		const string PTCitiesAdmin::TAB_LIST("li");
		const string PTCitiesAdmin::TAB_PHONETIC("ph");



		PTCitiesAdmin::PTCitiesAdmin()
			: AdminInterfaceElementTemplate<PTCitiesAdmin>()
		{ }


		
		void PTCitiesAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_searchName = map.getDefault<string>(PARAM_SEARCH_NAME);

			// Search table initialization
			_requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_NAME, 50);


		}



		ParametersMap PTCitiesAdmin::getParametersMap() const
		{
			ParametersMap m;
			m.insert(PARAM_SEARCH_NAME, _searchName);
			return m;
		}


		
		bool PTCitiesAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void PTCitiesAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const AdminRequest& request
		) const	{
		
			////////////////////////////////////////////////////////////////////
			// TAB LIST
			if (openTabContent(stream, TAB_LIST))
			{
				AdminFunctionRequest<PTCitiesAdmin> searchRequest(request);
			}


			////////////////////////////////////////////////////////////////////
			// TAB LIST
			if (openTabContent(stream, TAB_PHONETIC))
			{				
				AdminFunctionRequest<PTCitiesAdmin> searchRequest(request);

				stream << "<h1>Recherche</h1>";

				PropertiesHTMLTable t(searchRequest.getHTMLForm());
				stream << t.open();
				stream << t.cell("Nom", t.getForm().getTextInput(PARAM_SEARCH_NAME, _searchName));
				stream << t.close();

				if(!_searchName.empty())
				{
					AdminFunctionRequest<PTPlacesAdmin> openCityRequest(request);

					stream << "<h1>Resultat</h1>";

					stream << "<p>Phonétique du texte recherché : " << FrenchSentence(_searchName).getPhoneticString() << "</p>";

					HTMLTable::ColsVector c;
					c.push_back("Localité");
					c.push_back("Phonétique");
					c.push_back("Score");
					c.push_back("Actions");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();

					const GeographyModule::CitiesMatcher& matcher(GeographyModule::GetCitiesMatcher());
					GeographyModule::CitiesMatcher::MatchResult result(matcher.bestMatches(_searchName, 20));
										
					BOOST_FOREACH(const GeographyModule::CitiesMatcher::MatchHit& it, result)
					{
						openCityRequest.getPage()->setCity(Env::GetOfficialEnv().getSPtr(it.value));

						stream << t.row();
						stream << t.col() << it.key.getSource();
						stream << t.col() << it.key.getPhoneticString();
						stream << t.col() << it.score;
						stream << t.col() << HTMLModule::getLinkButton(openCityRequest.getURL(), "Ouvrir", string(), "building.png");
					}

					stream << t.close();
				}
			}

			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}



		AdminInterfaceElement::PageLinks PTCitiesAdmin::getSubPagesOfModule(
			const std::string& moduleKey,
			const AdminInterfaceElement& currentPage,
			const AdminRequest& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			if(	moduleKey == PTModule::FACTORY_KEY &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewPage());
			}
			
			return links;
		}


		
		AdminInterfaceElement::PageLinks PTCitiesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const AdminRequest& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;

			if(	currentPage.getCurrentTreeBranch().find(*this))
			{
				links.push_back(currentPage.getCurrentTreeBranch().getNextSubPage(*this));
			}
			
			return links;
		}



		void PTCitiesAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();

			_tabs.push_back(Tab("Recherche", TAB_LIST, true, "find.png"));
			_tabs.push_back(Tab("Recherche phonétique", TAB_PHONETIC, true, "text_allcaps.png"));

			_tabBuilded = true;
		}
	}
}
