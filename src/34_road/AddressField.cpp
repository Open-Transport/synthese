
/** AddressField class implementation.
	@file AddressField.cpp

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

#include "AddressField.hpp"

#include "Address.h"
#include "Env.h"
#include "ObjectBase.hpp"
#include "ParametersMap.h"

using namespace boost;
using namespace std;

namespace synthese
{
	template<>
	const Field ComplexObjectFieldDefinition<road::AddressField>::FIELDS[] = {
		Field("road_chunk_id", SQL_INTEGER),
		Field("metric_offset", SQL_DOUBLE),
		Field("number", SQL_INTEGER),
		Field()
	};

	namespace road
	{
		using namespace graph;
		using namespace util;

		bool AddressField::LoadFromRecord(
			Type& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		){
			assert(dynamic_cast<Address*>(&object));
			Address& address(dynamic_cast<Address&>(object));
			bool result(false);

			if(	record.isDefined(FIELDS[0].name) &&
				record.isDefined(FIELDS[1].name)
			){

				// Address
				address.setRoadChunk(NULL);
				address.setMetricOffset(0);
				RegistryKeyType chunkId(
					record.get<RegistryKeyType>(FIELDS[0].name)
				);
				if(chunkId > 0)
				{
					// Road chunk
					try
					{
						MainRoadChunk* value(
							env.getEditable<MainRoadChunk>(chunkId).get()
						);
						if(value != address.getRoadChunk())
						{
							address.setRoadChunk(value);
							result = true;
						}
					}
					catch (ObjectNotFoundException<MainRoadChunk>&)
					{
						Log::GetInstance().warn(
							"Bad value " + lexical_cast<string>(chunkId) + " for projected chunk in stop " + lexical_cast<string>(object.getKey())
						);
					}

					// Metric offset
					{
						MetricOffset value(
							record.get<MetricOffset>(
								FIELDS[1].name
						)	);
						if(value != address.getMetricOffset())
						{
							address.setMetricOffset(value);
							result = true;
						}
					}

					// Geometry generation
					if(address.getRoadChunk())
					{
						address.setGeometry(
							address.getRoadChunk()->getPointFromOffset(
								address.getMetricOffset()
						)	);
					}
				}
			}

			if(	record.isDefined(FIELDS[2].name) &&
				!record.getValue(FIELDS[2].name).empty()
			){
				// House number
				MainRoadChunk::HouseNumber houseNumber(
					record.get<MainRoadChunk::HouseNumber>(
						FIELDS[2].name
				)	);
				address.setHouseNumber(
					houseNumber ?
					optional<MainRoadChunk::HouseNumber>(houseNumber) :
					optional<MainRoadChunk::HouseNumber>()
				);
			}

			return result;
		}



		void AddressField::SaveToFilesMap( const Type& fieldObject, const ObjectBase& object, FilesMap& map )
		{

		}



		void AddressField::SaveToParametersMap( const Type& fieldObject, const ObjectBase& object, util::ParametersMap& map, const std::string& prefix, boost::logic::tribool withFiles )
		{
			if(withFiles == true)
			{
				return;
			}

			assert(dynamic_cast<const Address*>(&object));
			const Address& address(dynamic_cast<const Address&>(object));

			map.insert(prefix + FIELDS[0].name, address.getRoadChunk() ? address.getRoadChunk()->getKey() : RegistryKeyType(0));
			map.insert(prefix + FIELDS[1].name, address.getMetricOffset());
			if(address.getHouseNumber())
			{
				map.insert(prefix + FIELDS[2].name, lexical_cast<string>(*address.getHouseNumber()));
			}
		}



		void AddressField::SaveToDBContent( const Type& fieldObject, const ObjectBase& object, DBContent& content )
		{
			assert(dynamic_cast<const Address*>(&object));
			const Address& address(dynamic_cast<const Address&>(object));

			content.push_back(Cell(address.getRoadChunk() ? address.getRoadChunk()->getKey() : RegistryKeyType(0)));
			content.push_back(Cell(address.getMetricOffset()));
			if(address.getHouseNumber())
			{
				content.push_back(Cell(static_cast<int>(*address.getHouseNumber())));
			}
			else
			{
				content.push_back(Cell(optional<string>()));
			}
		}



		void AddressField::GetLinkedObjectsIds( LinkedObjectsIds& list, const Record& record )
		{
			RegistryKeyType road_chunk_id(record.getDefault<RegistryKeyType>(FIELDS[0].name, 0));
			if(road_chunk_id > 0)
			{
				list.push_back(road_chunk_id);
			}
		}
}	}
