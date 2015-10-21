
//////////////////////////////////////////////////////////////////////////
/// LineStopUpdateAction class implementation.
/// @file LineStopUpdateAction.cpp
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

#include "LineStopUpdateAction.hpp"

#include "ActionException.h"
#include "ContinuousServiceTableSync.h"
#include "DBModule.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "LineStopTableSync.h"
#include "GeometryField.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "ObjectUpdateAction.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "PTUseRule.h"
#include "Request.h"
#include "ScheduledServiceTableSync.h"
#include "Session.h"
#include "StopPointTableSync.hpp"
#include "TransportNetworkRight.h"
#include "User.h"

#include <geos/geom/LineString.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace geos::io;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace db;
	using namespace geography;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::LineStopUpdateAction>::FACTORY_KEY("LineStopUpdateAction");
	}

	namespace pt
	{
		const string LineStopUpdateAction::PARAMETER_LINE_STOP_ID = Action_PARAMETER_PREFIX + "id";
		const string LineStopUpdateAction::PARAMETER_PHYSICAL_STOP_ID = Action_PARAMETER_PREFIX + "ps";
		const string LineStopUpdateAction::PARAMETER_ALLOWED_DEPARTURE = Action_PARAMETER_PREFIX + "ad";
		const string LineStopUpdateAction::PARAMETER_ALLOWED_ARRIVAL = Action_PARAMETER_PREFIX + "aa";
		const string LineStopUpdateAction::PARAMETER_ALLOWED_INTERNAL = Action_PARAMETER_PREFIX + "ai";
		const string LineStopUpdateAction::PARAMETER_WITH_SCHEDULES = Action_PARAMETER_PREFIX + "with_schedules";
		const string LineStopUpdateAction::PARAMETER_READ_LENGTH_FROM_GEOMETRY = Action_PARAMETER_PREFIX + "read_length_from_geometry";
		const string LineStopUpdateAction::PARAMETER_RESERVATION_NEEDED = Action_PARAMETER_PREFIX + "_reservation_needed";
		const string LineStopUpdateAction::PARAMETER_CLEAR_GEOM = Action_PARAMETER_PREFIX + "_clear_geom";
		const string LineStopUpdateAction::PARAMETER_REVERSE_DRT_AREA = Action_PARAMETER_PREFIX + "_reverse_drt_area";



		LineStopUpdateAction::LineStopUpdateAction():
			_nextLineStop(NULL),
			_prevLineStop(NULL),
			_readLengthFromGeometry(false),
			_clearGeom(false)
		{}



		ParametersMap LineStopUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_lineStop.get())
			{
				map.insert(PARAMETER_LINE_STOP_ID, _lineStop->getKey());
			}
			if(_physicalStop.get())
			{
				map.insert(PARAMETER_PHYSICAL_STOP_ID, _physicalStop->getKey());
			}
			if(_allowedDeparture)
			{
				map.insert(PARAMETER_ALLOWED_DEPARTURE, *_allowedDeparture);
			}
			if(_allowedArrival)
			{
				map.insert(PARAMETER_ALLOWED_ARRIVAL, *_allowedArrival);
			}
			if(_allowedInternal)
			{
				map.insert(PARAMETER_ALLOWED_INTERNAL, *_allowedInternal);
			}
			if(_withSchedules)
			{
				map.insert(PARAMETER_WITH_SCHEDULES, *_withSchedules);
			}
			if(_reservationNeeded)
			{
				map.insert(PARAMETER_RESERVATION_NEEDED, *_reservationNeeded);
			}
			if(_reverseDRTArea)
			{
				map.insert(PARAMETER_REVERSE_DRT_AREA, *_reverseDRTArea);
			}
			if(_geometry)
			{
				WKTWriter writer;
				map.insert(
					ObjectUpdateAction::GetInputName<LineStringGeometry>(),
					_geometry->get() ? writer.write(static_cast<const Geometry*>(_geometry->get())) : string()
				);
			}
			map.insert(PARAMETER_READ_LENGTH_FROM_GEOMETRY, _readLengthFromGeometry);
			map.insert(PARAMETER_CLEAR_GEOM, _clearGeom);
			return map;
		}



		void LineStopUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Line stop
			try
			{
				_lineStop = LineStopTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_LINE_STOP_ID), *_env);
			}
			catch(ObjectNotFoundException<LineStop>&)
			{
				throw ActionException("No such line stop");
			}

			// Loading all line stops of the path
			try
			{
				const JourneyPattern& path(*_lineStop->get<Line>());
				LineStopTableSync::Search(
					*_env,
					path.getKey()
				);
				if(path.getLineStop(_lineStop->get<RankInPath>()) != _lineStop.get())
				{
					throw ActionException("Rank are corrupted, use the fix utility");
				}
				if(_lineStop->get<RankInPath>() != 0)
				{
					_prevLineStop = const_cast<LineStop*>(path.getLineStop(_lineStop->get<RankInPath>() - 1));
				}
				if(_lineStop->get<RankInPath>()+1 != path.getLineStops().size())
				{
					_nextLineStop = const_cast<LineStop*>(path.getLineStop(_lineStop->get<RankInPath>() + 1));
				}
			}
			catch(...)
			{}

			// Physical stop
			if(map.isDefined(PARAMETER_PHYSICAL_STOP_ID)) try
			{
				RegistryKeyType psid(map.get<RegistryKeyType>(PARAMETER_PHYSICAL_STOP_ID));
				if(psid)
				{
					_physicalStop = StopPointTableSync::GetEditable(psid, *_env);
				}
			}
			catch(ObjectNotFoundException<StopPoint>&)
			{
				throw ActionException("No such physical stop");
			}

			// Allowed arrival
			if(map.isDefined(PARAMETER_ALLOWED_ARRIVAL))
			{
				_allowedArrival = map.get<bool>(PARAMETER_ALLOWED_ARRIVAL);
			}

			// Allowed departure
			if(map.isDefined(PARAMETER_ALLOWED_DEPARTURE))
			{
				_allowedDeparture = map.get<bool>(PARAMETER_ALLOWED_DEPARTURE);
			}

			// Allowed internal
			if(map.isDefined(PARAMETER_ALLOWED_INTERNAL))
			{
				_allowedInternal = map.get<bool>(PARAMETER_ALLOWED_INTERNAL);
			}

			// With schedules
			if(map.isDefined(PARAMETER_WITH_SCHEDULES))
			{
				_withSchedules = map.get<bool>(PARAMETER_WITH_SCHEDULES);
			}

			// Reservation needed
			if(map.isDefined(PARAMETER_RESERVATION_NEEDED))
			{
				_reservationNeeded = map.get<bool>(PARAMETER_RESERVATION_NEEDED);
			}

			// Reservation needed
			if(map.isDefined(PARAMETER_REVERSE_DRT_AREA))
			{
				_reverseDRTArea = map.get<bool>(PARAMETER_REVERSE_DRT_AREA);
			}

			// Clear geom
			_clearGeom = map.getDefault<bool>(PARAMETER_CLEAR_GEOM, false);

			// Geometry
			if(map.isDefined(ObjectUpdateAction::GetInputName<LineStringGeometry>()))
			{
				WKTReader reader(&CoordinatesSystem::GetStorageCoordinatesSystem().getGeometryFactory());
				_geometry = boost::shared_ptr<LineString>(
					static_cast<LineString*>(
						reader.read(map.get<string>(ObjectUpdateAction::GetInputName<LineStringGeometry>()))
				)	);
				_clearGeom = false;
			}
			
			// Read length from geometry
			_readLengthFromGeometry = map.getDefault<bool>(PARAMETER_READ_LENGTH_FROM_GEOMETRY, false);

			// Load services if update should be necessary
			if(	(_readLengthFromGeometry && _lineStop->get<LineStringGeometry>()) ||
				_withSchedules
			){
				BOOST_FOREACH(boost::shared_ptr<ScheduledService> serv, ScheduledServiceTableSync::Search(
					*_env,
					_lineStop->get<Line>()->getKey()
				)){
					serv->getDepartureSchedules(true, false);
					serv->getArrivalSchedules(true, false);
				}

				BOOST_FOREACH(boost::shared_ptr<ContinuousService> serv, ContinuousServiceTableSync::Search(
					*_env,
					_lineStop->get<Line>()->getKey()
				)){
					serv->getDepartureSchedules(true, false);
					serv->getArrivalSchedules(true, false);
				}
			}
		}



		void LineStopUpdateAction::run(
			Request& request
		){
			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			// Physical stop change
			if(	_physicalStop.get() &&
				dynamic_cast<StopPoint*>(&*_lineStop->get<LineNode>()) &&
				_physicalStop.get() != dynamic_cast<StopPoint*>(&*_lineStop->get<LineNode>())
			){
				// Automated update of geometry of preceding line stop if possible
				if(_prevLineStop && _prevLineStop->get<LineNode>() && dynamic_cast<StopPoint*>(&*_prevLineStop->get<LineNode>()))
				{
					Env env2;
					boost::shared_ptr<LineStop> templateObject(
						LineStopTableSync::SearchSimilarLineStop(
							dynamic_cast<StopPoint&>(*_prevLineStop->get<LineNode>()),
							*_physicalStop,
							env2
					)	);
					if(templateObject.get())
					{
						_prevLineStop->set<LineStringGeometry>(templateObject->get<LineStringGeometry>());
					}
				}

				// Automated update of current geometry if possible and necessary
				if(_nextLineStop && !_geometry && _nextLineStop->get<LineNode>() && dynamic_cast<StopPoint*>(&*_nextLineStop->get<LineNode>()))
				{
					Env env2;
					boost::shared_ptr<LineStop> templateObject(
						LineStopTableSync::SearchSimilarLineStop(
							*_physicalStop,
							dynamic_cast<StopPoint&>(*_nextLineStop->get<LineNode>()),
							env2
					)	);
					if(templateObject.get())
					{
						_lineStop->set<LineStringGeometry>(templateObject->get<LineStringGeometry>());
					}
				}

				// Change the physical stop
				_lineStop->set<LineNode>(*_physicalStop);
			}

			// Allowed internal
			if(_allowedInternal)
			{
				_lineStop->set<InternalService>(*_allowedInternal);
			}

			// Allowed arrival
			if(_allowedArrival)
			{
				_lineStop->set<IsArrival>(*_allowedArrival);
			}

			// Allowed departure
			if(_allowedDeparture)
			{
				_lineStop->set<IsDeparture>(*_allowedDeparture);
			}

			// With schedules
			if(_withSchedules)
			{
				 _lineStop->set<ScheduleInput>(*_withSchedules);
			}

			// With reservation
			if(_reservationNeeded)
			{
				_lineStop->set<ReservationNeeded>(*_reservationNeeded);
			}

			// Reverse DRT Area
			if(_reverseDRTArea)
			{
				_lineStop->set<ReverseDRTArea>(*_reverseDRTArea);
			}

			// Geometry
			if(_geometry)
			{
				_lineStop->set<LineStringGeometry>(*_geometry);
			}

			// Clear geometry
			if(_clearGeom)
			{
				_lineStop->set<LineStringGeometry>(boost::shared_ptr<LineString>());
			}

			// Savings
			DBTransaction transaction;
			if(_prevLineStop)
			{
				LineStopTableSync::Save(_prevLineStop, transaction);
			}
			LineStopTableSync::Save(_lineStop.get(), transaction);

			if(	_readLengthFromGeometry && _lineStop->get<LineStringGeometry>())
			{
				LineStopTableSync::ChangeLength(
					*_lineStop,
					floor(_lineStop->get<LineStringGeometry>()->getLength()),
					transaction
				);
			}

			// Some line stop updates can impact the service schedules
			BOOST_FOREACH(const ScheduledService::Registry::value_type& it, _env->getRegistry<ScheduledService>())
			{
				it.second->regenerateDataSchedules();
				ScheduledServiceTableSync::Save(it.second.get(), transaction);
			}
			BOOST_FOREACH(const ContinuousService::Registry::value_type& it, _env->getRegistry<ContinuousService>())
			{
				it.second->regenerateDataSchedules();
				ContinuousServiceTableSync::Save(it.second.get(), transaction);
			}

			transaction.run();

			//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool LineStopUpdateAction::isAuthorized(
			const Session* session
		) const {
			//TODO test if the user has sufficient right level for this commercial line
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE, UNKNOWN_RIGHT_LEVEL, string());
		}
}	}
