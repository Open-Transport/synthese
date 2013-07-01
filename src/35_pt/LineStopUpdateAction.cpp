
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
#include "DBModule.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "LineStopTableSync.h"
#include "GeometryField.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineArea.hpp"
#include "LineStopTableSync.h"
#include "ObjectUpdateAction.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
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
			if(dynamic_cast<DesignatedLinePhysicalStop*>(_lineStop.get())) try
			{
				const Path& path(*_lineStop->getParentPath());
				LineStopTableSync::Search(
					*_env,
					path.getKey()
				);
				if(path.getEdge(_lineStop->getRankInPath()) != _lineStop.get())
				{
					throw ActionException("Rank are corrupted, use the fix utility");
				}
				if(_lineStop->getRankInPath() != 0)
				{
					_prevLineStop = dynamic_cast<DesignatedLinePhysicalStop*>(const_cast<Edge*>(path.getEdge(_lineStop->getRankInPath() - 1)));
				}
				if(_lineStop->getRankInPath()+1 != path.getEdges().size())
				{
					_nextLineStop = dynamic_cast<DesignatedLinePhysicalStop*>(const_cast<Edge*>(path.getEdge(_lineStop->getRankInPath() + 1)));
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
		}



		void LineStopUpdateAction::run(
			Request& request
		){
			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			// Physical stop change
			if(	_physicalStop.get() &&
				dynamic_cast<DesignatedLinePhysicalStop*>(_lineStop.get()) &&
				_physicalStop.get() != dynamic_cast<DesignatedLinePhysicalStop&>(*_lineStop).getPhysicalStop()
			){
				DesignatedLinePhysicalStop& linePhysicalStop(
					dynamic_cast<DesignatedLinePhysicalStop&>(*_lineStop)
				);

				// Automated update of geometry of preceding line stop if possible
				if(_prevLineStop)
				{
					Env env2;
					boost::shared_ptr<DesignatedLinePhysicalStop> templateObject(
						LineStopTableSync::SearchSimilarLineStop(
							*_prevLineStop->getPhysicalStop(),
							*_physicalStop,
							env2
					)	);
					if(templateObject.get())
					{
						_prevLineStop->setGeometry(templateObject->getGeometry());
					}
				}

				// Automated update of current geometry if possible and necessary
				if(_nextLineStop && !_geometry)
				{
					Env env2;
					boost::shared_ptr<DesignatedLinePhysicalStop> templateObject(
						LineStopTableSync::SearchSimilarLineStop(
							*_physicalStop,
							*_nextLineStop->getPhysicalStop(),
							env2
					)	);
					if(templateObject.get())
					{
						_lineStop->setGeometry(templateObject->getGeometry());
					}
				}

				// Change the physical stop
				linePhysicalStop.setPhysicalStop(*_physicalStop);
			}

			// Allowed internal
			if(_allowedInternal && dynamic_cast<LineArea*>(_lineStop.get()))
			{
				dynamic_cast<LineArea&>(*_lineStop).setInternalService(*_allowedInternal);
			}

			// Allowed arrival
			if(_allowedArrival)
			{
				_lineStop->setIsArrival(*_allowedArrival);
			}

			// Allowed departure
			if(_allowedDeparture)
			{
				_lineStop->setIsDeparture(*_allowedDeparture);
			}

			// With schedules
			if(_withSchedules && dynamic_cast<DesignatedLinePhysicalStop*>(_lineStop.get()))
			{
				 dynamic_cast<DesignatedLinePhysicalStop*>(_lineStop.get())->setScheduleInput(*_withSchedules);
			}

			// With schedules
			if(_reservationNeeded && dynamic_cast<DesignatedLinePhysicalStop*>(_lineStop.get()))
			{
				dynamic_cast<DesignatedLinePhysicalStop*>(_lineStop.get())->setReservationNeeded(*_reservationNeeded);
			}

			// Geometry
			if(_geometry)
			{
				_lineStop->setGeometry(*_geometry);
			}

			// Clear geometry
			if(_clearGeom)
			{
				_lineStop->setGeometry(boost::shared_ptr<LineString>());
			}

			// Savings
			DBTransaction transaction;
			if(_prevLineStop)
			{
				LineStopTableSync::Save(_prevLineStop, transaction);
			}
			LineStopTableSync::Save(_lineStop.get(), transaction);

			if(	_readLengthFromGeometry && _lineStop->getGeometry())
			{
				LineStopTableSync::ChangeLength(
					*_lineStop,
					floor(_lineStop->getGeometry()->getLength()),
					transaction
				);
			}

			if(	(_readLengthFromGeometry && _lineStop->getGeometry()) ||
				_withSchedules
			){
				JourneyPatternTableSync::ReloadServices(
					_lineStop->getKey(),
					transaction
				);
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
