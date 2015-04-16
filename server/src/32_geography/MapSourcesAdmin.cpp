
//////////////////////////////////////////////////////////////////////////
/// MapSourcesAdmin class implementation.
///	@file MapSourcesAdmin.cpp
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

#include "MapSourcesAdmin.hpp"

#include "GlobalRight.h"
#include "Profile.h"
#include "User.h"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "GeographyModule.h"
#include "MapSourceTableSync.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "HTMLModule.h"
#include "MapSourceAdmin.hpp"
#include "MapSourceUpdateAction.hpp"
#include "RemoveObjectAction.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace html;
	using namespace db;
	using namespace geography;


	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, MapSourcesAdmin>::FACTORY_KEY("MapSources");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<MapSourcesAdmin>::ICON("monitor.png");
		template<> const string AdminInterfaceElementTemplate<MapSourcesAdmin>::DEFAULT_TITLE("Fonds de carte");
	}

	namespace geography
	{
		const string MapSourcesAdmin::PARAM_SEARCH_NAME("name");



		MapSourcesAdmin::MapSourcesAdmin()
			: AdminInterfaceElementTemplate<MapSourcesAdmin>()
		{ }



		void MapSourcesAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_searchName = map.getOptional<string>(PARAM_SEARCH_NAME);

			// Search table initialization
			_requestParameters.setFromParametersMap(map, PARAM_SEARCH_NAME, 100);
		}



		ParametersMap MapSourcesAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());

			if(_searchName)
			{
				m.insert(PARAM_SEARCH_NAME, *_searchName);
			}

			return m;
		}



		bool MapSourcesAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<GlobalRight>(READ);
		}



		void MapSourcesAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			AdminFunctionRequest<MapSourceAdmin> openRequest(request);

			AdminActionFunctionRequest<RemoveObjectAction, MapSourcesAdmin> deleteRequest(request, *this);

			AdminActionFunctionRequest<MapSourceUpdateAction, MapSourceAdmin> addRequest(request);
			addRequest.setActionWillCreateObject();
			addRequest.setActionFailedPage<MapSourcesAdmin>();

			HTMLForm f(addRequest.getHTMLForm("add"));

			HTMLTable::ColsVector c;
			c.push_back("ID");
			c.push_back("Nom");
			c.push_back("Type");
			c.push_back("Projection");
			c.push_back("Action");
			c.push_back("Action");
			HTMLTable t(c, ResultHTMLTable::CSS_CLASS);

			stream << f.open();
			stream << t.open();

			MapSourceTableSync::SearchResult mapSources(
				MapSourceTableSync::Search(
					*_env,
					_searchName ? optional<string>("%"+ *_searchName +"%") : _searchName,
					_requestParameters.first,
					_requestParameters.maxSize,
					_requestParameters.orderField == PARAM_SEARCH_NAME,
					_requestParameters.raisingOrder
			)	);
			BOOST_FOREACH(const boost::shared_ptr<MapSource>& mapSource, mapSources)
			{
				// Row init
				stream << t.row();

				// ID
				stream << t.col() << mapSource->getKey();

				// Name
				stream << t.col() << mapSource->getName();

				// Type
				stream << t.col() << MapSource::GetTypeString(mapSource->getType());

				// Projection
				stream << t.col();
				try
				{
					stream << mapSource->getCoordinatesSystem().getName();
				}
				catch(MapSource::NoCoordinatesSystemException&)
				{
					stream << "(non défini)";
				}

				// Open button
				openRequest.getPage()->setMapSource(const_pointer_cast<const MapSource>(mapSource));
				stream << t.col();
				stream << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir");

				// Delete button
				deleteRequest.getAction()->setObjectId(mapSource->getKey());
				stream << t.col();
				stream << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le fond de carte ?");
			}

			// New destination form
			stream << t.row();
			stream << t.col();
			stream << t.col() << f.getTextInput(MapSourceUpdateAction::PARAMETER_NAME, string());
			stream << t.col() << f.getSelectInput(MapSourceUpdateAction::PARAMETER_TYPE, MapSource::GetTypesMap(), optional<MapSource::Type>());
			stream << t.col() << f.getSelectInput(MapSourceUpdateAction::PARAMETER_SRID, CoordinatesSystem::GetCoordinatesSystemsTextMap(), optional<CoordinatesSystem::SRID>());
			stream << t.col() << f.getSubmitButton("Ajouter");
			stream << t.col();

			stream << t.close();
			stream << f.close();
		}



		AdminInterfaceElement::PageLinks MapSourcesAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const GeographyModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}

			return links;
		}



		AdminInterfaceElement::PageLinks MapSourcesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			const MapSourcesAdmin* ua(
				dynamic_cast<const MapSourcesAdmin*>(&currentPage)
			);

			const MapSourceAdmin* ua2(
				dynamic_cast<const MapSourceAdmin*>(&currentPage)
			);

			if(ua || ua2)
			{

				MapSourceTableSync::SearchResult mapSources(
					MapSourceTableSync::Search(
						*_env
				)	);
				BOOST_FOREACH(const boost::shared_ptr<MapSource>& mapSource, mapSources)
				{
					boost::shared_ptr<MapSourceAdmin> p(getNewPage<MapSourceAdmin>());
					p->setMapSource(const_pointer_cast<const MapSource>(mapSource));
					links.push_back(p);
				}
			}

			return links;
		}
}	}
