////////////////////////////////////////////////////////////////////////////////
/// DisplayScreenTableSync class implementation.
///	@file DisplayScreenTableSync.cpp
///	@author Hugues Romain
///	@date 2008-12-21 20:16
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "DisplayScreenTableSync.h"

#include "AlarmObjectLinkTableSync.h"
#include "AlarmTableSync.h"
#include "DataSourceLinksField.hpp"
#include "DeparturesTableModule.h"
#include "DisplayTypeTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "PlaceWithDisplayBoards.hpp"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "Session.h"
#include "SQLSingleOperatorExpression.hpp"
#include "User.h"
#include "StopAreaTableSync.hpp"
#include "CommercialLineTableSync.h"
#include "CityTableSync.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "StopPointTableSync.hpp"
#include "SentAlarm.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenCPUTableSync.h"
#include "ScenarioTableSync.h"
#include "ArrivalDepartureTableLog.h"
#include "ArrivalDepartureTableRight.h"
#include "AlarmObjectLinkTableSync.h"
#include "ImportableTableSync.hpp"

#include <sstream>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::algorithm;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace departure_boards;
	using namespace pt;
	using namespace security;
	using namespace messages;
	using namespace geography;
	using namespace impex;


	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync, DisplayScreenTableSync>::FACTORY_KEY("54.50 Display Screens");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DisplayScreenTableSync>::TABLE(
			"t041_display_screens"
		);

		template<> const Field DBTableSyncTemplate<DisplayScreenTableSync>::_FIELDS[] =
		{
			Field()
		};

		template<> DBTableSync::Indexes DBTableSyncTemplate<DisplayScreenTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(BroadCastPoint::FIELD.name.c_str(), "")
			);
			r.push_back(
				DBTableSync::Index(MacAddress::FIELD.name.c_str(), "")
			);
			return r;
		}


			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				// Displayed place
				RegistryKeyType placeId(rows->getLongLong( DisplayScreenTableSync::COL_PLACE_ID));
				if(placeId != 0) try
				{
					object->setDisplayedPlace(StopAreaTableSync::Get(placeId, env, linkLevel).get());
				}
				catch(ObjectNotFoundException<StopArea>&)
				{
					Log::GetInstance().warn(
						"Data corrupted in "+ DisplayScreenTableSync::TABLE.NAME + " on display screen "+ lexical_cast<string>(object->getKey()) +" : location "+ lexical_cast<string>(placeId) + " not found"
					);
				}

				// Stop point location
				RegistryKeyType stopId(
					rows->getDefault<RegistryKeyType>(DisplayScreenTableSync::COL_STOP_POINT_LOCATION, 0)
				);
				if(stopId) try
				{
					object->setStopPointLocation(
						StopPointTableSync::GetEditable(stopId, env, linkLevel).get()
					);
				}
				catch(ObjectNotFoundException<StopPoint>&)
				{
					Log::GetInstance().warn(
						"Data corrupted in "+ DisplayScreenTableSync::TABLE.NAME + " on display screen "+ lexical_cast<string>(object->getKey()) +" : stop point location "+ lexical_cast<string>(stopId) + " not found"
					);
				}

				// Line filter
				LineFilter lineFilter(
					DisplayScreenTableSync::UnserializeLineFilter(
						rows->getText(DisplayScreenTableSync::COL_ALLOWED_LINES_IDS),
						env,
						linkLevel
				)	);
				object->setAllowedLines(lineFilter);

				// Up & root
				RegistryKeyType upId(rows->getLongLong(DisplayScreenTableSync::COL_UP_ID));
				if(upId > 0) try
				{
					object->setParent(DisplayScreenTableSync::GetEditable(upId, env, linkLevel).get());
				}
				catch(ObjectNotFoundException<DisplayScreen>&)
				{
					Log::GetInstance().warn(
						"Data corrupted in "+ DisplayScreenTableSync::TABLE.NAME + " on display screen : up display screen " +
						lexical_cast<string>(upId) + " not found"
					);
				}
				else
				{
					// CPU
					RegistryKeyType cpuId(rows->getLongLong(DisplayScreenTableSync::COL_CPU_HOST_ID));
					if (cpuId > 0) try
					{
						object->setRoot(DisplayScreenCPUTableSync::GetEditable(cpuId, env, linkLevel).get());
						object->setParent(NULL);
					}
					catch(ObjectNotFoundException<StopArea>&)
					{
						Log::GetInstance().warn(
							"Data corrupted in "+ DisplayScreenTableSync::TABLE.NAME + " on display screen : cpu host " +
							lexical_cast<string>(cpuId) + " not found"
						);
					}
					else
					{
						object->setRoot(
							DeparturesTableModule::GetPlaceWithDisplayBoards(
								object->getDisplayedPlace(),
								env
						)	);
						object->setParent(NULL);
					}
				}

				object->registerInParentOrRoot();

				// Type
				RegistryKeyType typeId(rows->getLongLong ( DisplayScreenTableSync::COL_TYPE_ID));
				if (typeId > 0) try
				{
					object->setType(DisplayTypeTableSync::Get(typeId, env, linkLevel).get());
				}
				catch(ObjectNotFoundException<DisplayType>&)
				{
					Log::GetInstance().warn(
						"Data corrupted in "+ DisplayScreenTableSync::TABLE.NAME + " on display screen "+ lexical_cast<string>(object->getKey()) +" : type " +
						lexical_cast<string>(typeId) + " not found"
					);
				}

				// Physical stops
				vector<string> stops = Conversion::ToStringVector(rows->getText ( DisplayScreenTableSync::COL_PHYSICAL_STOPS_IDS));
				ArrivalDepartureTableGenerator::PhysicalStops pstops;
				BOOST_FOREACH(const string& stop, stops)
				{
					try
					{
						RegistryKeyType id(lexical_cast<RegistryKeyType>(stop));
						pstops.insert(
							make_pair(
								id,
								StopPointTableSync::Get(id, env, linkLevel).get()
						)	);
					}
					catch (ObjectNotFoundException<StopPoint>&)
					{
						Log::GetInstance().warn("Data corrupted in " + DisplayScreenTableSync::TABLE.NAME + "/" + DisplayScreenTableSync::COL_PHYSICAL_STOPS_IDS +" "+ lexical_cast<string>(object->getKey()));
					}
				}
				object->setStops(pstops);

				// Forbidden places
				stops = Conversion::ToStringVector (rows->getText (DisplayScreenTableSync::COL_FORBIDDEN_ARRIVAL_PLACES_IDS));
				BOOST_FOREACH(const string& stop, stops)
				{
					try
					{
						object->addForbiddenPlace(StopAreaTableSync::Get(lexical_cast<RegistryKeyType>(stop), env, linkLevel).get());
					}
					catch (ObjectNotFoundException<StopArea>& e)
					{
						Log::GetInstance().warn("Data corrupted in " + DisplayScreenTableSync::TABLE.NAME + "/" + DisplayScreenTableSync::COL_FORBIDDEN_ARRIVAL_PLACES_IDS +" "+ lexical_cast<string>(object->getKey()), e);
					}
				}

				// Displayed places
				stops = Conversion::ToStringVector (rows->getText (DisplayScreenTableSync::COL_DISPLAYED_PLACES_IDS));
				BOOST_FOREACH(const string& stop, stops)
				{
					try
					{
						object->addDisplayedPlace(StopAreaTableSync::Get(lexical_cast<RegistryKeyType>(stop), env, linkLevel).get());
					}
					catch (ObjectNotFoundException<StopArea>& e)
					{
						Log::GetInstance().warn("Data corrupted in " + DisplayScreenTableSync::TABLE.NAME + "/" + DisplayScreenTableSync::COL_DISPLAYED_PLACES_IDS +"  "+ lexical_cast<string>(object->getKey()), e);
					}
				}

				// Forced destinations
				stops = Conversion::ToStringVector (rows->getText ( DisplayScreenTableSync::COL_FORCED_DESTINATIONS_IDS));
				BOOST_FOREACH(const string& stop, stops)
				{
					try
					{
						object->addForcedDestination(StopAreaTableSync::Get(lexical_cast<RegistryKeyType>(stop), env, linkLevel).get());
					}
					catch (ObjectNotFoundException<StopArea>& e)
					{
						Log::GetInstance().warn("Data corrupted in " + DisplayScreenTableSync::TABLE.NAME + "/" + DisplayScreenTableSync::COL_FORCED_DESTINATIONS_IDS +"  "+ lexical_cast<string>(object->getKey()), e);
				}	}

				// Transfers
				stops = Conversion::ToStringVector(rows->getText(DisplayScreenTableSync::COL_TRANSFER_DESTINATIONS));
				BOOST_FOREACH(const string& stop, stops)
				{
					typedef tokenizer<char_separator<char> > tokenizer;
					tokenizer tokens (stop, char_separator<char>(":"));
					tokenizer::iterator it(tokens.begin());
					string id1(*it);
					++it;
					string id2(*it);

					try
					{
						object->addTransferDestination(
							StopAreaTableSync::Get(lexical_cast<RegistryKeyType>(id1), env, linkLevel).get(),
							StopAreaTableSync::Get(lexical_cast<RegistryKeyType>(id2), env, linkLevel).get()
						);
					}
					catch (ObjectNotFoundException<StopArea>& e)
					{
						Log::GetInstance().warn("Data corrupted in " + DisplayScreenTableSync::TABLE.NAME + "/" + DisplayScreenTableSync::COL_FORCED_DESTINATIONS_IDS +"  "+ lexical_cast<string>(object->getKey()), e);
					}
				}

				// Data source links (at the end of the load to avoid registration of objects which are removed later by an exception)
				Importable::DataSourceLinks links(
					ImportableTableSync::GetDataSourceLinksFromSerializedString(
						rows->getText(DisplayScreenTableSync::COL_DATASOURCE_LINKS),
						env
				)	);
				object->setDataSourceLinksWithRegistration(links);
		}	}



		template<> void OldLoadSavePolicy<DisplayScreenTableSync,DisplayScreen>::Unlink(
			DisplayScreen* object
		){
			object->setParent(NULL);
		}



		template<> void OldLoadSavePolicy<DisplayScreenTableSync,DisplayScreen>::Save(
			DisplayScreen* object,
			optional<DBTransaction&> transaction
		){
			// Physical stops
			stringstream psstream;
			bool first(true);
			const ArrivalDepartureTableGenerator::PhysicalStops& pss = object->getPhysicalStops(false);
			BOOST_FOREACH(const ArrivalDepartureTableGenerator::PhysicalStops::value_type& itp, pss)
			{
				assert(itp.second->getKey() > 0);
				if(!first)
				{
					psstream << ",";
				}
				psstream << itp.first;
				first = false;
			}

			// Forbidden places
			stringstream fpstream;
			first = true;
			for (ForbiddenPlacesList::const_iterator itf = object->getForbiddenPlaces().begin(); itf != object->getForbiddenPlaces().end(); ++itf)
			{
				assert(itf->second->getKey() > 0);
				if(!first)
				{
					fpstream << ",";
				}
				fpstream << *itf->first;
				first = false;
			}

			// Displayed places
			stringstream dpstream;
			first = true;
			BOOST_FOREACH(const DisplayedPlacesList::value_type& itd, object->getDisplayedPlaces())
			{
				assert(itd.second->getKey() > 0);
				if (!first)
				{
					dpstream << ",";
				}
				dpstream << itd.second->getKey();
				first = false;
			}

			// Forced destinations
			stringstream fdstream;
			first = true;
			BOOST_FOREACH(const DisplayedPlacesList::value_type& itd2, object->getForcedDestinations())
			{
				assert(itd2.second->getKey() > 0);
				if (!first)
				{
					fdstream << ",";
				}
				fdstream << itd2.second->getKey();
				first = false;
			}

			// Transfer destinations
			stringstream tdstream;
			first = true;
			BOOST_FOREACH(const TransferDestinationsList::value_type& it, object->getTransferdestinations())
			{
				BOOST_FOREACH(const TransferDestinationsList::mapped_type::value_type& it2, it.second)
				{
					if(!first)
					{
						tdstream << ",";
					}
					tdstream << it.first->getKey() << ":" << it2->getKey();
					first = false;
				}
			}


			// Query
			ReplaceQuery<DisplayScreenTableSync> query(*object);
			query.addField(object->getDisplayedPlace() ? object->getDisplayedPlace()->getKey() : RegistryKeyType(0));
			query.addField(object->getName());
			query.addField(object->getType() ? object->getType()->getKey() : RegistryKeyType(0));
			query.addField(object->getWiringCode());
			query.addField(object->getTitle());
			query.addField(object->getBlinkingDelay());
			query.addField(object->getTrackNumberDisplay());
			query.addField(object->getServiceNumberDisplay());
			query.addField(object->getDisplayTeam());
			query.addField(psstream.str());
			query.addField(object->getAllPhysicalStopsDisplayed());
			query.addField(fpstream.str());
			query.addField(DisplayScreenTableSync::SerializeLineFilter(object->getAllowedLines()));
			query.addField(static_cast<int>(object->getDirection()));
			query.addField(static_cast<int>(object->getEndFilter()));
			query.addField(dpstream.str());
			query.addField(object->getMaxDelay());
			query.addField(object->getClearingDelay());
			query.addField(object->getFirstRow());
			query.addField(static_cast<int>(object->getGenerationMethod()));
			query.addField(fdstream.str());
			query.addField(object->getForceDestinationDelay());
			query.addField(string());
			query.addField(object->getIsOnline());
			query.addField(object->getMaintenanceMessage());
			query.addField(object->getDisplayClock());
			query.addField(object->getComPort());
			query.addField((object->getRoot<DisplayScreenCPU>() && !object->getParent()) ? object->getRoot<DisplayScreenCPU>()->getKey() : RegistryKeyType(0));
			query.addField(object->getMacAddress());
			query.addField(object->getRoutePlanningWithTransfer());
			query.addField(tdstream.str());
			query.addField(object->getParent(true) ? object->getParent()->getKey() : RegistryKeyType(0));
			query.addField(static_cast<int>(object->getSubScreenType()));
			query.addField(
				DataSourceLinks::Serialize(
					object->getDataSourceLinks()
			)	);
			query.addField(object->getAllowCanceled());
			query.addField(object->getStopPointLocation() ? object->getStopPointLocation()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}




		template<> bool DBTableSyncTemplate<DisplayScreenTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			try
			{
				Env env;
				boost::shared_ptr<const DisplayScreen> screen(DisplayScreenTableSync::Get(object_id, env));
				if (screen->getLocation() != NULL)
				{
					return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(DELETE_RIGHT, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(screen->getLocation()->getKey()));
				}
				else
				{
					return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE);
				}
			}
			catch(ObjectNotFoundException<DisplayScreen>&)
			{
				return false;
			}
		}



		template<> void DBTableSyncTemplate<DisplayScreenTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			AlarmObjectLinkTableSync::RemoveByTarget(id, transaction);
		}



		template<> void DBTableSyncTemplate<DisplayScreenTableSync>::AfterDelete(
			util::RegistryKeyType id,
			DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DisplayScreenTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			Env env;
			boost::shared_ptr<const DisplayScreen> screen(DisplayScreenTableSync::Get(id, env));
			ArrivalDepartureTableLog::addRemoveEntry(screen.get(), session->getUser().get());
		}
	}

	namespace departure_boards
	{
		DisplayScreenTableSync::SearchResult DisplayScreenTableSync::Search(
			Env& env,
			optional<const security::RightsOfSameClassMap&> rights,
			bool totalControl,
			RightLevel neededLevel,
			optional<RegistryKeyType> duid,
			optional<RegistryKeyType> localizationid,
			optional<RegistryKeyType> lineid,
			optional<RegistryKeyType> typeuid,
			string cityName,
			string stopName,
			string name,
			optional<int> state,
			optional<int> message,
			size_t first /*= 0*/,
			boost::optional<std::size_t> number,
			bool orderByUid, /*= false*/
			bool orderByCity, /*= true*/
			bool orderByStopName, /*= false*/
			bool orderByName, /*= false*/
			bool orderByType, /*= false*/
			bool orderByStatus,
			bool orderByMessage,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<DisplayScreenTableSync> query;

			// Tables
			if(!localizationid || *localizationid != 0)
			{
				query.addTableAndEqualJoin<StopAreaTableSync>(TABLE_COL_ID, BroadCastPoint::FIELD.name);
				query.addTableAndEqualOtherJoin<CityTableSync,StopAreaTableSync>(TABLE_COL_ID, StopAreaTableSync::TABLE_COL_CITYID);
				query.addTableAndEqualOtherJoin<StopPointTableSync,StopAreaTableSync>(StopPointTableSync::COL_PLACEID, TABLE_COL_ID);

				if (lineid || neededLevel > FORBIDDEN)
				{
					query.addTableAndEqualOtherJoin<LineStopTableSync,StopPointTableSync>(LineNode::FIELD.name, TABLE_COL_ID);
					query.addTableAndEqualOtherJoin<JourneyPatternTableSync,LineStopTableSync>(TABLE_COL_ID, Line::FIELD.name);
				}

				if(orderByType)
				{
					query.addTableAndEqualJoin<DisplayTypeTableSync>(TABLE_COL_ID, DisplayTypePtr::FIELD.name);
				}
			}

			// Filtering
			if(!localizationid || *localizationid != 0)
			{
				if (neededLevel > FORBIDDEN && rights)
				{
					query.addWhere(
						ComposedExpression::Get(
							FieldExpression::Get(
								JourneyPatternTableSync::TABLE.NAME, JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID
							),
							ComposedExpression::OP_IN,
							SubQueryExpression::Get(
								CommercialLineTableSync::getSQLLinesList(*rights, totalControl, neededLevel, false)
					)	)	);
				}
				if (!cityName.empty())
				{
					query.addWhereFieldOther<CityTableSync>(CityTableSync::TABLE_COL_NAME, "%"+cityName+"%", ComposedExpression::OP_LIKE);
				}
				if (!stopName.empty())
				{
					query.addWhereFieldOther<StopAreaTableSync>(StopAreaTableSync::TABLE_COL_NAME, "%"+stopName+"%", ComposedExpression::OP_LIKE);
				}
				if (lineid)
				{
					query.addWhereFieldOther<JourneyPatternTableSync>(JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID, *lineid);
				}
			}
			if(state)
			{
				/// @todo write status filter
			}
			if(message)
			{
				/// @todo write message filter
			}
			if (!name.empty())
			{
				query.addWhereField(BroadCastPointComment::FIELD.name, "%"+name+"%", ComposedExpression::OP_LIKE);
			}
			if (duid)
			{
				query.addWhereField(TABLE_COL_ID, *duid);
			}
			if (localizationid)
			{
				query.addWhereField(BroadCastPoint::FIELD.name, *localizationid);
			}
			if (typeuid)
			{
				query.addWhereField(DisplayTypePtr::FIELD.name, *typeuid);
			}

			// Grouping
			query.addGroupByField();

			// Ordering
			if (orderByUid)
			{
				query.addOrderField(TABLE_COL_ID, raisingOrder);
			}
			else if ((!localizationid || *localizationid != 0) && orderByCity)
			{
				query.addOrderFieldOther<CityTableSync>(CityTableSync::TABLE_COL_NAME, raisingOrder);
				query.addOrderFieldOther<StopAreaTableSync>(StopAreaTableSync::TABLE_COL_NAME, raisingOrder);
				query.addOrderField(BroadCastPointComment::FIELD.name, raisingOrder);
			}
			else if ((!localizationid || *localizationid != 0) && orderByStopName)
			{
				query.addOrderFieldOther<StopAreaTableSync>(StopAreaTableSync::TABLE_COL_NAME, raisingOrder);
				query.addOrderFieldOther<CityTableSync>(CityTableSync::TABLE_COL_NAME, raisingOrder);
				query.addOrderField(BroadCastPointComment::FIELD.name, raisingOrder);
			}
			else if (orderByName)
			{
				query.addOrderField(BroadCastPointComment::FIELD.name, raisingOrder);
				if(!localizationid || *localizationid != 0)
				{
					query.addOrderFieldOther<CityTableSync>(CityTableSync::TABLE_COL_NAME, raisingOrder);
					query.addOrderFieldOther<StopAreaTableSync>(StopAreaTableSync::TABLE_COL_NAME, raisingOrder);
			}	}
			else if (orderByType)
			{
			    query.addOrderFieldOther<DisplayTypeTableSync>(Name::FIELD.name, raisingOrder);
				if(!localizationid || *localizationid != 0)
				{
					query.addOrderFieldOther<CityTableSync>(CityTableSync::TABLE_COL_NAME, raisingOrder);
					query.addOrderFieldOther<StopAreaTableSync>(StopAreaTableSync::TABLE_COL_NAME, raisingOrder);
				}
				query.addOrderField(BroadCastPointComment::FIELD.name, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
				if (first > 0)
					query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}



		DisplayScreenTableSync::SearchResult DisplayScreenTableSync::SearchFromCPU(
			util::Env& env,
			util::RegistryKeyType cpuId,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			SelectQuery<DisplayScreenTableSync> query;
			query.addWhereField(CpuHostId::FIELD.name, cpuId);
			query.addOrderField(ComPort::FIELD.name, true);
			query.addOrderField(WiringCode::FIELD.name, true);
			return LoadFromQuery(query, env, linkLevel);
		}



		vector<boost::shared_ptr<SentAlarm> > DisplayScreenTableSync::GetCurrentDisplayedMessage(
			util::Env& env,
			util::RegistryKeyType screenId,
			optional<int> limit
		){
			ptime now(second_clock::local_time());

			SelectQuery<AlarmObjectLinkTableSync> query;
			query.addTableField(AlarmObjectLinkTableSync::COL_ALARM_ID);
			query.addTableAndEqualJoin<AlarmTableSync>(TABLE_COL_ID, AlarmObjectLinkTableSync::COL_ALARM_ID);
			query.addTableAndEqualOtherJoin<ScenarioTableSync, AlarmTableSync>(TABLE_COL_ID, AlarmTableSync::COL_SCENARIO_ID);
			query.addWhereField(AlarmObjectLinkTableSync::COL_OBJECT_ID, screenId);
			query.addWhereFieldOther<ScenarioTableSync>(ScenarioTableSync::COL_ENABLED, 1);
			query.addWhere(
				SQLSingleOperatorExpression::Get(
					SQLSingleOperatorExpression::OP_NOT,
					FieldExpression::Get(
						ScenarioTableSync::TABLE.NAME, ScenarioTableSync::COL_IS_TEMPLATE
			)	)	);
			query.addWhere(
				ComposedExpression::Get(
					SQLSingleOperatorExpression::Get(
						SQLSingleOperatorExpression::OP_IS_NULL,
						FieldExpression::Get(
							ScenarioTableSync::TABLE.NAME, ScenarioTableSync::COL_PERIODSTART
					)	),
					ComposedExpression::OP_OR,
					ComposedExpression::Get(
						FieldExpression::Get(
							ScenarioTableSync::TABLE.NAME, ScenarioTableSync::COL_PERIODSTART
						),
						ComposedExpression::OP_INF,
						ValueExpression<ptime>::Get(now)
					)
			)	);
			query.addWhere(
				ComposedExpression::Get(
					SQLSingleOperatorExpression::Get(
					SQLSingleOperatorExpression::OP_IS_NULL,
						FieldExpression::Get(
							ScenarioTableSync::TABLE.NAME, ScenarioTableSync::COL_PERIODEND
					)	),
					ComposedExpression::OP_OR,
					ComposedExpression::Get(
						FieldExpression::Get(
							ScenarioTableSync::TABLE.NAME, ScenarioTableSync::COL_PERIODEND
						),
						ComposedExpression::OP_SUP,
						ValueExpression<ptime>::Get(now)
					)
			)	);
			query.addOrderFieldOther<AlarmTableSync>(AlarmTableSync::COL_LEVEL, false);
			query.addOrderFieldOther<ScenarioTableSync>(ScenarioTableSync::COL_PERIODSTART, false);
			if (limit)
			{
				query.setNumber(*limit);
			}
			DBResultSPtr rows = query.execute();
			vector<boost::shared_ptr<SentAlarm> > result;
			while(rows->next())
			{
				result.push_back(
					static_pointer_cast<SentAlarm,Alarm>(
						AlarmTableSync::GetEditable(
							rows->getLongLong(AlarmObjectLinkTableSync::COL_ALARM_ID),
							env
				)	)	);
			}
			return result;
		}



		bool DisplayScreenTableSync::GetIsAtLeastALineDisplayed(
			util::RegistryKeyType screenId
		){
			stringstream q;
			q	<< "SELECT l." << TABLE_COL_ID
				<< " FROM " << TABLE.NAME << " AS d"
				<< " INNER JOIN " << StopPointTableSync::TABLE.NAME << " AS s ON s." << StopPointTableSync::COL_PLACEID << "=d." << BroadCastPoint::FIELD.name
				<< " INNER JOIN " << LineStopTableSync::TABLE.NAME << " AS l ON l." << LineNode::FIELD.name << "=s." << TABLE_COL_ID
				<< " WHERE d." << TABLE_COL_ID << "=" << screenId
				<< " AND (d." << AllPhysicalDisplayed::FIELD.name << " OR d." << PhysicalStops::FIELD.name << " LIKE ('%'|| s." << TABLE_COL_ID << " ||'%'))"
				<< " AND (l." << IsDeparture::FIELD.name << " AND d." << Direction::FIELD.name << " OR l." << IsArrival::FIELD.name << " AND NOT d." << Direction::FIELD.name << ")"
				<< " AND (NOT d." << OriginsOnly::FIELD.name << " OR l." << RankInPath::FIELD.name << "=0)"
				<< " AND NOT EXISTS(SELECT p2." << StopPointTableSync::COL_PLACEID << " FROM " << StopPointTableSync::TABLE.NAME << " AS p2 INNER JOIN " << LineStopTableSync::TABLE.NAME << " AS l2 ON l2." << LineNode::FIELD.name << "=p2." << TABLE_COL_ID
				<< " WHERE l2." << Line::FIELD.name << "=l." << Line::FIELD.name
				<< " AND l2." << RankInPath::FIELD.name << ">l." << RankInPath::FIELD.name
				<< " AND l2." << IsArrival::FIELD.name
				<< " AND ('%'|| p2." << StopPointTableSync::COL_PLACEID << " ||'%') LIKE d." << ForbiddenArrivalPlaces::FIELD.name
				<< ")"
				<< " LIMIT 1";
			DBResultSPtr rows = DBModule::GetDB()->execQuery(q.str());
			return rows->next();
		}



		vector<boost::shared_ptr<SentAlarm> > DisplayScreenTableSync::GetFutureDisplayedMessages(
			Env& env,
			RegistryKeyType screenId,
			optional<int> number
		){
			ptime now(second_clock::local_time());
			stringstream q;
			q	<< "SELECT " << AlarmObjectLinkTableSync::COL_ALARM_ID
				<< " FROM " << AlarmObjectLinkTableSync::TABLE.NAME << " AS aol "
				<< " INNER JOIN " << AlarmTableSync::TABLE.NAME << " AS a ON a." << TABLE_COL_ID << "=aol." << AlarmObjectLinkTableSync::COL_ALARM_ID
				<< " INNER JOIN " << ScenarioTableSync::TABLE.NAME << " AS s ON s." << TABLE_COL_ID << "=a." << AlarmTableSync::COL_SCENARIO_ID
				<< " WHERE aol." << AlarmObjectLinkTableSync::COL_OBJECT_ID << "=" << screenId
				<< " AND s." << ScenarioTableSync::COL_ENABLED
				<< " AND NOT s." << ScenarioTableSync::COL_IS_TEMPLATE
				<< " AND s." << ScenarioTableSync::COL_PERIODSTART << ">'" << to_iso_extended_string(now.date()) << " " << to_simple_string(now.time_of_day()) << "'"
				<< " ORDER BY s." << ScenarioTableSync::COL_PERIODSTART;
			if (number)
			{
				q << " LIMIT " << *number;
			}
			DBResultSPtr rows = DBModule::GetDB()->execQuery(q.str());
			vector<boost::shared_ptr<SentAlarm> > result;
			while(rows->next())
			{
				result.push_back(static_pointer_cast<SentAlarm,Alarm>(AlarmTableSync::GetEditable(rows->getLongLong(AlarmObjectLinkTableSync::COL_ALARM_ID), env)));
			}
			return result;
		}



		boost::shared_ptr<DisplayScreen> DisplayScreenTableSync::GetByMACAddress(
			util::Env& env,
			const std::string& macAddress,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */ )
		{
			SelectQuery<DisplayScreenTableSync> query;
			query.addWhereField(MacAddress::FIELD.name, macAddress);
			query.setNumber(1);
			DBResultSPtr rows = query.execute();
			if(rows->next ())
			{
				util::Registry<DisplayScreen>& registry(env.getEditableRegistry<DisplayScreen>());
				if(registry.contains(rows->getKey()))
				{
					return registry.getEditable(rows->getKey());
				}
				else
				{
					boost::shared_ptr<DisplayScreen> object(DisplayScreenTableSync::GetNewObject(*rows));
					registry.add(object);
					Load(object.get(), rows, env, linkLevel);
					return object;
				}
			}
			throw Exception("Display screen not found");
		}



		LineFilter DisplayScreenTableSync::UnserializeLineFilter(
			const std::string& text,
			util::Env& env,
			util::LinkLevel linkLevel
		){
			LineFilter result;

			// Empty text makes split throwing an exception
			if(text.empty())
			{
				return result;
			}

			vector<string> lines;
			split(lines, text, is_any_of(", "));
			BOOST_FOREACH(const string& line, lines)
			{
				vector<string> parts;
				split(parts, line, is_any_of("|"));

				RegistryKeyType id(lexical_cast<RegistryKeyType>(parts[0]));
				try
				{
					boost::shared_ptr<const CommercialLine> lineObj(
						CommercialLineTableSync::Get(id, env, linkLevel)
					);
					result.insert(
						make_pair(
							lineObj.get(),
							parts.size() > 1 ? optional<bool>(lexical_cast<bool>(parts[1])) : optional<bool>()
					)	);
				}
				catch(ObjectNotFoundException<CommercialLine>&)
				{
				}
			}

			return result;
		}



		std::string DisplayScreenTableSync::SerializeLineFilter( const LineFilter& value )
		{
			stringstream result;

			bool first(true);
			BOOST_FOREACH(const LineFilter::value_type& itLineFilter, value)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					result << ",";
				}

				result << itLineFilter.first->getKey();

				if(itLineFilter.second)
				{
					result << "|" << *itLineFilter.second;
				}
			}

			return result.str();
		}

}	}
