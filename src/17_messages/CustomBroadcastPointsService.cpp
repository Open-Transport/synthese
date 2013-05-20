
//////////////////////////////////////////////////////////////////////////////////////////
///	CustomBroadcastPointsService class implementation.
///	@file CustomBroadcastPointsService.cpp
///	@author hromain
///	@date 2013
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

#include "CustomBroadcastPointsService.hpp"

#include "CustomBroadcastPointTableSync.hpp"
#include "RequestException.h"
#include "Request.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace tree;

	template<>
	const string FactorableTemplate<Function,messages::CustomBroadcastPointsService>::FACTORY_KEY = "custom_broadcast_points";
	
	namespace messages
	{
		const string CustomBroadcastPointsService::PARAMETER_PARENT_ID = "parent_id";
		const string CustomBroadcastPointsService::PARAMETER_RECURSIVE = "recursive";
		
		const string CustomBroadcastPointsService::TAG_BROADCAST_POINT = "broadcast_point";


		ParametersMap CustomBroadcastPointsService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void CustomBroadcastPointsService::_setFromParametersMap(const ParametersMap& map)
		{
			// Object id
			RegistryKeyType bpId(
				map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0)
			);
			if(bpId > 0)
			{
				try
				{
					_bp = Env::GetOfficialEnv().get<CustomBroadcastPoint>(bpId).get();
				}
				catch (ObjectNotFoundException<CustomBroadcastPoint>&)
				{
					throw RequestException("No such broadcast point");
				}
			}
			else
			{
				RegistryKeyType parentId(
					map.getDefault<RegistryKeyType>(PARAMETER_PARENT_ID, 0)
				);
				if(parentId > 0) try
				{
					_parent = Env::GetOfficialEnv().get<CustomBroadcastPoint>(parentId).get();
				}
				catch (ObjectNotFoundException<CustomBroadcastPoint>&)
				{
					throw RequestException("No such parent broadcast point");
				}
			}

			// Recursive
			_recursive = map.getDefault<bool>(PARAMETER_RECURSIVE, false);
		}



		ParametersMap CustomBroadcastPointsService::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap map;

			// Single broadcast point export
			if(_bp)
			{
				boost::shared_ptr<ParametersMap> bpPM(new ParametersMap);
				_bp->toParametersMap(*bpPM, true);
				map.insert(TAG_BROADCAST_POINT, bpPM);
			}
			else // All broadcast points export
			{
				_exportLevel(map, _parent);
			}

			return map;
		}
		
		
		
		bool CustomBroadcastPointsService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string CustomBroadcastPointsService::getOutputMimeType() const
		{
			return "text/html";
		}



		CustomBroadcastPointsService::CustomBroadcastPointsService():
			_bp(NULL),
			_parent(NULL),
			_recursive(false)
		{}



		//////////////////////////////////////////////////////////////////////////
		/// Exports both broadcast points and folders contained in the specified folder.
		/// @param pm the parameters map to populate
		/// @param parent the parent of the objects to export
		void CustomBroadcastPointsService::_exportLevel(
			util::ParametersMap& pm,
			const CustomBroadcastPoint* parent
		) const	{

			// Loop on broadcast points
			BOOST_FOREACH(
				boost::shared_ptr<CustomBroadcastPoint> bp,
				CustomBroadcastPointTableSync::Search(
					Env::GetOfficialEnv(),
					string(),
					parent ? parent->getKey() : RegistryKeyType(0)
				)
			){
				boost::shared_ptr<ParametersMap> bpPM(new ParametersMap);
				bp->toParametersMap(*bpPM, true);
				pm.insert(TAG_BROADCAST_POINT, bpPM);

				// If recursive export in the folder
				if(_recursive)
				{
					_exportLevel(*bpPM, bp.get());
				}
			}
		}
}	}
