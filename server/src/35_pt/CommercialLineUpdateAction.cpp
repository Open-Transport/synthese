
//////////////////////////////////////////////////////////////////////////
/// CommercialLineUpdateAction class implementation.
/// @file CommercialLineUpdateAction.cpp
/// @author Hugues Romain
/// @date 2010
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

#include "CommercialLineUpdateAction.h"

#include "ActionException.h"
#include "CommercialLineTableSync.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "ReservationContact.h"
#include "ReservationContactTableSync.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "TransportNetworkTableSync.h"
#include "TreeFolder.hpp"
#include "TreeFolderTableSync.hpp"
#include "User.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace pt;
	using namespace impex;
	using namespace tree;

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
		const string CommercialLineUpdateAction::PARAMETER_FOREGROUND_COLOR = Action_PARAMETER_PREFIX + "foreground_color";
		const string CommercialLineUpdateAction::PARAMETER_STYLE = Action_PARAMETER_PREFIX + "st";
		const string CommercialLineUpdateAction::PARAMETER_IMAGE = Action_PARAMETER_PREFIX + "im";
		const string CommercialLineUpdateAction::PARAMETER_NETWORK_ID = Action_PARAMETER_PREFIX + "ni";
		const string CommercialLineUpdateAction::PARAMETER_RESERVATION_CONTACT_ID = Action_PARAMETER_PREFIX + "ri";
		const string CommercialLineUpdateAction::PARAMETER_MAP_URL = Action_PARAMETER_PREFIX + "map_url";
		const string CommercialLineUpdateAction::PARAMETER_DOC_URL = Action_PARAMETER_PREFIX + "doc_url";
		const string CommercialLineUpdateAction::PARAMETER_TIMETABLE_ID = Action_PARAMETER_PREFIX + "timetable_id";
		const string CommercialLineUpdateAction::PARAMETER_DISPLAY_DURATION_BEFORE_FIRST_DEPARTURE = Action_PARAMETER_PREFIX + "_display_duration_before_first_departure";
		const string CommercialLineUpdateAction::PARAMETER_WEIGHT_FOR_SORTING = Action_PARAMETER_PREFIX + "weight_for_sorting";



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
			if(_fgColor)
			{
				map.insert(PARAMETER_FOREGROUND_COLOR, *_fgColor ? (*_fgColor)->toXMLColor() : string());
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
			if(_folder)
			{
				map.insert(PARAMETER_NETWORK_ID, _folder->get() ? (*_folder)->getKey() : 0);
			}
			if(_reservationContact)
			{
				map.insert(PARAMETER_RESERVATION_CONTACT_ID, _reservationContact.get() ? (*_reservationContact)->getKey() : 0);
			}
			if(_style)
			{
				map.insert(PARAMETER_STYLE, *_style);
			}
			if(_mapURL)
			{
				map.insert(PARAMETER_MAP_URL, *_mapURL);
			}
			if(_docURL)
			{
				map.insert(PARAMETER_DOC_URL, *_docURL);
			}
			if(_timetableId)
			{
				map.insert(PARAMETER_TIMETABLE_ID, *_timetableId);
			}
			if(_displayDurationBeforeFirstDeparture)
			{
				map.insert(PARAMETER_DISPLAY_DURATION_BEFORE_FIRST_DEPARTURE, _displayDurationBeforeFirstDeparture->total_seconds() / 60);
			}
			if(_weightForSorting)
			{
				map.insert(PARAMETER_WEIGHT_FOR_SORTING, _weightForSorting.get_value_or(0));
			}

			// Importable
			_getImportableUpdateParametersMap(map);

			return map;
		}



		void CommercialLineUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Line
			RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_LINE_ID, 0));
			if(id) try
			{
				_line = CommercialLineTableSync::GetEditable(id, *_env);
			}
			catch(ObjectNotFoundException<CommercialLine>&)
			{
				throw ActionException("No such line");
			}

			// Network
			if(map.isDefined(PARAMETER_NETWORK_ID))	try
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID));
				if(decodeTableId(id) == TransportNetworkTableSync::TABLE.ID)
				{
					_network = TransportNetworkTableSync::GetEditable(id, *_env);
				}
				else if(decodeTableId(id) == TreeFolder::CLASS_NUMBER)
				{
					_folder = TreeFolderTableSync::GetEditable(id, *_env);
				}
			}
			catch(ObjectNotFoundException<TransportNetwork>&)
			{
				throw ActionException("No such network");
			}
			catch(ObjectNotFoundException<TreeFolder>&)
			{
				throw ActionException("No such folder");
			}
			if(!_line.get() && (!_network || !_network->get()) && (!_folder || !_folder->get()))
			{
				throw ActionException("A network or a folder must be defined");
			}

			// Color
			if(map.isDefined(PARAMETER_COLOR))
			{
				string color(map.getDefault<string>(PARAMETER_COLOR));
				_color = color.empty() ? optional<RGBColor>() : RGBColor::FromXMLColor(color);
			}

			// Foreground Color
			if(map.isDefined(PARAMETER_FOREGROUND_COLOR))
			{
				string color(map.getDefault<string>(PARAMETER_FOREGROUND_COLOR));
				_fgColor = color.empty() ? optional<RGBColor>() : RGBColor::FromXMLColor(color);
			}

			// Importable
			_setImportableUpdateFromParametersMap(*_env, map);

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

			if(map.isDefined(PARAMETER_MAP_URL))
			{
				_mapURL = map.get<string>(PARAMETER_MAP_URL);
			}

			if(map.isDefined(PARAMETER_DOC_URL))
			{
				_docURL = map.get<string>(PARAMETER_DOC_URL);
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
					_reservationContact = boost::shared_ptr<const ReservationContact>();
				}
			}

			// Timetable id
			if(map.isDefined(PARAMETER_TIMETABLE_ID))
			{
				_timetableId = map.get<RegistryKeyType>(PARAMETER_TIMETABLE_ID);
			}

			if(map.isDefined(PARAMETER_DISPLAY_DURATION_BEFORE_FIRST_DEPARTURE))
			{
				_displayDurationBeforeFirstDeparture =
					map.get<string>(PARAMETER_DISPLAY_DURATION_BEFORE_FIRST_DEPARTURE).empty() ?
					time_duration(not_a_date_time) :
					minutes(map.get<long>(PARAMETER_DISPLAY_DURATION_BEFORE_FIRST_DEPARTURE))
				;
			}

			if(map.isDefined(PARAMETER_WEIGHT_FOR_SORTING))
			{
				_weightForSorting = map.get<int>(PARAMETER_WEIGHT_FOR_SORTING);
			}
		}



		void CommercialLineUpdateAction::run(
			Request& request
		){
			//stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			if(!_line.get())
			{
				_line.reset(new CommercialLine);
			}

			if(_color)
			{
				_line->setColor(*_color);
			}

			if(_fgColor)
			{
				_line->setFgColor(*_fgColor);
			}

			// Importable
			_doImportableUpdate(*_line, request);

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
			if(_network && _network->get())
			{
				_line->setParent(**_network);
			}
			if(_folder && _folder->get())
			{
				_line->setParent(**_folder);
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
			if(_mapURL)
			{
				_line->setMapURL(*_mapURL);
			}
			if(_docURL)
			{
				_line->setDocURL(*_docURL);
			}
			if(_timetableId)
			{
				_line->setTimetableId(*_timetableId);
			}
			if(_displayDurationBeforeFirstDeparture)
			{
				_line->setDisplayDurationBeforeFirstDeparture(*_displayDurationBeforeFirstDeparture);
			}
			if(_weightForSorting)
			{
				_line->setWeightForSorting(*_weightForSorting);
			}

			CommercialLineTableSync::Save(_line.get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_line->getKey());
			}

			// ::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool CommercialLineUpdateAction::isAuthorized(
			const Session* session
		) const {
			//TODO test if the user has sufficient right level for this commercial line
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE, UNKNOWN_RIGHT_LEVEL, string());
		}
	}
}
