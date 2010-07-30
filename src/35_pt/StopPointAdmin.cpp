
//////////////////////////////////////////////////////////////////////////
/// StopPointAdmin class implementation.
///	@file StopPointAdmin.cpp
///	@author Hugues Romain
///	@date 2010
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

#include "StopPointAdmin.hpp"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "PropertiesHTMLTable.h"
#include "AdminActionFunctionRequest.hpp"
#include "StopPoint.hpp"
#include "StopPointUpdateAction.hpp"
#include "StopArea.hpp"
#include "PTPlaceAdmin.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace html;
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, StopPointAdmin>::FACTORY_KEY("StopPointAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<StopPointAdmin>::ICON("building.png");
		template<> const string AdminInterfaceElementTemplate<StopPointAdmin>::DEFAULT_TITLE("Arrêt physique");
	}

	namespace pt
	{
		const string StopPointAdmin::TAB_LINKS("li");
		const string StopPointAdmin::TAB_OPERATOR_CODES("oc");
		const string StopPointAdmin::TAB_PROPERTIES("pr");



		StopPointAdmin::StopPointAdmin()
			: AdminInterfaceElementTemplate<StopPointAdmin>()
		{ }


		
		void StopPointAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_stop = Env::GetOfficialEnv().get<StopPoint>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch (ObjectNotFoundException<StopPoint>&)
			{
				throw AdminParametersException("No such physical stop");
			}
		}



		ParametersMap StopPointAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_stop.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _stop->getKey());
			}
			return m;
		}


		
		bool StopPointAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void StopPointAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const admin::AdminRequest& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// PROPERTIES TAB
			if (openTabContent(stream, TAB_PROPERTIES))
			{
				AdminActionFunctionRequest<StopPointUpdateAction, StopPointAdmin> updateRequest(request);
				updateRequest.getAction()->setStop(const_pointer_cast<StopPoint>(_stop));
				
				PropertiesHTMLTable t(updateRequest.getHTMLForm());
				stream << t.open();
				stream << t.cell("Nom", t.getForm().getTextInput(StopPointUpdateAction::PARAMETER_NAME, _stop->getName()));
				stream << t.cell("X", t.getForm().getTextInput(StopPointUpdateAction::PARAMETER_X, lexical_cast<string>(_stop->getX())));
				stream << t.cell("Y", t.getForm().getTextInput(StopPointUpdateAction::PARAMETER_Y, lexical_cast<string>(_stop->getY())));
				stream << t.cell("Code opérateur", t.getForm().getTextInput(StopPointUpdateAction::PARAMETER_OPERATOR_CODE, _stop->getCodeBySource()));
				stream << t.close();
			}


			////////////////////////////////////////////////////////////////////
			// OPERATOR CODES TAB
			if (openTabContent(stream, TAB_OPERATOR_CODES))
			{
			}
		
			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		std::string StopPointAdmin::getTitle() const
		{
			return
				_stop.get() ?
				(_stop->getName().empty() ?
					lexical_cast<string>(_stop->getKey()) :
					_stop->getName()) :
				DEFAULT_TITLE
			;
		}



		bool StopPointAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _stop->getKey() == static_cast<const StopPointAdmin&>(other)._stop->getKey();
		}



		void StopPointAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();

			_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, profile.isAuthorized<TransportNetworkRight>(WRITE, UNKNOWN_RIGHT_LEVEL)));

			_tabBuilded = true;
		}



		AdminInterfaceElement::PageLinks StopPointAdmin::_getCurrentTreeBranch() const
		{
			PageLinks links;

			shared_ptr<PTPlaceAdmin> p(getNewOtherPage<PTPlaceAdmin>(false));
			p->setConnectionPlace(Env::GetOfficialEnv().getSPtr(
					_stop->getConnectionPlace()
			)	);
			links = p->_getCurrentTreeBranch();
			links.push_back(getNewPage());

			return links;

		}
	}
}
