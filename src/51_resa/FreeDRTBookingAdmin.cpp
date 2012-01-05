
//////////////////////////////////////////////////////////////////////////
/// FreeDRTBookingAdmin class implementation.
///	@file FreeDRTBookingAdmin.cpp
///	@author Hugues Romain
///	@date 2012
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

#include "FreeDRTBookingAdmin.hpp"

#include "AdminParametersException.h"
#include "BookableCommercialLineAdmin.h"
#include "FreeDRTArea.hpp"
#include "FreeDRTTimeSlot.hpp"
#include "ParametersMap.h"
#include "ResaModule.h"
#include "ResaRight.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace resa;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, FreeDRTBookingAdmin>::FACTORY_KEY("FreeDRTBookingAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<FreeDRTBookingAdmin>::ICON = "resa_compulsory.png";
		template<> const string AdminInterfaceElementTemplate<FreeDRTBookingAdmin>::DEFAULT_TITLE = "Réservation TAD libéralisé";
	}

	namespace resa
	{
		const string FreeDRTBookingAdmin::PARAMETER_AREA_ID = "area_id";



		FreeDRTBookingAdmin::FreeDRTBookingAdmin()
			: AdminInterfaceElementTemplate<FreeDRTBookingAdmin>()
		{ }


		
		void FreeDRTBookingAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			// Area
			try
			{
				_area = Env::GetOfficialEnv().get<FreeDRTArea>(
					map.get<RegistryKeyType>(PARAMETER_AREA_ID)
				);
			}
			catch (ObjectNotFoundException<FreeDRTTimeSlot>&)
			{
				throw AdminParametersException("No such free DRT time slot");
			}
		}



		ParametersMap FreeDRTBookingAdmin::getParametersMap() const
		{
			ParametersMap m;

			// Time slot
			if(_area.get())
			{
				m.insert(PARAMETER_AREA_ID, _area->getKey());
			}

			return m;
		}


		
		bool FreeDRTBookingAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL);
		}



		void FreeDRTBookingAdmin::display(
			ostream& stream,
			const admin::AdminRequest& request
		) const	{

			
		}



		FreeDRTBookingAdmin::PageLinks FreeDRTBookingAdmin::_getCurrentTreeBranch() const
		{
			shared_ptr<BookableCommercialLineAdmin> p(
				getNewPage<BookableCommercialLineAdmin>()
			);
			p->setCommercialLine(Env::GetOfficialEnv().getSPtr(_area->getLine()));

			PageLinks links(p->_getCurrentTreeBranch());
			links.push_back(getNewCopiedPage());
			return links;

		}
}	}

