
//////////////////////////////////////////////////////////////////////////
/// PTPhysicalStopAdmin class implementation.
///	@file PTPhysicalStopAdmin.cpp
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

#include "PTPhysicalStopAdmin.h"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "PropertiesHTMLTable.h"
#include "AdminActionFunctionRequest.hpp"
#include "PhysicalStop.h"
#include "PhysicalStopUpdateAction.h"
#include "PublicTransportStopZoneConnectionPlace.h"
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
		template<> const string FactorableTemplate<AdminInterfaceElement, PTPhysicalStopAdmin>::FACTORY_KEY("PTPhysicalStopAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTPhysicalStopAdmin>::ICON("building.png");
		template<> const string AdminInterfaceElementTemplate<PTPhysicalStopAdmin>::DEFAULT_TITLE("Arrêt physique");
	}

	namespace pt
	{
		const string PTPhysicalStopAdmin::TAB_LINKS("li");
		const string PTPhysicalStopAdmin::TAB_OPERATOR_CODES("oc");
		const string PTPhysicalStopAdmin::TAB_PROPERTIES("pr");



		PTPhysicalStopAdmin::PTPhysicalStopAdmin()
			: AdminInterfaceElementTemplate<PTPhysicalStopAdmin>()
		{ }


		
		void PTPhysicalStopAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_stop = Env::GetOfficialEnv().get<PhysicalStop>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch (ObjectNotFoundException<PhysicalStop>&)
			{
				throw AdminParametersException("No such physical stop");
			}
		}



		ParametersMap PTPhysicalStopAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_stop.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _stop->getKey());
			}
			return m;
		}


		
		bool PTPhysicalStopAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void PTPhysicalStopAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const admin::AdminRequest& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// PROPERTIES TAB
			if (openTabContent(stream, TAB_PROPERTIES))
			{
				AdminActionFunctionRequest<PhysicalStopUpdateAction, PTPhysicalStopAdmin> updateRequest(request);
				updateRequest.getAction()->setStop(const_pointer_cast<PhysicalStop>(_stop));
				
				PropertiesHTMLTable t(updateRequest.getHTMLForm());
				stream << t.open();
				stream << t.cell("Nom", t.getForm().getTextInput(PhysicalStopUpdateAction::PARAMETER_NAME, _stop->getName()));
				stream << t.cell("X", t.getForm().getTextInput(PhysicalStopUpdateAction::PARAMETER_X, lexical_cast<string>(_stop->getX())));
				stream << t.cell("Y", t.getForm().getTextInput(PhysicalStopUpdateAction::PARAMETER_Y, lexical_cast<string>(_stop->getY())));
				stream << t.cell("Code opérateur", t.getForm().getTextInput(PhysicalStopUpdateAction::PARAMETER_OPERATOR_CODE, _stop->getCodeBySource()));
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



		std::string PTPhysicalStopAdmin::getTitle() const
		{
			return _stop.get() ? _stop->getConnectionPlace()->getFullName() + "/" + _stop->getName() : DEFAULT_TITLE;
		}



		bool PTPhysicalStopAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _stop->getKey() == static_cast<const PTPhysicalStopAdmin&>(other)._stop->getKey();
		}



		void PTPhysicalStopAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();

			_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, profile.isAuthorized<TransportNetworkRight>(WRITE, UNKNOWN_RIGHT_LEVEL)));

			_tabBuilded = true;
		}



		AdminInterfaceElement::PageLinks PTPhysicalStopAdmin::_getCurrentTreeBranch() const
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
