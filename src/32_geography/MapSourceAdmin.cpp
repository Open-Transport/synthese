
//////////////////////////////////////////////////////////////////////////
/// MapSourceAdmin class implementation.
///	@file MapSourceAdmin.cpp
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

#include "MapSourceAdmin.hpp"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "PTModule.h"
#include "User.h"
#include "GlobalRight.h"
#include "MapSourceTableSync.hpp"
#include "PropertiesHTMLTable.h"
#include "MapSourceUpdateAction.hpp"
#include "AdminActionFunctionRequest.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace geography;
	using namespace html;


	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, MapSourceAdmin>::FACTORY_KEY("MapSource");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<MapSourceAdmin>::ICON("monitor.png");
		template<> const string AdminInterfaceElementTemplate<MapSourceAdmin>::DEFAULT_TITLE("MapSource");
	}

	namespace geography
	{
		MapSourceAdmin::MapSourceAdmin():
			AdminInterfaceElementTemplate<MapSourceAdmin>()
		{ }



		void MapSourceAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_mapSource = MapSourceTableSync::Get(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID), *_env);
			}
			catch(ObjectNotFoundException<MapSource>&)
			{
				throw AdminParametersException("No such map source");
			}
		}



		ParametersMap MapSourceAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_mapSource.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _mapSource->getKey());
			}
			return m;
		}



		bool MapSourceAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<GlobalRight>(READ);
		}



		void MapSourceAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			{
				stream << "<h1>Propriétés</h1>";

				AdminActionFunctionRequest<MapSourceUpdateAction, MapSourceAdmin> updateRequest(request, *this);
				updateRequest.getAction()->setMapSource(const_pointer_cast<MapSource>(_mapSource));

				PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
				stream << t.open();
				stream << t.cell("ID", lexical_cast<string>(_mapSource->getKey()));
				stream << t.cell("Nom", t.getForm().getTextInput(MapSourceUpdateAction::PARAMETER_NAME, _mapSource->getName()));
				stream << t.cell("URL", t.getForm().getTextInput(MapSourceUpdateAction::PARAMETER_URL, _mapSource->getURL()));
				stream << t.cell("Projection", t.getForm().getSelectInput(MapSourceUpdateAction::PARAMETER_SRID, CoordinatesSystem::GetCoordinatesSystemsTextMap(), optional<CoordinatesSystem::SRID>(_mapSource->getCoordinatesSystem().getSRID())));
				stream << t.cell("Type", t.getForm().getSelectInput(MapSourceUpdateAction::PARAMETER_TYPE, MapSource::GetTypesMap(), optional<MapSource::Type>(_mapSource->getType())));
				stream << t.close();
			}
		}



		std::string MapSourceAdmin::getTitle() const
		{
			return _mapSource.get() ? _mapSource->getName() : DEFAULT_TITLE;
		}



		bool MapSourceAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _mapSource->getKey() == static_cast<const MapSourceAdmin&>(other)._mapSource->getKey();
		}
	}
}
