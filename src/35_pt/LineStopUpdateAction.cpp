
//////////////////////////////////////////////////////////////////////////
/// LineStopUpdateAction class implementation.
/// @file LineStopUpdateAction.cpp
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
#include "LineStopUpdateAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "LineStopTableSync.h"
#include "StopPointTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "LineArea.hpp"
#include "DBModule.h"
#include "HTMLMap.hpp"

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
			if(_geometry)
			{
				WKTWriter writer;
				map.insert(HTMLMap::PARAMETER_ACTION_WKT, _geometry->get() ? writer.write(static_cast<const Geometry*>(_geometry->get())) : string());
			}
			return map;
		}



		void LineStopUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_lineStop = LineStopTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_LINE_STOP_ID), *_env);

				if(map.isDefined(PARAMETER_PHYSICAL_STOP_ID))
				{
					RegistryKeyType psid(map.get<RegistryKeyType>(PARAMETER_PHYSICAL_STOP_ID));
					if(psid)
					{
						_physicalStop = StopPointTableSync::GetEditable(psid, *_env);
					}
				}

				if(map.isDefined(PARAMETER_ALLOWED_ARRIVAL))
				{
					_allowedArrival = map.get<bool>(PARAMETER_ALLOWED_ARRIVAL);
				}
				if(map.isDefined(PARAMETER_ALLOWED_DEPARTURE))
				{
					_allowedDeparture = map.get<bool>(PARAMETER_ALLOWED_DEPARTURE);
				}
				if(map.isDefined(PARAMETER_ALLOWED_INTERNAL))
				{
					_allowedInternal = map.get<bool>(PARAMETER_ALLOWED_INTERNAL);
				}

				if(map.isDefined(HTMLMap::PARAMETER_ACTION_WKT))
				{
					WKTReader reader(&DBModule::GetStorageCoordinatesSystem().getGeometryFactory());
					_geometry = shared_ptr<LineString>(
						static_cast<LineString*>(
							reader.read(map.get<string>(HTMLMap::PARAMETER_ACTION_WKT))
					)	);
				}
			}
			catch(ObjectNotFoundException<LineStop>&)
			{
				throw ActionException("No such line stop");
			}
			catch(ObjectNotFoundException<StopPoint>&)
			{
				throw ActionException("No such physical stop");
			}
		}



		void LineStopUpdateAction::run(
			Request& request
		){
			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			if(_physicalStop.get() && dynamic_cast<DesignatedLinePhysicalStop*>(_lineStop.get()))
			{
				dynamic_cast<DesignatedLinePhysicalStop&>(*_lineStop).setPhysicalStop(*_physicalStop);
			}
			if(_allowedInternal && dynamic_cast<LineArea*>(_lineStop.get()))
			{
				dynamic_cast<LineArea&>(*_lineStop).setInternalService(*_allowedInternal);
			}
			if(_allowedArrival)
			{
				_lineStop->setIsArrival(*_allowedArrival);
			}
			if(_allowedDeparture)
			{
				_lineStop->setIsDeparture(*_allowedDeparture);
			}
			if(_geometry)
			{
				_lineStop->setGeometry(*_geometry);
			}

			LineStopTableSync::Save(_lineStop.get());

			//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool LineStopUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
