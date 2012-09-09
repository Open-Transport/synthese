
/** Address class implementation.
	@file Address.cpp

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

#include "Address.h"

#include "AccessParameters.h"
#include "Env.h"
#include "ObjectBase.hpp"
#include "ParametersMap.h"
#include "Record.hpp"
#include "ReverseRoadChunk.hpp"
#include "Road.h"
#include "RoadModule.h"
#include "VertexAccessMap.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace geos::geom;

namespace synthese
{
	using namespace geography;
	using namespace graph;
	using namespace road;
	using namespace util;

	namespace road
	{
		Address::Address (
			MainRoadChunk& roadChunk,
			double metricOffset,
			optional<MainRoadChunk::HouseNumber> houseNumber
		):	WithGeometry<Point>(
				roadChunk.getGeometry().get() ?
				roadChunk.getPointFromOffset(metricOffset) :
				boost::shared_ptr<geos::geom::Point>()
			),
			_roadChunk(&roadChunk),
			_metricOffset(metricOffset),
			_houseNumber(houseNumber)
		{
		}



		Address::Address()
		:	_roadChunk(NULL),
			_metricOffset(0)
		{
		}


		Address::~Address()
		{
		}



		void Address::getVertexAccessMap(
			graph::VertexAccessMap& result,
			const graph::AccessParameters& accessParameters,
			const Place::GraphTypes& whatToSearch
		) const	{
			if(whatToSearch.find(RoadModule::GRAPH_ID) != whatToSearch.end())
			{
				// Chunk linked with the house
				{
					double distance(_metricOffset - _roadChunk->getMetricOffset());
					result.insert(
						_roadChunk->getFromVertex(),
						VertexAccess(
							seconds(static_cast<long>(distance / accessParameters.getApproachSpeed())),
							distance
					)	);
				}

				// Reverse chunk
				if(_roadChunk->getNext())
				{
					assert(static_cast<MainRoadChunk*>(_roadChunk->getNext())->getReverseRoadChunk());

					double distance(_roadChunk->getEndMetricOffset() - _metricOffset);
					result.insert(
						static_cast<MainRoadChunk*>(_roadChunk->getNext())->getReverseRoadChunk()->getFromVertex(),
						VertexAccess(
							seconds(static_cast<long>(distance / accessParameters.getApproachSpeed())),
							distance
					)	);
				}
			}
		}
	}

	template<> const Field ComplexObjectFieldDefinition<AddressData>::FIELDS[] = {
		Field("road_chunk_id", SQL_INTEGER),
		Field("metric_offset", SQL_DOUBLE),
		Field("number", SQL_INTEGER),
		Field()
	};



	template<> void ComplexObjectField<AddressData, AddressData::Type>::GetLinkedObjectsIds(
		LinkedObjectsIds& list,
		const Record& record
	){
		RegistryKeyType road_chunk_id(record.getDefault<RegistryKeyType>(FIELDS[0].name, 0));
		if(road_chunk_id > 0)
		{
			list.push_back(road_chunk_id);
		}
	}



	template<>
	void ComplexObjectField<AddressData, AddressData::Type>::SaveToFilesMap(
		const AddressData::Type& fieldObject,
		const ObjectBase& object,
		FilesMap& map
	){
	}



	template<>
	void ComplexObjectField<AddressData, AddressData::Type>::LoadFromRecord(
		AddressData::Type& fieldObject,
		ObjectBase& object,
		const Record& record,
		const Env& env
	){
		assert(dynamic_cast<Address*>(&object));
		Address& address(dynamic_cast<Address&>(object));

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
					address.setRoadChunk(
						env.getEditable<MainRoadChunk>(chunkId).get()
					);
				}
				catch (ObjectNotFoundException<MainRoadChunk>&)
				{
					Log::GetInstance().warn(
						"Bad value " + lexical_cast<string>(chunkId) + " for projected chunk in stop " + lexical_cast<string>(object.getKey())
					);
				}

				// Metric offset
				address.setMetricOffset(
					record.get<MetricOffset>(
						FIELDS[1].name
				)	);

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
	}



	template<>
	void ComplexObjectField<AddressData, AddressData::Type>::SaveToParametersMap(
		const AddressData::Type& fieldObject,
		const ObjectBase& object,
		util::ParametersMap& map,
		const std::string& prefix,
		boost::logic::tribool withFiles
	){
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
}
