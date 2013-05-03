
/** StopsInterSYNTHESEIdFilter class implementation.
	@file StopsInterSYNTHESEIdFilter.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "StopsInterSYNTHESEIdFilter.hpp"

#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace inter_synthese;

	template<>
	const string FactorableTemplate<InterSYNTHESEIdFilter, StopsInterSYNTHESEIdFilter>::FACTORY_KEY = "stops";

	namespace pt
	{
		std::string StopsInterSYNTHESEIdFilter::convertId(
			util::RegistryTableType tableId,
			const std::string& fieldName,
			const string& objectId
		) const	{

			if(fieldName == TABLE_COL_ID)
			{
				RegistryKeyType id(lexical_cast<RegistryKeyType>(objectId));
				if(tableId == StopAreaTableSync::TABLE.ID)
				{
					if(!Env::GetOfficialEnv().getRegistry<StopArea>().contains(id))
					{
						StopArea* stopArea(_dataSource->getObjectByCode<StopArea>(objectId));
						if(stopArea)
						{
							return string();
						}
					}
				}
				else if(tableId ==  StopPointTableSync::TABLE.ID)
				{
					if(!Env::GetOfficialEnv().getRegistry<StopPoint>().contains(id))
					{
						StopPoint* stopPoint(_dataSource->getObjectByCode<StopPoint>(objectId));
						if(stopPoint)
						{
							return string();
						}
					}
				}
				return objectId;
			}
			else
			{
				if(objectId[0] >= '0' && objectId[0] <= '9')
				{
					try
					{
						RegistryKeyType id(lexical_cast<RegistryKeyType>(objectId));
						RegistryTableType tblId(decodeTableId(id));
						if(tblId == StopAreaTableSync::TABLE.ID)
						{
							if(!Env::GetOfficialEnv().getRegistry<StopArea>().contains(id))
							{
								StopArea* stopArea(_dataSource->getObjectByCode<StopArea>(objectId));
								if(stopArea)
								{
									return lexical_cast<string>(stopArea->getKey()); 
								}
							}
						}
						else if(tblId == StopPointTableSync::TABLE.ID)
						{
							if(!Env::GetOfficialEnv().getRegistry<StopPoint>().contains(id))
							{
								StopPoint* stopPoint(_dataSource->getObjectByCode<StopPoint>(objectId));
								if(stopPoint)
								{
									return lexical_cast<string>(stopPoint->getKey());
								}
							}
						}
					}
					catch(bad_lexical_cast&)
					{
					}
				}
			}
			return objectId;
		}
}	}

