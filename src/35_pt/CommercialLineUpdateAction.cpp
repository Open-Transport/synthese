
//////////////////////////////////////////////////////////////////////////
/// CommercialLineUpdateAction class implementation.
/// @file CommercialLineUpdateAction.cpp
/// @author Hugues Romain
/// @date 2010
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "CommercialLineUpdateAction.h"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "CommercialLineTableSync.h"
#include "TransportNetworkTableSync.h"
#include "ReservationContactTableSync.h"
#include "CommercialLineTableSync.h"
#include "ImportableTableSync.hpp"
#include "ImportableAdmin.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace pt;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::CommercialLineUpdateAction>::FACTORY_KEY("CommercialLineUpdateAction");
	}

	namespace pt
	{
		const string CommercialLineUpdateAction::PARAMETER_LINE_ID = Action_PARAMETER_PREFIX + "id";
		const string CommercialLineUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string CommercialLineUpdateAction::PARAMETER_SHORT_NAME = Action_PARAMETER_PREFIX + "sn";
		const string CommercialLineUpdateAction::PARAMETER_LONG_NAME = Action_PARAMETER_PREFIX + "ln";
		const string CommercialLineUpdateAction::PARAMETER_COLOR = Action_PARAMETER_PREFIX + "co";
		const string CommercialLineUpdateAction::PARAMETER_STYLE = Action_PARAMETER_PREFIX + "st";
		const string CommercialLineUpdateAction::PARAMETER_IMAGE = Action_PARAMETER_PREFIX + "im";
		const string CommercialLineUpdateAction::PARAMETER_NETWORK_ID = Action_PARAMETER_PREFIX + "ni";
		const string CommercialLineUpdateAction::PARAMETER_RESERVATION_CONTACT_ID = Action_PARAMETER_PREFIX + "ri";



		ParametersMap CommercialLineUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_line.get())
			{
				map.insert(PARAMETER_LINE_ID, _line->getKey());
			}
			if(_color)
			{
				map.insert(PARAMETER_COLOR, *_color ? (*_color)->toXMLColor() : string());
			}
			if(_dataSourceLinks)
			{
				map.insert(ImportableAdmin::PARAMETER_DATA_SOURCE_LINKS, ImportableTableSync::SerializeDataSourceLinks(*_dataSourceLinks));
			}
			if(_image)
			{
				map.insert(PARAMETER_IMAGE, *_image);
			}
			if(_longName)
			{
				map.insert(PARAMETER_LONG_NAME, *_longName);
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			if(_shortName)
			{
				map.insert(PARAMETER_SHORT_NAME, *_shortName);
			}
			if(_network)
			{
				map.insert(PARAMETER_NETWORK_ID, _network->get() ? (*_network)->getKey() : 0);
			}
			if(_reservationContact)
			{
				map.insert(PARAMETER_RESERVATION_CONTACT_ID, _reservationContact.get() ? (*_reservationContact)->getKey() : 0);
			}
			if(_style)
			{
				map.insert(PARAMETER_STYLE, *_style);
			}
			return map;
		}



		void CommercialLineUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// JourneyPattern
			try
			{
				_line = CommercialLineTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_LINE_ID), *_env);
			}
			catch(ObjectNotFoundException<CommercialLine>&)
			{
				throw ActionException("No such line");
			}

			// Network
			if(map.isDefined(PARAMETER_NETWORK_ID))	try
			{
				_network = TransportNetworkTableSync::Get(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID), *_env);
			}
			catch(ObjectNotFoundException<TransportNetwork>&)
			{
				throw ActionException("No such network");
			}

			// Color
			if(map.isDefined(PARAMETER_COLOR))
			{
				string color(map.getDefault<string>(PARAMETER_COLOR));
				_color = color.empty() ? optional<RGBColor>() : RGBColor::FromXMLColor(color);
			}

			// Creator ID
			if(map.isDefined(ImportableAdmin::PARAMETER_DATA_SOURCE_LINKS))
			{
				_dataSourceLinks = ImportableTableSync::GetDataSourceLinksFromSerializedString(
					map.get<string>(ImportableAdmin::PARAMETER_DATA_SOURCE_LINKS),
					*_env
				);
			}

			// Image
			if(map.isDefined(PARAMETER_IMAGE))
			{
				_image = map.get<string>(PARAMETER_IMAGE);
			}

			// Name
			if(map.isDefined(PARAMETER_LONG_NAME))
			{
				_longName = map.get<string>(PARAMETER_LONG_NAME);
			}

			if(map.isDefined(PARAMETER_SHORT_NAME))
			{
				_shortName = map.get<string>(PARAMETER_SHORT_NAME);
			}

			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
			}

			// Style
			if(map.isDefined(PARAMETER_STYLE))
			{
				_style = map.get<string>(PARAMETER_STYLE);
			}

			// Reservation contact
			if(map.isDefined(PARAMETER_RESERVATION_CONTACT_ID))
			{
				RegistryKeyType rid(map.getDefault<RegistryKeyType>(PARAMETER_RESERVATION_CONTACT_ID, 0));
				if(rid > 0)
				{
					try
					{
						_reservationContact = ReservationContactTableSync::Get(rid, *_env);
					}
					catch (ObjectNotFoundException<ReservationContact>&)
					{
						throw ActionException("No such reservation contact");
					}
				}
				else
				{
					_reservationContact = shared_ptr<const ReservationContact>();
				}
			}
		}



		void CommercialLineUpdateAction::run(
			Request& request
		){
			//stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			if(_color)
			{
				_line->setColor(*_color);
			}
			if(_dataSourceLinks)
			{
				_line->setDataSourceLinks(*_dataSourceLinks);
			}
			if(_image)
			{
				_line->setImage(*_image);
			}
			if(_longName)
			{
				_line->setLongName(*_longName);
			}
			if(_name)
			{
				_line->setName(*_name);
			}
			if(_network)
			{
				_line->setNetwork(_network->get());
			}
			if(_reservationContact)
			{
				_line->setReservationContact(_reservationContact->get());
			}
			if(_shortName)
			{
				_line->setShortName(*_shortName);
			}
			if(_style)
			{
				_line->setStyle(*_style);
			}

			CommercialLineTableSync::Save(_line.get());

			// ::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool CommercialLineUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
