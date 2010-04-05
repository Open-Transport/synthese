
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

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace pt;
	
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
		const string CommercialLineUpdateAction::PARAMETER_CREATOR_ID = Action_PARAMETER_PREFIX + "ci";

		
		
		ParametersMap CommercialLineUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_line.get())
			{
				map.insert(PARAMETER_LINE_ID, _line->getKey());
				map.insert(PARAMETER_COLOR, _color ? _color->toString() : string());
				map.insert(PARAMETER_CREATOR_ID, _creatorId);
				map.insert(PARAMETER_IMAGE, _image);
				map.insert(PARAMETER_LONG_NAME, _longName);
				if(_network.get())
				{
					map.insert(PARAMETER_NETWORK_ID, _network->getKey());
				}
				if(_reservationContact.get())
				{
					map.insert(PARAMETER_RESERVATION_CONTACT_ID, _reservationContact->getKey());
				}
				map.insert(PARAMETER_STYLE, _style);
			}
			return map;
		}
		
		
		
		void CommercialLineUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Line
			try
			{
				_line = CommercialLineTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_LINE_ID), *_env);
			}
			catch(ObjectNotFoundException<CommercialLine>& e)
			{
				throw ActionException("No such line");
			}

			// Network
			try
			{
				_network = TransportNetworkTableSync::Get(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID), *_env);
			}
			catch(ObjectNotFoundException<TransportNetwork>& e)
			{
				throw ActionException("No such network");
			}

			// Color
			string color(map.getDefault<string>(PARAMETER_COLOR));
			if(!color.empty())
			{
				_color = RGBColor(color);
			}

			// Creator ID
			_creatorId = map.getDefault<string>(PARAMETER_CREATOR_ID);

			// Image
			_image = map.getDefault<string>(PARAMETER_IMAGE);

			// Name
			_longName = map.getDefault<string>(PARAMETER_LONG_NAME);
			_shortName = map.getDefault<string>(PARAMETER_SHORT_NAME);
			_name = map.getDefault<string>(PARAMETER_NAME);

			// Style
			_style = map.getDefault<string>(PARAMETER_STYLE);

			// Reservation contact
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
		}
		
		
		
		void CommercialLineUpdateAction::run(
			Request& request
		){
			//stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
			
			_line->setColor(_color);
			_line->setCreatorId(_creatorId);
			_line->setImage(_image);
			_line->setLongName(_longName);
			_line->setName(_name);
			_line->setNetwork(_network.get());
			_line->setReservationContact(_reservationContact.get());
			_line->setShortName(_shortName);
			_line->setStyle(_style);
			
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
