
/** VehicleAlarmRecipient class implementation.
	@file VehicleAlarmRecipient.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "VehicleAlarmRecipient.hpp"

#include "ResultHTMLTable.h"
#include "SecurityConstants.hpp"
#include "User.h"
#include "Profile.h"
#include "HTMLList.h"
#include "Vehicle.hpp"
#include "VehicleTableSync.hpp"
#include "PTModule.h"
#include "Alarm.h"
#include "AlarmObjectLink.h"
#include "AlarmObjectLinkTableSync.h"
#include "AlarmRemoveLinkAction.h"
#include "AlarmAddLinkAction.h"
#include "MessagesRight.h"
#include "AlarmObjectLinkException.h"
#include "AdminModule.h"
#include "PTPlacesAdmin.h"
#include "TransportNetwork.h"
#include "ImportableTableSync.hpp"
#include "TransportNetworkRight.h"

#include <boost/foreach.hpp>
#include <vector>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace messages;
	using namespace util;
	using namespace server;
	using namespace pt;
	using namespace admin;
	using namespace html;
	using namespace security;
	using namespace impex;
	using namespace vehicle;
	

	namespace util
	{
		template<> const std::string FactorableTemplate<AlarmRecipient, VehicleAlarmRecipient>::FACTORY_KEY("vehicle");
	}

	namespace messages
	{
	    /* IMPORTANT : for some reason, probably a gcc bug, is was necessary to
	       explicitly call constructor with () in order to avoid undefined references.
	       This should be investigate further.
	    */
		template<>
		AlarmRecipient::ObjectLinks	AlarmRecipientTemplate<VehicleAlarmRecipient>::_linksObject = AlarmRecipient::ObjectLinks();

		template<> const string AlarmRecipientTemplate<VehicleAlarmRecipient>::TITLE("Véhicules");

		template<> void AlarmRecipientTemplate<VehicleAlarmRecipient>::GetParametersLabels(
			ParameterLabelsVector& m
		){
			m.push_back(make_pair(FACTORY_KEY +"/"+ GLOBAL_PERIMETER,"(tous les véhicules)"));
		}



		template<>
		util::RegistryKeyType AlarmRecipientTemplate<VehicleAlarmRecipient>::GetObjectIdBySource(
			const impex::DataSource& source,
			const std::string& key,
			util::Env& env
		){
			ImportableTableSync::ObjectBySource<VehicleTableSync> vehicles(source, env);
			ImportableTableSync::ObjectBySource<VehicleTableSync>::Set vehiclesSet(vehicles.get(key));
			if(vehiclesSet.empty())
			{
				throw Exception("No such vehicle");
			}
			return (*vehiclesSet.begin())->getKey();
		}

	}


	namespace vehicle
	{
		void VehicleAlarmRecipient::displayBroadcastListEditor(
			std::ostream& stream,
			const messages::Alarm* alarm,
			const util::ParametersMap& parameters,
			Request& searchRequest,
			StaticActionRequest<messages::AlarmAddLinkAction>& addRequest,
			StaticActionRequest<messages::AlarmRemoveLinkAction>& removeRequest
		){
		}



		AlarmRecipientSearchFieldsMap VehicleAlarmRecipient::getSearchFields(HTMLForm& form, const ParametersMap& parameters) const
		{
			AlarmRecipientSearchFieldsMap map;
			return map;
		}



		boost::shared_ptr<security::Right> VehicleAlarmRecipient::getRight( const std::string& perimeter ) const
		{
			TransportNetworkRight* result(new TransportNetworkRight);
			result->setParameter(perimeter);
			return shared_ptr<Right>(result);
		}
}	}
