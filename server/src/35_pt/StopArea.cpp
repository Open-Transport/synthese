
/** StopArea class implementation.
	@file StopArea.cpp

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

#include "StopArea.hpp"

#include "AllowedUseRule.h"
#include "CityTableSync.h"
#include "CommercialLine.h"
#include "Crossing.h"
#include "DataSourceLinksField.hpp"
#include "DBConstants.h"
#include "Edge.h"
#include "Env.h"
#include "ForbiddenUseRule.h"
#include "FreeDRTArea.hpp"
#include "DRTArea.hpp"
#include "ImportableTableSync.hpp"
#include "JourneyPattern.hpp"
#include "ParametersMap.h"
#include "PTModule.h"
#include "PTUseRuleTableSync.h"
#include "Registry.h"
#include "RoadChunkEdge.hpp"
#include "RoadModule.h"
#include "StopAreaTableSync.hpp"
#include "StopPoint.hpp"
#include "VertexAccessMap.h"

#include <boost/foreach.hpp>
#include <geos/geom/Envelope.h>

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace boost::posix_time;


namespace synthese
{
	using namespace db;
	using namespace impex;
	using namespace util;
	using namespace graph;
	using namespace pt;
	using namespace road;
	using namespace geography;

	FIELD_DEFINITION_OF_OBJECT(StopArea, "stop_area_id", "stop_area_ids")

	namespace util
	{
		template<> const string Registry<pt::StopArea>::KEY("StopArea");
		template<> const string FactorableTemplate<geography::NamedPlace,pt::StopArea>::FACTORY_KEY("StopArea");
	}

	namespace pt
	{
		const std::string StopArea::DATA_STOP_ID("stop_id");
		const std::string StopArea::DATA_STOP_NAME("stop_name");
		const std::string StopArea::DATA_CITY_NAME("city_name");
		const std::string StopArea::DATA_STOP_NAME_13("stop_name_13");
		const std::string StopArea::DATA_STOP_NAME_26("stop_name_26");
		const std::string StopArea::DATA_STOP_NAME_FOR_TIMETABLES("stop_name_for_timetables");



		StopArea::StopArea(
			util::RegistryKeyType id,
			bool allowedConnection, /*= CONNECTION_TYPE_FORBIDDEN */
			posix_time::time_duration defaultTransferDelay /*= FORBIDDEN_TRANSFER_DELAY  */
		):	Registrable(id),
			NamedPlaceTemplate<StopArea>(),
			_isMainPlaceOfCity(false),
			_allowedConnection(allowedConnection),
			_defaultTransferDelay(defaultTransferDelay),
			_isRelayPark(false)
		{
			RuleUser::Rules rules(RuleUser::GetEmptyRules());
			rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			rules[USER_CAR - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			setRules(rules);
		}


		StopArea::~StopArea()
		{
			unlink();
		}


		void StopArea::addPhysicalStop(
			const StopPoint& physicalStop
		){
			_isoBarycentre.reset();
			_physicalStops.insert(make_pair(physicalStop.getKey(), &physicalStop));
		}



		StopArea::PhysicalStopsLabels StopArea::getPhysicalStopLabels(
			bool withAll /*= false*/,
			string noStopLabel
		) const {

			PhysicalStopsLabels m;

			// All stops item
			if (withAll)
			{
				m.push_back(make_pair(0, "(tous)"));
			}

			// Stop items
			for (PhysicalStops::const_iterator it = _physicalStops.begin(); it != _physicalStops.end(); ++it)
			{
				m.push_back(make_pair(it->first, it->second->getCodeBySources() + " / " + it->second->getName()));
			}

			// No stop item
			if(!noStopLabel.empty())
			{
				m.push_back(make_pair(0, noStopLabel));
			}

			return m;
		}



		StopArea::PhysicalStopsLabels StopArea::getPhysicalStopLabels( const PhysicalStops& noDisplay ) const
		{
			PhysicalStopsLabels m;
			for (PhysicalStops::const_iterator it = _physicalStops.begin(); it != _physicalStops.end(); ++it)
				if (noDisplay.find(it->first) == noDisplay.end())
					m.push_back(make_pair(it->first, it->second->getCodeBySources() + " / " + it->second->getName()));
			return m;
		}



		HubScore StopArea::getScore() const
		{
			typedef map<const CommercialLine*, int> ScoresMap;

			if(	!_score)
			{
				// Default value
				_score = NO_TRANSFER_HUB_SCORE;

				if(_allowedConnection)
				{
					ScoresMap scores;
					BOOST_FOREACH(PhysicalStops::value_type its, _physicalStops)
					{
						BOOST_FOREACH(const Vertex::Edges::value_type& edge, its.second->getDepartureEdges())
						{
							// Jump over junctions
							if(!dynamic_cast<const JourneyPattern*>(edge.second))
							{
								continue;
							}

							const JourneyPattern* route(static_cast<const JourneyPattern*>(edge.first));
							ScoresMap::iterator itl(
								scores.find(route->getCommercialLine())
							);
							if (itl == scores.end())
							{
								scores.insert(make_pair(route->getCommercialLine(), route->getAllServices().size()));
							}
							else
							{
								itl->second += route->getAllServices().size();
							}
						}
					}

					BOOST_FOREACH(ScoresMap::value_type itc, scores)
					{
						if (itc.second <= 10)
						{
							*_score += 1;
						}
						else if (itc.second <= 50)
						{
							*_score += 2;
						}
						else if (itc.second <= 100)
						{
							*_score += 3;
						}
						else
						{
							*_score += 4;
						}
						if (*_score > MAX_HUB_SCORE)
						{
							_score = MAX_HUB_SCORE;
							break;
						}
					}
				}
			}
			return *_score;
		}



		void StopArea::getVertexAccessMap(
			VertexAccessMap& result,
			GraphIdType whatToSearch,
			const Vertex& origin,
			bool vertexIsOrigin
		) const {

			if (whatToSearch != PTModule::GRAPH_ID) return;

			if(vertexIsOrigin)
			{
				BOOST_FOREACH(
					const PhysicalStops::value_type& it,
					_physicalStops
				){

					if(!isConnectionAllowed(origin, *it.second)) continue;

					result.insert(
						it.second,
						VertexAccess(getTransferDelay(origin, *it.second))
					);
				}
			} else {
				BOOST_FOREACH(
					const PhysicalStops::value_type& it,
					_physicalStops
				){

					if(!isConnectionAllowed(*it.second, origin)) continue;

					result.insert(
						it.second,
						VertexAccess(getTransferDelay(*it.second, origin))
					);
				}
			}
		}



		void StopArea::getVertexAccessMap(
			VertexAccessMap& result,
			const AccessParameters& accessParameters,
			const geography::Place::GraphTypes& whatToSearch
		) const {
			// If the VAM uses road, append the crossings around each physical stops of the StopArea
			if(whatToSearch.find(RoadModule::GRAPH_ID) != whatToSearch.end())
			{
				BOOST_FOREACH(
					const PhysicalStops::value_type& it,
					_physicalStops
				){
					if(!it.second->getProjectedPoint().getRoadChunk())
					{
						// This physical stop is not projected on a road chunk, we cannot retrieve its crossings
						continue;
					}

					// The metric offset of the projected point is expressed from the start of its road chunk
					MetricOffset distanceFromChunkStart = it.second->getProjectedPoint().getMetricOffset();

					// Insert the crossing of the road chunk (= starting point of the road chunk) into the VAM
					result.insert(
						it.second->getProjectedPoint().getRoadChunk()->getFromCrossing(),
						VertexAccess(
							minutes(static_cast<long>(ceil(distanceFromChunkStart / 50.0))),
							distanceFromChunkStart
					)	);

					// If the next edge exist add the next crossing to the VAM (see issue #23315)
					if(it.second->getProjectedPoint().getRoadChunk()->getForwardEdge().getNext())
					{
						// The metric offset of the road chunk start and end are expressed from the start of the road
						MetricOffset chunkStartOffset     = it.second->getProjectedPoint().getRoadChunk()->getMetricOffset();
						MetricOffset chunkEndOffset       = it.second->getProjectedPoint().getRoadChunk()->getForwardEdge().getEndMetricOffset();

						// The distance between the chunk end and the stop projection is : chunk size - offset of projected point
						MetricOffset distanceFromChunkEnd = (chunkEndOffset - chunkStartOffset) - distanceFromChunkStart;

						// Insert the next crossing (= ending point of the road chunk) into the VAM
						result.insert(
							it.second->getProjectedPoint().getRoadChunk()->getForwardEdge().getNext()->getFromVertex(),
							VertexAccess(
								minutes(static_cast<long>(ceil(distanceFromChunkEnd / 50.0))),
								distanceFromChunkEnd
						)	);
					}
				}
			}

			// If the VAM uses public transportation, append each physical stops of the StopArea
			if(whatToSearch.find(PTModule::GRAPH_ID) != whatToSearch.end())
			{
				BOOST_FOREACH(
					const PhysicalStops::value_type& it,
					_physicalStops
				){
					result.insert(
						it.second,
						VertexAccess()
					);
				}
			}
		}



		const boost::shared_ptr<Point>& StopArea::getPoint() const
		{
			if (_location.get())
			{
				return _location;
			}
			if (!_isoBarycentre.get())
			{
				Envelope e;
				BOOST_FOREACH(const PhysicalStops::value_type& it, _physicalStops)
				{
					if(it.second->hasGeometry())
					{
						e.expandToInclude(*it.second->getGeometry()->getCoordinate());
					}
				}
				if(!e.isNull())
				{
					Coordinate c;
					e.centre(c);
					_isoBarycentre.reset(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(c));
				}
			}
			return _isoBarycentre;
		}



		bool StopArea::containsAnyVertex(
			GraphIdType graphType
		) const	{
			if(graphType == PTModule::GRAPH_ID)
			{
				return !_physicalStops.empty();
			}
			if(graphType == RoadModule::GRAPH_ID)
			{
//				return !_addresses.empty(); @todo Station entrances
			}
			return false;
		}



		std::string StopArea::getNameForAllPlacesMatcher(
			std::string text
		) const	{
			return (text.empty() ? getName() : text);
		}



		bool StopArea::isConnectionPossible() const
		{
			return _allowedConnection;
		}



		bool StopArea::isConnectionAllowed( const graph::Vertex& fromVertex , const graph::Vertex& toVertex ) const
		{
			return !getTransferDelay(fromVertex, toVertex).is_not_a_date_time();
		}

		bool StopArea::isConnectionAllowed( util::RegistryKeyType fromId, util::RegistryKeyType toId ) const
		{
			return !getTransferDelay(fromId, toId).is_not_a_date_time();
		}



		boost::posix_time::time_duration StopArea::getTransferDelay(
			const graph::Vertex& fromVertex,
			const graph::Vertex& toVertex
		) const	{
			return getTransferDelay(fromVertex.getKey(), toVertex.getKey());
		}



		boost::posix_time::time_duration StopArea::getTransferDelay( util::RegistryKeyType fromId, util::RegistryKeyType toId ) const
		{
			TransferDelaysMap::const_iterator it(
				_transferDelays.find(make_pair(fromId, toId))
			);

			// If not defined in map, return default transfer delay
			return (it == _transferDelays.end ()) ? _defaultTransferDelay : it->second;
		}



		boost::posix_time::time_duration StopArea::getMinTransferDelay() const
		{
			if (_minTransferDelay.is_not_a_date_time())
			{
				_minTransferDelay = _defaultTransferDelay;
				for (TransferDelaysMap::const_iterator it(_transferDelays.begin()); it != _transferDelays.end(); ++it)
					if (it->second < _minTransferDelay)
						_minTransferDelay = it->second;
			}
			return _minTransferDelay;
		}



		void StopArea::_addTransferDelay(
			TransferDelaysMap& map,
			TransferDelaysMap::key_type::first_type fromVertex,
			TransferDelaysMap::key_type::second_type toVertex,
			boost::posix_time::time_duration transferDelay
		){
			assert(transferDelay >= minutes(0) && !transferDelay.is_not_a_date_time());

			map[std::make_pair (fromVertex, toVertex)] = transferDelay;
		}



		void StopArea::_addForbiddenTransferDelay(
			TransferDelaysMap& map,
			TransferDelaysMap::key_type::first_type fromVertex,
			TransferDelaysMap::key_type::second_type toVertex
		){
			map[std::make_pair (fromVertex, toVertex)] = posix_time::time_duration(not_a_date_time);
		}



		void StopArea::setDefaultTransferDelay( boost::posix_time::time_duration defaultTransferDelay )
		{
			assert(defaultTransferDelay >= minutes(0) && !defaultTransferDelay.is_not_a_date_time());

			_defaultTransferDelay = defaultTransferDelay;
			_minTransferDelay = posix_time::time_duration(not_a_date_time);
		}



		void StopArea::removePhysicalStop( const pt::StopPoint& physicalStop )
		{
			_isoBarycentre.reset();
			_physicalStops.erase(physicalStop.getKey());
		}



		StopArea::Lines StopArea::getLines(
			bool includeArrivals
		) const {
			Lines result;
			BOOST_FOREACH(const PhysicalStops::value_type& itStop, _physicalStops)
			{
				BOOST_FOREACH(const Vertex::Edges::value_type& itEdge, itStop.second->getDepartureEdges())
				{
					if(!dynamic_cast<const JourneyPattern*>(itEdge.first))
					{
						continue;
					}
					CommercialLine* line(
						static_cast<const JourneyPattern*>(itEdge.first)->getCommercialLine()
					);
					if(line)
					{
						result.insert(line);
					}
				}
				if(includeArrivals)
				{
					BOOST_FOREACH(const Vertex::Edges::value_type& itEdge, itStop.second->getArrivalEdges())
					{
						if(!dynamic_cast<const JourneyPattern*>(itEdge.first))
						{
							continue;
						}
						{
							result.insert(static_cast<const JourneyPattern*>(itEdge.first)->getCommercialLine());
						}
					}
				}
			}
			return result;
		}



		void StopArea::toParametersMap(
			util::ParametersMap& pm,
			const CoordinatesSystem* coordinatesSystem,
			string prefix
		) const {
			pm.insert(
				prefix + TABLE_COL_ID,
				getKey()
			); // For StopAreasList compatibility
			pm.insert(
				prefix + StopAreaTableSync::TABLE_COL_NAME,
				getName()
			);
			if(getCity())
			{
				pm.insert(
					prefix + StopAreaTableSync::TABLE_COL_CITYID,
					getCity()->get<Key>()
				);
			}
			pm.insert(
				prefix + StopAreaTableSync::TABLE_COL_CONNECTIONTYPE,
				getAllowedConnection()
			);
			pm.insert(
				prefix + StopAreaTableSync::TABLE_COL_ISCITYMAINCONNECTION,
				_isMainPlaceOfCity
			);
			pm.insert(
				prefix + StopAreaTableSync::TABLE_COL_DEFAULTTRANSFERDELAY,
				getDefaultTransferDelay().total_seconds() / 60
			);
			pm.insert(
				prefix + StopAreaTableSync::TABLE_COL_ISRELAYPARK,
				_isRelayPark
			);
			pm.insert(
				prefix + StopAreaTableSync::TABLE_COL_TRANSFERDELAYS,
				StopArea::SerializeTransferDelaysMatrix(getTransferDelays())
			);
			pm.insert(
				prefix + StopAreaTableSync::COL_NAME13,
				getName13()
			);
			pm.insert(
				prefix + StopAreaTableSync::COL_NAME26,
				getName26()
			);
			pm.insert(
				prefix + StopAreaTableSync::COL_CODE_BY_SOURCE,
				impex::DataSourceLinks::Serialize(getDataSourceLinks())
			);
			pm.insert(
				prefix + StopAreaTableSync::COL_TIMETABLE_NAME,
				getTimetableName()
			);
			pm.insert(
				prefix + StopAreaTableSync::COL_HANDICAPPED_COMPLIANCE_ID,
				(	(getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(getRule(USER_HANDICAPPED))) ?
					static_cast<const PTUseRule*>(getRule(USER_HANDICAPPED))->getKey() :
					RegistryKeyType(0)
			)	);
			if(getLocation())
			{
				boost::shared_ptr<geos::geom::Geometry> projected(getLocation());
				if(	CoordinatesSystem::GetStorageCoordinatesSystem().getSRID() !=
					static_cast<CoordinatesSystem::SRID>(getLocation()->getSRID())
				){
					projected = CoordinatesSystem::GetStorageCoordinatesSystem().convertGeometry(*getLocation());
				}

				geos::io::WKTWriter writer;
				pm.insert(
					prefix + TABLE_COL_GEOMETRY,
					writer.write(projected.get())
				);
			}

			pm.insert(prefix + DATA_STOP_ID, getKey());
			pm.insert(prefix + DATA_STOP_NAME, getName());
			pm.insert(prefix + "name", getName()); // For StopAreasList compatibility
			pm.insert(prefix + DATA_STOP_NAME_13, getName13());
			pm.insert(prefix + DATA_STOP_NAME_26, getName26());
			pm.insert(prefix + "directionAlias", getName26()); // For StopAreasList compatibility
			pm.insert(prefix + DATA_STOP_NAME_FOR_TIMETABLES, getTimetableName());
			if(getCity())
			{
				pm.insert(prefix + "cityId", getCity()->getKey()); // For StopAreasList compatibility
				pm.insert(prefix + DATA_CITY_NAME, getCity()->getName());
				pm.insert(prefix + "cityName", getCity()->getName()); // For StopAreasList compatibility
			}
			if(coordinatesSystem && getPoint())
			{
				boost::shared_ptr<Point> pg(
					coordinatesSystem->convertPoint(*getPoint())
				);
				{
					stringstream s;
					s << std::fixed << pg->getX();
					pm.insert(prefix + StopAreaTableSync::COL_X, s.str());
				}
				{
					stringstream s;
					s << std::fixed << pg->getY();
					pm.insert(prefix + StopAreaTableSync::COL_Y, s.str());
				}
				pm.setGeometry(static_pointer_cast<Geometry,Point>(getPoint()));
			}
		}



		void StopArea::toParametersMap( util::ParametersMap& pm, bool withAdditionalParameters, boost::logic::tribool withFiles /*= boost::logic::indeterminate*/, std::string prefix /*= std::string() */ ) const
		{
			toParametersMap(
				pm,
				&CoordinatesSystem::GetInstanceCoordinatesSystem(),
				prefix
			);
		}



		std::string StopArea::getRuleUserName() const
		{
			return "Zone d'arrêt";
		}



		StopArea::FreeDRTs StopArea::getFreeDRTs() const
		{
			FreeDRTs result;
			BOOST_FOREACH(const FreeDRTArea::Registry::value_type& item, Env::GetOfficialEnv().getRegistry<FreeDRTArea>())
			{
				if(item.second->includesPlace(*this))
				{
					result.insert(item.second.get());
				}
			}
			return result;
		}



		bool StopArea::isInDRT() const
		{
			return !_drtAreas.empty();
		}



		Hub::Vertices StopArea::getVertices(
			GraphIdType graphId
		) const	{
			Vertices result;
			if(graphId == PTModule::GRAPH_ID)
			{
				BOOST_FOREACH(const PhysicalStops::value_type& it, _physicalStops)
				{
					result.push_back(it.second);
				}
			}
			return result;
		}



		bool StopArea::loadFromRecord( const Record& record, util::Env& env )
		{
			bool result(false);

			// Name
			if(record.isDefined(StopAreaTableSync::TABLE_COL_NAME))
			{
				string name(
					record.get<string>(StopAreaTableSync::TABLE_COL_NAME)
				);
				trim(name);
				if(name != getName())
				{
					setName(name);
					result = true;
				}
			}

			// Name 13
			if(record.isDefined(StopAreaTableSync::COL_NAME13))
			{
				string name13(
					record.get<string>(StopAreaTableSync::COL_NAME13)
				);
				if(name13 != getName13())
				{
					setName13(name13);
					result = true;
				}
			}

			// Name 26
			if(record.isDefined(StopAreaTableSync::COL_NAME26))
			{
				string name26(
					record.get<string>(StopAreaTableSync::COL_NAME26)
				);
				if(name26 != getName26())
				{
					setName26(name26);
					result = true;
				}
			}

			// Transfer is allowed
			if(record.isDefined(StopAreaTableSync::TABLE_COL_CONNECTIONTYPE))
			{
				bool connectionType(
					record.getDefault<bool>(
						StopAreaTableSync::TABLE_COL_CONNECTIONTYPE,
						true
				)	);
				if(connectionType != getAllowedConnection())
				{
					setAllowedConnection(connectionType);
					result = true;
				}
			}
	
			// Default transfer duration
			if(record.isDefined(StopAreaTableSync::TABLE_COL_DEFAULTTRANSFERDELAY))
			{
				posix_time::time_duration defaultTransferDelay(
					posix_time::minutes(
						record.getDefault<long>(
							StopAreaTableSync::TABLE_COL_DEFAULTTRANSFERDELAY,
							2
				)	)	);
				if(defaultTransferDelay != getDefaultTransferDelay())
				{
					setDefaultTransferDelay(defaultTransferDelay);
					result = true;
				}
			}
			
			// Is relay park
			if (record.isDefined(StopAreaTableSync::TABLE_COL_ISRELAYPARK))
			{
				bool isRelayPark(
					record.getDefault<bool>(
						StopAreaTableSync::TABLE_COL_ISRELAYPARK,
						false
				)   );
				if (isRelayPark != getIsRelayPark())
				{
					setIsRelayPark(isRelayPark);
					result = true;
				}
			}

			// Timetable name
			if(record.isDefined(StopAreaTableSync::COL_TIMETABLE_NAME))
			{
				string value(
					record.get<string>(StopAreaTableSync::COL_TIMETABLE_NAME)
				);
				if(value != getTimetableName())
				{
					setTimetableName(value);
					result = true;
				}
			}

			// Transfer delay matrix
			if(record.isDefined(StopAreaTableSync::TABLE_COL_TRANSFERDELAYS))
			{
				string transferDelaysStr(
					record.get<string>(StopAreaTableSync::TABLE_COL_TRANSFERDELAYS)
				);
				TransferDelaysMap value;
				typedef tokenizer<char_separator<char> > tokenizer;
				char_separator<char> sep1 (",");
				char_separator<char> sep2 (":");
				tokenizer tripletTokens (transferDelaysStr, sep1);
				for (tokenizer::iterator tripletIter = tripletTokens.begin();
					tripletIter != tripletTokens.end (); ++tripletIter)
				{
					tokenizer valueTokens (*tripletIter, sep2);
					tokenizer::iterator valueIter = valueTokens.begin();

					// departureRank:arrivalRank:transferDelay
					RegistryKeyType startStop(lexical_cast<RegistryKeyType>(*valueIter));
					RegistryKeyType endStop(lexical_cast<RegistryKeyType>(*(++valueIter)));
					const string delay(*(++valueIter));
					if(delay == StopAreaTableSync::FORBIDDEN_DELAY_SYMBOL)
					{
						_addForbiddenTransferDelay(value, startStop, endStop);
					}
					else
					{
						time_duration duration(posix_time::minutes(lexical_cast<long>(delay)));
						_addTransferDelay(value, startStop, endStop, duration);
					}
				}
				if(value != _transferDelays)
				{
					setTransferDelaysMatrix(value);
					result = true;
				}
			}

//			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
//			{
				// City
				if(record.isDefined(StopAreaTableSync::TABLE_COL_CITYID))
				{
					City* value(NULL);
					RegistryKeyType cityId(
						record.getDefault<RegistryKeyType>(StopAreaTableSync::TABLE_COL_CITYID, 0)
					);
					if(cityId > 0) try
					{
						value = CityTableSync::GetEditable(cityId, env).get();
					}
					catch(ObjectNotFoundException<City>&)
					{
						Log::GetInstance().warn("Bad value " + lexical_cast<string>(cityId) + " for city in stop area " + lexical_cast<string>(getKey()));
					}
					if(value != getCity())
					{
						setCity(value);
						result = true;
					}
				}

				// City main connexion
				if(record.isDefined(StopAreaTableSync::TABLE_COL_ISCITYMAINCONNECTION))
				{
					bool value(record.getDefault<bool>(StopAreaTableSync::TABLE_COL_ISCITYMAINCONNECTION, false));
					if(value != _isMainPlaceOfCity)
					{
						_isMainPlaceOfCity = value;
						result = true;
					}
				}

				// Handicapped compliance
				if(record.isDefined(StopAreaTableSync::COL_HANDICAPPED_COMPLIANCE_ID))
				{
					RuleUser::Rules rules(getRules());
					rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
					rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
					RegistryKeyType handicappedComplianceId(
						record.getDefault<RegistryKeyType>(
							StopAreaTableSync::COL_HANDICAPPED_COMPLIANCE_ID,
							0
					)	);
					if(handicappedComplianceId > 0)
					{
						try
						{
							rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(handicappedComplianceId, env).get();
						}
						catch(ObjectNotFoundException<PTUseRule>&)
						{
							Log::GetInstance().warn("Bad value " + lexical_cast<string>(handicappedComplianceId) + " for handicapped compliance in stop area " + lexical_cast<string>(getKey()));
					}	}
					if(rules != getRules())
					{
						setRules(rules);
						result = true;
					}
				}

				// Position : Lon/lat prior to x/y
				if(record.isDefined(TABLE_COL_GEOMETRY))
				{
					boost::shared_ptr<Point> point;
					if(!record.get<string>(TABLE_COL_GEOMETRY).empty())
					{
						point =	dynamic_pointer_cast<Point, Geometry>(
							record.getGeometryFromWKT(TABLE_COL_GEOMETRY)
						);
					}
					else if(
						record.getDefault<double>(StopAreaTableSync::COL_X, 0) > 0 &&
						record.getDefault<double>(StopAreaTableSync::COL_Y, 0) > 0
					){
						point = CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(
							record.getDefault<double>(StopAreaTableSync::COL_X, 0),
							record.getDefault<double>(StopAreaTableSync::COL_Y, 0)
						);
					}
					if(	(!point.get() && getLocation().get()) ||
						(point.get() && !getLocation().get()) ||
						(point.get() && getLocation().get() && !point->equalsExact(getLocation().get(), 0.01))
					){
						setLocation(point);
						result = true;
					}
				}

				// Data source links (at the end of the load to avoid registration of objects which are removed later by an exception)
				if(record.isDefined(StopAreaTableSync::COL_CODE_BY_SOURCE))
				{
					Importable::DataSourceLinks value(
						ImportableTableSync::GetDataSourceLinksFromSerializedString(
							record.get<string>(StopAreaTableSync::COL_CODE_BY_SOURCE),
							env
					)	);
					if(value != getDataSourceLinks())
					{
						if(&env == &Env::GetOfficialEnv())
						{
							setDataSourceLinksWithRegistration(value);
						}
						else
						{
							setDataSourceLinksWithoutRegistration(value);
						}
						result = true;
					}
				}
//			}

			return result;
		}



		synthese::SubObjects StopArea::getSubObjects() const
		{
			SubObjects r;
			BOOST_FOREACH(const PhysicalStops::value_type& stop, getPhysicalStops())
			{
				r.push_back(const_cast<StopPoint*>(stop.second));
			}
			return r;
		}



		std::string StopArea::SerializeTransferDelaysMatrix( const TransferDelaysMap& matrix )
		{
			stringstream delays;
			bool first(true);
			BOOST_FOREACH(const StopArea::TransferDelaysMap::value_type& td, matrix)
			{
				if(!first) delays << ",";
				delays << td.first.first << ":" << td.first.second << ":";
				if(td.second.is_not_a_date_time())
				{
					delays << StopAreaTableSync::FORBIDDEN_DELAY_SYMBOL;
				}
				else
				{
					delays << (td.second.total_seconds() / 60);
				}
				first = false;
			}
			return delays.str();
		}



		synthese::LinkedObjectsIds StopArea::getLinkedObjectsIds( const Record& record ) const
		{
			LinkedObjectsIds result;

			// City Id
			RegistryKeyType cityId(
				record.getDefault<RegistryKeyType>(StopAreaTableSync::TABLE_COL_CITYID, 0)
			);
			if(cityId > 0)
			{
				result.push_back(cityId);
			}

			// Handicapped use rule
			RegistryKeyType handicappedComplianceId(
				record.getDefault<RegistryKeyType>(
					StopAreaTableSync::COL_HANDICAPPED_COMPLIANCE_ID,
					0
			)	);
			if(handicappedComplianceId > 0)
			{
				result.push_back(handicappedComplianceId);
			}

			return result;
		}



		void StopArea::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(getCity())
			{
				// Registration to city places matcher
				getCity()->addPlaceToMatcher(env.getEditableSPtr(this));

				if(	&env == &Env::GetOfficialEnv()
				){
					// Registration to all places matcher
					GeographyModule::GetGeneralAllPlacesMatcher().add(
						getFullName(),
						env.getEditableSPtr(this)
					);

					// Registration to road places matcher
					PTModule::GetGeneralStopsMatcher().add(
						getFullName(),
						env.getEditableSPtr(this)
					);
				}
			}

			if(&env == &Env::GetOfficialEnv())
			{
				setDataSourceLinksWithRegistration(getDataSourceLinks());
			}

			if(getCity())
			{
				if (_isMainPlaceOfCity)
				{
					const_cast<City*>(getCity())->addIncludedPlace(*this);
				}
				else
				{
					const_cast<City*>(getCity())->removeIncludedPlace(*this);
				}
			}
		}



		void StopArea::unlink()
		{
			// unregister from each DRTArea
			BOOST_FOREACH(DRTArea* drtArea, getDRTAreas())
			{
				drtArea->removeStopArea(this);
			}
		}



		void StopArea::setTransferDelaysMatrix( const TransferDelaysMap& value )
		{
			_transferDelays = value;
			_minTransferDelay = posix_time::time_duration(not_a_date_time);
		}



		void StopArea::addDRTArea( const DRTArea& area )
		{
			_drtAreas.insert(const_cast<DRTArea*>(&area));
		}



		void StopArea::removeDRTArea( const DRTArea& area )
		{
			_drtAreas.erase(const_cast<DRTArea*>(&area));
		}
}	}
