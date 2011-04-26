
//////////////////////////////////////////////////////////////////////////
/// DataSourceUpdateAction class implementation.
/// @file DataSourceUpdateAction.cpp
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
#include "DataSourceUpdateAction.hpp"
#include "GlobalRight.h"
#include "Request.h"
#include "DataSourceTableSync.h"
#include "FileFormat.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace impex;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, impex::DataSourceUpdateAction>::FACTORY_KEY("DataSourceUpdateAction");
	}

	namespace impex
	{
		const string DataSourceUpdateAction::PARAMETER_DATA_SOURCE = Action_PARAMETER_PREFIX + "id";
		const string DataSourceUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string DataSourceUpdateAction::PARAMETER_FORMAT = Action_PARAMETER_PREFIX + "fo";
		const string DataSourceUpdateAction::PARAMETER_ICON = Action_PARAMETER_PREFIX + "ic";
		const string DataSourceUpdateAction::PARAMETER_CHARSET = Action_PARAMETER_PREFIX + "cs";
		const string DataSourceUpdateAction::PARAMETER_SRID = Action_PARAMETER_PREFIX + "sr";
		
		
		
		ParametersMap DataSourceUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_dataSource.get())
			{
				map.insert(PARAMETER_DATA_SOURCE, _dataSource->getKey());
			}
			if(_format)
			{
				map.insert(PARAMETER_FORMAT, *_format);
			}
			if(_icon)
			{
				map.insert(PARAMETER_ICON, *_icon);
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			if(_charset)
			{
				map.insert(PARAMETER_CHARSET, *_charset);
			}
			if(_coordinatesSystem)
			{
				map.insert(PARAMETER_SRID, lexical_cast<int>(_coordinatesSystem->getSRID()));
			}
			return map;
		}
		
		
		
		void DataSourceUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.isDefined(PARAMETER_DATA_SOURCE)) try
			{
				_dataSource = DataSourceTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_DATA_SOURCE), *_env);
			}
			catch(ObjectNotFoundException<DataSource>&)
			{
				throw ActionException("No such data source");
			}
			else
			{
				_dataSource.reset(new DataSource);
			}

			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
				if(_name->empty())
				{
					throw ActionException("The name must be non empty");
				}
			}

			if(map.isDefined(PARAMETER_FORMAT))
			{
				_format = map.getDefault<string>(PARAMETER_FORMAT);
				if(	!_format->empty() &&
					!Factory<FileFormat>::contains(*_format)
				){
					throw ActionException("The format must be manual typing or must correspond to an available file format");
				}
			}

			if(map.isDefined(PARAMETER_ICON))
			{
				_icon = map.getDefault<string>(PARAMETER_ICON);
			}

			if(map.isDefined(PARAMETER_CHARSET))
			{
				_charset = map.getDefault<string>(PARAMETER_CHARSET);
			}

			if(map.isDefined(PARAMETER_SRID))
			{
				try
				{
					_coordinatesSystem = CoordinatesSystem::GetCoordinatesSystem(map.get<CoordinatesSystem::SRID>(PARAMETER_SRID));
				}
				catch(CoordinatesSystem::CoordinatesSystemNotFoundException&)
				{
					throw ActionException("No such coordinate system");
				}
			}
		}
		
		
		
		void DataSourceUpdateAction::run(
			Request& request
		){
//			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			if(_name)
			{
				_dataSource->setName(*_name);
			}
			if(_format)
			{
				_dataSource->setFormat(*_format);
			}
			if(_icon)
			{
				_dataSource->setIcon(*_icon);
			}
			if(_charset)
			{
				_dataSource->setCharset(*_charset);
			}
			if(_coordinatesSystem)
			{
				_dataSource->setCoordinatesSystem(&*_coordinatesSystem);
			}

			DataSourceTableSync::Save(_dataSource.get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_dataSource->getKey());
			}

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}
		
		
		
		bool DataSourceUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<GlobalRight>(WRITE);
		}
	}
}
