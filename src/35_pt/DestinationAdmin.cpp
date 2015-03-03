
//////////////////////////////////////////////////////////////////////////
/// DestinationAdmin class implementation.
///	@file DestinationAdmin.cpp
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

#include "DestinationAdmin.hpp"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "DestinationTableSync.hpp"
#include "PropertiesHTMLTable.h"
#include "DestinationUpdateAction.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "StaticActionRequest.h"
#include "ImportableAdmin.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace impex;
	using namespace html;


	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, DestinationAdmin>::FACTORY_KEY("DestinationAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DestinationAdmin>::ICON("monitor.png");
		template<> const string AdminInterfaceElementTemplate<DestinationAdmin>::DEFAULT_TITLE("Destination");
	}

	namespace pt
	{
		DestinationAdmin::DestinationAdmin()
			: AdminInterfaceElementTemplate<DestinationAdmin>()
		{ }



		void DestinationAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_destination = DestinationTableSync::Get(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID), *_env);
			}
			catch(ObjectNotFoundException<Destination>&)
			{
				throw AdminParametersException("No such destination");
			}
		}



		ParametersMap DestinationAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_destination.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _destination->getKey());
			}
			return m;
		}



		bool DestinationAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void DestinationAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			{
				stream << "<h1>Propriétés</h1>";

				AdminActionFunctionRequest<DestinationUpdateAction, DestinationAdmin> updateRequest(request);
				updateRequest.getAction()->setDestination(const_pointer_cast<Destination>(_destination));

				PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
				stream << t.open();
				stream << t.cell("ID", lexical_cast<string>(_destination->getKey()));
				stream << t.cell("Texte affiché", t.getForm().getTextInput(DestinationUpdateAction::PARAMETER_DISPLAYED_TEXT, _destination->get<DisplayedText>()));
				stream << t.cell("Texte lu", t.getForm().getTextInput(DestinationUpdateAction::PARAMETER_TTS_TEXT, _destination->get<TtsText>()));
				stream << t.cell("Commentaire", t.getForm().getTextInput(DestinationUpdateAction::PARAMETER_COMMENT, _destination->get<Comment>()));
				stream << t.close();
			}

			{
				StaticActionRequest<DestinationUpdateAction> updateOnlyRequest(request);
				updateOnlyRequest.getAction()->setDestination(const_pointer_cast<Destination>(_destination));
				ImportableAdmin::DisplayDataSourcesTab(stream, *_destination, updateOnlyRequest);
			}
		}



		std::string DestinationAdmin::getTitle() const
		{
			return _destination.get() ? _destination->get<DisplayedText>() : DEFAULT_TITLE;
		}



		bool DestinationAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _destination->getKey() == static_cast<const DestinationAdmin&>(other)._destination->getKey();
		}
	}
}
