
//////////////////////////////////////////////////////////////////////////
/// StopAreaUpdateAction class implementation.
/// @file StopAreaUpdateAction.cpp
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

#include "StopAreaUpdateAction.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "Request.h"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"
#include "City.h"
#include "CityTableSync.h"
#include "PathGroup.h"
#include "StopPoint.hpp"
#include "StopPointTableSync.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"

using namespace std;
using namespace boost::posix_time;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
    using namespace pt;
	using namespace impex;
	using namespace geography;
    using namespace graph;
    using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::StopAreaUpdateAction>::FACTORY_KEY("StopAreaUpdateAction");
	}

	namespace pt
	{
		const string StopAreaUpdateAction::PARAMETER_ALLOWED_CONNECTIONS = Action_PARAMETER_PREFIX + "ac";
		const string StopAreaUpdateAction::PARAMETER_PLACE_ID = Action_PARAMETER_PREFIX + "id";
		const string StopAreaUpdateAction::PARAMETER_DEFAULT_TRANSFER_DURATION = Action_PARAMETER_PREFIX + "dt";
		const string StopAreaUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string StopAreaUpdateAction::PARAMETER_SHORT_NAME = Action_PARAMETER_PREFIX + "sn";
		const string StopAreaUpdateAction::PARAMETER_LONG_NAME = Action_PARAMETER_PREFIX + "ln";
		const string StopAreaUpdateAction::PARAMETER_CITY_ID = Action_PARAMETER_PREFIX + "ci";
		const string StopAreaUpdateAction::PARAMETER_IS_MAIN = Action_PARAMETER_PREFIX + "ma";
		const string StopAreaUpdateAction::PARAMETER_TIMETABLE_NAME = Action_PARAMETER_PREFIX + "tn";



		ParametersMap StopAreaUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_place.get())
			{
				map.insert(PARAMETER_PLACE_ID, _place->getKey());
			}
			if(_allowedConnections)
			{
				map.insert(PARAMETER_ALLOWED_CONNECTIONS, *_allowedConnections);
			}
			if(_defaultTransferDuration)
			{
				map.insert(PARAMETER_DEFAULT_TRANSFER_DURATION, _defaultTransferDuration->total_seconds() / 60);
			}
			if(_city.get())
			{
				map.insert(PARAMETER_CITY_ID, _city->getKey());
			}
			if(_longName)
			{
				map.insert(PARAMETER_LONG_NAME, *_longName);
			}
			if(_shortName)
			{
				map.insert(PARAMETER_SHORT_NAME, *_shortName);
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			if(_isMain)
			{
				map.insert(PARAMETER_IS_MAIN, *_isMain);
			}
			if(_timetableName)
			{
				map.insert(PARAMETER_TIMETABLE_NAME, *_timetableName);
			}

			// Importable
			_getImportableUpdateParametersMap(map);

			return map;
		}



		void StopAreaUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_place = StopAreaTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_PLACE_ID), *_env);
			}
			catch(ObjectNotFoundException<StopArea>&)
			{
				throw ActionException("No such place");
			}

			if(map.getOptional<RegistryKeyType>(PARAMETER_CITY_ID)) try
			{
				_city = CityTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_CITY_ID), *_env);
			}
			catch(ObjectNotFoundException<City>&)
			{
				throw ActionException("No such city");
			}

			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
			}

			if(map.isDefined(PARAMETER_SHORT_NAME))
			{
				_shortName = map.get<string>(PARAMETER_SHORT_NAME);
			}

			if(map.isDefined(PARAMETER_LONG_NAME))
			{
				_longName = map.get<string>(PARAMETER_LONG_NAME);
			}

			if(map.isDefined(PARAMETER_IS_MAIN))
			{
				_isMain = map.get<bool>(PARAMETER_IS_MAIN);
			}

			if(map.isDefined(PARAMETER_TIMETABLE_NAME))
			{
				_timetableName = map.get<string>(PARAMETER_TIMETABLE_NAME);
			}

			if(map.isDefined(PARAMETER_ALLOWED_CONNECTIONS))
			{
				_allowedConnections = map.get<bool>(PARAMETER_ALLOWED_CONNECTIONS);
			}

			if(map.isDefined(PARAMETER_DEFAULT_TRANSFER_DURATION))
			{
				_defaultTransferDuration = minutes(map.get<int>(PARAMETER_DEFAULT_TRANSFER_DURATION));
			}

			// Importable
			_setImportableUpdateFromParametersMap(*_env, map);
		}



		void StopAreaUpdateAction::run(
			Request& request
		){
			//stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			if(_allowedConnections)
			{
				_place->setAllowedConnection(*_allowedConnections);
			}

			if(_defaultTransferDuration)
			{
				_place->setDefaultTransferDelay(*_defaultTransferDuration);
			}

			if(_city.get())
			{
				_place->setCity(_city.get());
				if(_isMain)
				{
					if(*_isMain && !_city->includes(*_place))
					{
						_city->addIncludedPlace(*_place);
					}
					if(!*_isMain && _city->includes(*_place))
					{
						_city->removeIncludedPlace(*_place);
					}
				}
			}

			if(_name)
			{
				_place->setName(*_name);
			}

			if(_shortName)
			{
				_place->setName13(*_shortName);
			}

			if(_longName)
			{
				_place->setName26(*_longName);
			}

			if(_timetableName)
			{
				_place->setTimetableName(*_timetableName);
			}

			// Importable
			_doImportableUpdate(*_place, request);

            // TODO : do we have to load everything of these ?
			// at least it actually works.
            StopPointTableSync::SearchResult stops(
                StopPointTableSync::Search(*_env, _place->getKey())
            );
            CommercialLineTableSync::SearchResult lines(
                CommercialLineTableSync::Search(*_env)
            );
            JourneyPatternTableSync::SearchResult journeyPatterns(
                JourneyPatternTableSync::Search(*_env)
            );
            LineStopTableSync::SearchResult lineStops(
                LineStopTableSync::Search(*_env)
            );

            _place->clearAndPropagateUsefulTransfer(PTModule::GRAPH_ID);

            DBTransaction transaction;

            StopAreaTableSync::Save(_place.get(), transaction);
            BOOST_FOREACH(const boost::shared_ptr<StopPoint>& stop, stops)
            {
                StopPointTableSync::Save(stop.get(), transaction);
            }
            BOOST_FOREACH(const boost::shared_ptr<CommercialLine>& line, lines)
            {
                CommercialLineTableSync::Save(line.get(), transaction);
            }
            BOOST_FOREACH(const boost::shared_ptr<JourneyPattern>& line, journeyPatterns)
            {
                JourneyPatternTableSync::Save(line.get(), transaction);
            }
            BOOST_FOREACH(const boost::shared_ptr<LineStop>& lineStop, lineStops)
            {
                LineStopTableSync::Save(lineStop.get(), transaction);
            }

            transaction.run();

			//::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool StopAreaUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
