
//////////////////////////////////////////////////////////////////////////
/// StopPointUpdateAction class implementation.
/// @file StopPointUpdateAction.cpp
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
#include "StopPointUpdateAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "StopPointTableSync.hpp"
#include "StopAreaTableSync.hpp"
#include "ImportableTableSync.hpp"
#include "ImportableAdmin.hpp"
#include "HTMLMap.hpp"

#include <geos/io/WKTReader.h>

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace geos::io;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace geography;
	using namespace impex;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::StopPointUpdateAction>::FACTORY_KEY("StopPointUpdateAction");
	}

	namespace pt
	{
		const string StopPointUpdateAction::PARAMETER_STOP_ID = Action_PARAMETER_PREFIX + "id";
		const string StopPointUpdateAction::PARAMETER_X = Action_PARAMETER_PREFIX + "x";
		const string StopPointUpdateAction::PARAMETER_Y = Action_PARAMETER_PREFIX + "y";
		const string StopPointUpdateAction::PARAMETER_OPERATOR_CODE = Action_PARAMETER_PREFIX + "oc";
		const string StopPointUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string StopPointUpdateAction::PARAMETER_STOP_AREA = Action_PARAMETER_PREFIX + "sa";



		ParametersMap StopPointUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_stop.get())
			{
				map.insert(PARAMETER_STOP_ID, _stop->getKey());
			}

			if(_point.get() && !_point->isEmpty())
			{
				map.insert(PARAMETER_X, _point->getX());
				map.insert(PARAMETER_Y, _point->getY());
			}

			if(_dataSourceLinks)
			{
				map.insert(PARAMETER_OPERATOR_CODE, ImportableTableSync::SerializeDataSourceLinks(*_dataSourceLinks));
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			if(_stopArea)
			{
				map.insert(PARAMETER_STOP_AREA, _stopArea->get() ? (*_stopArea)->getKey() : RegistryKeyType(0));
			}
			if(_dataSourceLinks)
			{
				map.insert(ImportableAdmin::PARAMETER_DATA_SOURCE_LINKS, ImportableTableSync::SerializeDataSourceLinks(*_dataSourceLinks));
			}
			return map;
		}



		void StopPointUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_stop = StopPointTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_STOP_ID), *_env);
			}
			catch(ObjectNotFoundException<StopPoint>&)
			{
				throw ActionException("No such physical stop");
			}

			if(map.isDefined(PARAMETER_STOP_AREA)) try
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_STOP_AREA));
				if(id)
				{
					_stopArea = StopAreaTableSync::Get(id, *_env);
				}
				else
				{
					_stopArea = shared_ptr<StopArea>();
				}
			}
			catch(ObjectNotFoundException<StopArea>&)
			{
				throw ActionException("No such stop area");
			}

			if(	!map.getDefault<string>(PARAMETER_X).empty() &&
				!map.getDefault<string>(PARAMETER_Y).empty()
			){
				_point = CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(
					map.get<double>(PARAMETER_X),
					map.get<double>(PARAMETER_Y)
				);
			}

			if(map.isDefined(HTMLMap::PARAMETER_ACTION_WKT))
			{
				WKTReader reader(&DBModule::GetStorageCoordinatesSystem().getGeometryFactory());
				_point.reset(static_cast<Point*>(reader.read(map.get<string>(HTMLMap::PARAMETER_ACTION_WKT))));
			}

			if(map.isDefined(PARAMETER_OPERATOR_CODE))
			{
				_dataSourceLinks = ImportableTableSync::GetDataSourceLinksFromSerializedString(
					map.get<string>(PARAMETER_OPERATOR_CODE),
					*_env
				);
			}

			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
			}

			if(map.isDefined(ImportableAdmin::PARAMETER_DATA_SOURCE_LINKS))
			{
				_dataSourceLinks = ImportableTableSync::GetDataSourceLinksFromSerializedString(map.get<string>(ImportableAdmin::PARAMETER_DATA_SOURCE_LINKS), *_env);
			}
		}



		void StopPointUpdateAction::run(
			Request& request
		){
//			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			if(_point)
			{
				_stop->setGeometry(_point);
			}
			if(_dataSourceLinks)
			{
				_stop->setDataSourceLinks(*_dataSourceLinks);
			}
			if(_name)
			{
				_stop->setName(*_name);
			}
			if(_stopArea)
			{
				_stop->setHub(_stopArea->get());
			}

			StopPointTableSync::Save(_stop.get());

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool StopPointUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}



		void StopPointUpdateAction::setPoint( boost::shared_ptr<geos::geom::Point> value )
		{
			_point = _point = CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*value);
		}
	}
}
