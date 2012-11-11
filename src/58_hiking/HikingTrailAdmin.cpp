
//////////////////////////////////////////////////////////////////////////
/// HikingTrailAdmin class implementation.
///	@file HikingTrailAdmin.cpp
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

#include "HikingTrailAdmin.h"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "User.h"
#include "HikingModule.h"
#include "HikingRight.h"
#include "HTMLModule.h"
#include "StopArea.hpp"
#include "City.h"
#include "AdminActionFunctionRequest.hpp"
#include "HikingTrailUpdateAction.h"
#include "PropertiesHTMLTable.h"
#include "HikingTrail.h"
#include "HikingTrailStopAddAction.hpp"
#include "HikingTrailStopRemoveAction.hpp"
#include "ResultHTMLTable.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace hiking;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, HikingTrailAdmin>::FACTORY_KEY("HikingTrailAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<HikingTrailAdmin>::ICON("map.png");
		template<> const string AdminInterfaceElementTemplate<HikingTrailAdmin>::DEFAULT_TITLE("Randonnée");
	}

	namespace hiking
	{
		const string HikingTrailAdmin::TAB_PROPERTIES;

		HikingTrailAdmin::HikingTrailAdmin()
			: AdminInterfaceElementTemplate<HikingTrailAdmin>()
		{ }



		void HikingTrailAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_trail = Env::GetOfficialEnv().get<HikingTrail>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
		}



		ParametersMap HikingTrailAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_trail.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _trail->getKey());
			}
			return m;
		}



		bool HikingTrailAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<HikingRight>(READ);
		}



		void HikingTrailAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{

			////////////////////////////////////////////////////////////////////
			// PROPERTIES TAB
			if (openTabContent(stream, TAB_PROPERTIES))
			{
				stream << "<h1>Propriétés</h1>";
				{
					AdminActionFunctionRequest<HikingTrailUpdateAction,HikingTrailAdmin> updateRequest(_request, *this);
					updateRequest.getAction()->setTrail(const_pointer_cast<HikingTrail>(_trail));

					PropertiesHTMLTable t(updateRequest.getHTMLForm());
					stream << t.open();
					stream << t.cell("ID", lexical_cast<string>(_trail->getKey()));
					stream << t.cell("Nom", t.getForm().getTextInput(HikingTrailUpdateAction::PARAMETER_NAME, _trail->getName()));
					stream << t.cell("URL durées", t.getForm().getTextInput(HikingTrailUpdateAction::PARAMETER_DURATION, _trail->getDuration()));
					stream << t.cell("URL carte", t.getForm().getTextInput(HikingTrailUpdateAction::PARAMETER_MAP, _trail->getMap()));
					stream << t.cell("URL profil", t.getForm().getTextInput(HikingTrailUpdateAction::PARAMETER_PROFILE, _trail->getProfile()));
					stream << t.cell("URL lien", t.getForm().getTextInput(HikingTrailUpdateAction::PARAMETER_URL, _trail->getURL()));
					stream << t.close();

				}

				stream << "<h1>Arrêts</h1>";
				{
					AdminActionFunctionRequest<HikingTrailStopAddAction,HikingTrailAdmin> addRequest(_request, *this);
					addRequest.getAction()->setTrail(const_pointer_cast<HikingTrail>(_trail));

					AdminActionFunctionRequest<HikingTrailStopRemoveAction,HikingTrailAdmin> removeRequest(_request, *this);
					removeRequest.getAction()->setTrail(const_pointer_cast<HikingTrail>(_trail));

					HTMLForm f(addRequest.getHTMLForm());
					stream << f.open();

					HTMLTable::ColsVector c;
					c.push_back("Rang");
					c.push_back("Commune");
					c.push_back("Arrêt");
					c.push_back("Action");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();
					size_t rank(0);
					BOOST_FOREACH(HikingTrail::Stops::value_type stop, _trail->getStops())
					{
						removeRequest.getAction()->setRank(rank);

						stream << t.row();
						stream << t.col() << f.getRadioInput(HikingTrailStopAddAction::PARAMETER_RANK, optional<size_t>(rank), optional<size_t>(), string(), false);
						stream << t.col() << rank++;
						stream << t.col() << stop->getCity()->getName();
						stream << t.col() << stop->getName();
						stream << t.col() << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer l'arrêt ?");
					}
					stream << t.row();
					stream << t.col() << f.getRadioInput(HikingTrailStopAddAction::PARAMETER_RANK, optional<size_t>(rank), optional<size_t>(), string(), false);
					stream << t.col() << rank;
					stream << t.col() << f.getTextInput(HikingTrailStopAddAction::PARAMETER_CITY, string());
					stream << t.col() << f.getTextInput(HikingTrailStopAddAction::PARAMETER_NAME, string());
					stream << t.col() << f.getSubmitButton("Ajouter");
					stream << t.close() << f.close();
				}
			}

			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		std::string HikingTrailAdmin::getTitle() const
		{
			return _trail.get() ? _trail->getName() : DEFAULT_TITLE;
		}



		bool HikingTrailAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _trail == static_cast<const HikingTrailAdmin&>(other)._trail;
		}



		void HikingTrailAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();

			_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, profile.isAuthorized<HikingRight>(WRITE, UNKNOWN_RIGHT_LEVEL)));

			_tabBuilded = true;
		}
	}
}
