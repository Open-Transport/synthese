
/** JourneyPatternTableSync class implementation.
	@file JourneyPatternTableSync.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include <sstream>

#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "GraphConstants.h"
#include "CommercialLineTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "FareTableSync.h"
#include "RollingStockTableSync.h"
#include "DataSourceTableSync.h"
#include "PTUseRuleTableSync.h"
#include "PTUseRule.h"
#include "ImportableTableSync.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace impex;
	using namespace graph;
	using namespace pt;

	template<> const string util::FactorableTemplate<SQLiteTableSync,JourneyPatternTableSync>::FACTORY_KEY(
		"35.30.01 Journey patterns"
	);

	namespace pt
	{
		const string JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID = "commercial_line_id";
		const string JourneyPatternTableSync::COL_NAME ("name");
		const string JourneyPatternTableSync::COL_TIMETABLENAME ("timetable_name");
		const string JourneyPatternTableSync::COL_DIRECTION ("direction");
		const string JourneyPatternTableSync::COL_ISWALKINGLINE ("is_walking_line");
		const string JourneyPatternTableSync::COL_USEINDEPARTUREBOARDS ("use_in_departure_boards");
		const string JourneyPatternTableSync::COL_USEINTIMETABLES ("use_in_timetables");
		const string JourneyPatternTableSync::COL_USEINROUTEPLANNING ("use_in_routeplanning");
		const string JourneyPatternTableSync::COL_ROLLINGSTOCKID ("rolling_stock_id");
		const string JourneyPatternTableSync::COL_BIKECOMPLIANCEID ("bike_compliance_id");
		const string JourneyPatternTableSync::COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
		const string JourneyPatternTableSync::COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");
		const string JourneyPatternTableSync::COL_WAYBACK("wayback");
		const string JourneyPatternTableSync::COL_DATASOURCE_ID("data_source");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<JourneyPatternTableSync>::TABLE(
			"t009_lines"
		);
		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<JourneyPatternTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(JourneyPatternTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(JourneyPatternTableSync::COL_TIMETABLENAME, SQL_TEXT),
			SQLiteTableSync::Field(JourneyPatternTableSync::COL_DIRECTION, SQL_TEXT),
			SQLiteTableSync::Field(JourneyPatternTableSync::COL_ISWALKINGLINE, SQL_BOOLEAN),
			SQLiteTableSync::Field(JourneyPatternTableSync::COL_USEINDEPARTUREBOARDS, SQL_BOOLEAN),
			SQLiteTableSync::Field(JourneyPatternTableSync::COL_USEINTIMETABLES, SQL_BOOLEAN),
			SQLiteTableSync::Field(JourneyPatternTableSync::COL_USEINROUTEPLANNING, SQL_BOOLEAN),
			SQLiteTableSync::Field(JourneyPatternTableSync::COL_ROLLINGSTOCKID, SQL_INTEGER),
			SQLiteTableSync::Field(JourneyPatternTableSync::COL_BIKECOMPLIANCEID, SQL_INTEGER),
			SQLiteTableSync::Field(JourneyPatternTableSync::COL_HANDICAPPEDCOMPLIANCEID, SQL_INTEGER),
			SQLiteTableSync::Field(JourneyPatternTableSync::COL_PEDESTRIANCOMPLIANCEID, SQL_INTEGER),
			SQLiteTableSync::Field(JourneyPatternTableSync::COL_WAYBACK, SQL_INTEGER),
			SQLiteTableSync::Field(JourneyPatternTableSync::COL_DATASOURCE_ID, SQL_INTEGER),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<JourneyPatternTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID.c_str(), ""),
			SQLiteTableSync::Index(JourneyPatternTableSync::COL_DATASOURCE_ID.c_str(), ""),
			SQLiteTableSync::Index()
		};


		template<> void SQLiteDirectTableSyncTemplate<JourneyPatternTableSync,JourneyPattern>::Load(
			JourneyPattern* line,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			string name (
			    rows->getText (JourneyPatternTableSync::COL_NAME));
			string timetableName (
			    rows->getText (JourneyPatternTableSync::COL_TIMETABLENAME));
			string direction (
			    rows->getText (JourneyPatternTableSync::COL_DIRECTION));

			bool isWalkingLine (rows->getBool (JourneyPatternTableSync::COL_ISWALKINGLINE));
			bool useInDepartureBoards (rows->getBool (JourneyPatternTableSync::COL_USEINDEPARTUREBOARDS));
			bool useInTimetables (rows->getBool (JourneyPatternTableSync::COL_USEINTIMETABLES));
			bool useInRoutePlanning (rows->getBool (JourneyPatternTableSync::COL_USEINROUTEPLANNING));
			
			line->setName(name);
			line->setTimetableName (timetableName);
			line->setDirection (direction);
			line->setWalkingLine (isWalkingLine);
			line->setUseInDepartureBoards (useInDepartureBoards);
			line->setUseInTimetables (useInTimetables);
			line->setUseInRoutePlanning (useInRoutePlanning);
			line->setWayBack(rows->getBool(JourneyPatternTableSync::COL_WAYBACK));
			line->setRollingStock(NULL);
			line->setCommercialLine(NULL);
			line->cleanDataSourceLinks();
			RuleUser::Rules rules(RuleUser::GetEmptyRules());

			if (linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				RegistryKeyType commercialLineId(rows->getLongLong (JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID));
				try
				{
					CommercialLine* cline(CommercialLineTableSync::GetEditable(commercialLineId, env, linkLevel).get());
					line->setCommercialLine(cline);
					cline->addPath(line);
				}
				catch(ObjectNotFoundException<CommercialLine>)
				{
					Log::GetInstance().warn("Bad value " + lexical_cast<string>(commercialLineId) + " for fare in line " + lexical_cast<string>(line->getKey()));
				}

				// Data sources and operator codes
				line->setDataSourceLinks(
					ImportableTableSync::GetDataSourceLinksFromSerializedString(
						rows->getText(JourneyPatternTableSync::COL_DATASOURCE_ID),
						env
				)	);

				RegistryKeyType rollingStockId (rows->getLongLong (JourneyPatternTableSync::COL_ROLLINGSTOCKID));
				if(rollingStockId > 0)
				{
					try
					{
						line->setRollingStock(RollingStockTableSync::GetEditable(rollingStockId, env, linkLevel, AUTO_CREATE).get());
					}
					catch(ObjectNotFoundException<RollingStock>)
					{
						Log::GetInstance().warn("Bad value " + lexical_cast<string>(rollingStockId) + " for rolling stock in line " + lexical_cast<string>(line->getKey()));
				}	}


				RegistryKeyType bikeComplianceId (rows->getLongLong (JourneyPatternTableSync::COL_BIKECOMPLIANCEID));
				if(bikeComplianceId > 0)
				{
					try
					{
						rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(bikeComplianceId, env, linkLevel).get();
					}
					catch(ObjectNotFoundException<PTUseRule>)
					{
						Log::GetInstance().warn("Bad value " + lexical_cast<string>(bikeComplianceId) + " for bike compliance in line " + lexical_cast<string>(line->getKey()));
				}	}

				RegistryKeyType handicappedComplianceId (rows->getLongLong (JourneyPatternTableSync::COL_HANDICAPPEDCOMPLIANCEID));
				if(handicappedComplianceId > 0)
				{
					try
					{
						rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(handicappedComplianceId, env, linkLevel).get();
					}
					catch(ObjectNotFoundException<PTUseRule>)
					{
						Log::GetInstance().warn("Bad value " + lexical_cast<string>(handicappedComplianceId) + " for handicapped compliance in line " + lexical_cast<string>(line->getKey()));
				}	}
				
				RegistryKeyType pedestrianComplianceId(rows->getLongLong (JourneyPatternTableSync::COL_PEDESTRIANCOMPLIANCEID));
				if(pedestrianComplianceId > 0)
				{
					try
					{
						rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(pedestrianComplianceId, env, linkLevel).get();
					}
					catch(ObjectNotFoundException<PTUseRule>)
					{
						Log::GetInstance().warn("Bad value " + lexical_cast<string>(pedestrianComplianceId) + " for pedestrian compliance in line " + lexical_cast<string>(line->getKey()));
				}	}
			}
			line->setRules(rules);
		}



		template<> void SQLiteDirectTableSyncTemplate<JourneyPatternTableSync,JourneyPattern>::Save(
			JourneyPattern* object,
			optional<SQLiteTransaction&> transaction
		){
			if(!object->getCommercialLine()) throw Exception("JourneyPattern save error. Missing commercial line");
			ReplaceQuery<JourneyPatternTableSync> query(*object);
			query.addField(object->getCommercialLine()->getKey());
			query.addField(object->getName());
			query.addField(object->getTimetableName());
			query.addField(object->getDirection());
			query.addField(object->getWalkingLine());
			query.addField(object->getUseInDepartureBoards());
			query.addField(object->getUseInTimetables());
			query.addField(object->getUseInRoutePlanning());
			query.addField(object->getRollingStock() ? object->getRollingStock()->getKey() : RegistryKeyType(0));
			query.addField(
				object->getRule(USER_BIKE) && dynamic_cast<const PTUseRule*>(object->getRule(USER_BIKE)) ?
				static_cast<const PTUseRule*>(object->getRule(USER_BIKE))->getKey() : RegistryKeyType(0)
			);
			query.addField(
				object->getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(object->getRule(USER_HANDICAPPED)) ? 
				static_cast<const PTUseRule*>(object->getRule(USER_HANDICAPPED))->getKey() : RegistryKeyType(0)
			);
			query.addField(
				object->getRule(USER_PEDESTRIAN) && dynamic_cast<const PTUseRule*>(object->getRule(USER_PEDESTRIAN)) ? 
				static_cast<const PTUseRule*>(object->getRule(USER_PEDESTRIAN))->getKey() : RegistryKeyType(0)
			);
			query.addField(object->getWayBack());
			query.addField(
				ImportableTableSync::SerializeDataSourceLinks(
					object->getDataSourceLinks()
			)	);
			query.execute(transaction);
		}



		template<> void SQLiteDirectTableSyncTemplate<JourneyPatternTableSync,JourneyPattern>::Unlink(JourneyPattern* obj)
		{
			if(obj->getCommercialLine())
			{
				const_cast<CommercialLine*>(obj->getCommercialLine())->removePath(obj);
			}
		}
	}

	namespace pt
	{
		JourneyPatternTableSync::SearchResult JourneyPatternTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> commercialLineId,
			int first, /*= 0*/
			boost::optional<std::size_t> number,
			bool orderByName,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<JourneyPatternTableSync> query;
			if (commercialLineId)
			{
				query.addWhereField(COL_COMMERCIAL_LINE_ID, *commercialLineId);
			}
			if (orderByName)
			{
				query.addOrderField(COL_NAME, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
			}
			if (first > 0)
			{
				query.setFirst(first);
			}
			return LoadFromQuery(query, env, linkLevel);
		}
	}
}
