
/** FareTableSync class implementation.
	@file FareTableSync.cpp

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

#include "FareTableSync.hpp"

#include "Conversion.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "Session.h"
#include "User.h"

#include <sstream>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace fare;
	using namespace security;

	template<> const string util::FactorableTemplate<DBTableSync, FareTableSync>::FACTORY_KEY("30.10.02 Fares");

	namespace fare
	{
		const std::string FareTableSync::FIELDS_SEPARATOR("|");
		const std::string FareTableSync::ROWS_SEPARATOR(",");

		const std::string FareTableSync::COL_NAME("name");
		const std::string FareTableSync::COL_FARETYPE("fare_type");
		const std::string FareTableSync::COL_CURRENCY("currency");
		const std::string FareTableSync::COL_PERMITTED_CONNECTIONS_NUMBER("connections_permitted_number");
		const std::string FareTableSync::COL_REQUIRED_CONTINUITY("continuity_required");
		const std::string FareTableSync::COL_VALIDITY_PERIOD("validity_period");

		const std::string FareTableSync::COL_ACCESS("access");
		const std::string FareTableSync::COL_SLICES("slices");
		const std::string FareTableSync::COL_UNIT_PRICE("unit_price");
		const std::string FareTableSync::COL_MATRIX("matrix");
		const std::string FareTableSync::COL_SUB_FARES("sub_fares");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<FareTableSync>::TABLE(
			"t008_fares"
		);

		template<> const Field DBTableSyncTemplate<FareTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(FareTableSync::COL_NAME, SQL_TEXT),
			Field(FareTableSync::COL_FARETYPE, SQL_INTEGER),
			Field(FareTableSync::COL_CURRENCY, SQL_TEXT),
			Field(FareTableSync::COL_PERMITTED_CONNECTIONS_NUMBER, SQL_INTEGER),
			Field(FareTableSync::COL_REQUIRED_CONTINUITY, SQL_BOOLEAN),
			Field(FareTableSync::COL_VALIDITY_PERIOD, SQL_INTEGER),

			Field(FareTableSync::COL_ACCESS, SQL_DOUBLE),
			Field(FareTableSync::COL_SLICES, SQL_TEXT),
			Field(FareTableSync::COL_UNIT_PRICE, SQL_DOUBLE),
			Field(FareTableSync::COL_MATRIX, SQL_TEXT),
			Field(FareTableSync::COL_SUB_FARES, SQL_TEXT),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<FareTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}

		template<> void OldLoadSavePolicy<FareTableSync,Fare>::Load(
			Fare* fare,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			fare->setName(rows->getText(FareTableSync::COL_NAME));
			fare->setTypeNumber(static_cast<FareType::FareTypeNumber>(rows->getInt(FareTableSync::COL_FARETYPE)));
			if(fare->getType())
			{
				fare->getType()->setAccessPrice(rows->getDouble(FareTableSync::COL_ACCESS));
				fare->getType()->setIsUnitPrice(rows->getBool(FareTableSync::COL_UNIT_PRICE));
				fare->getType()->setSlices(
					FareTableSync::GetSlicesFromSerializedString(
						rows->getText(FareTableSync::COL_SLICES)
				)	);
			}
			fare->setCurrency(rows->getText(FareTableSync::COL_CURRENCY));
			fare->setPermittedConnectionsNumber(rows->getOptionalInt(FareTableSync::COL_PERMITTED_CONNECTIONS_NUMBER));
			fare->setRequiredContinuity(rows->getBool(FareTableSync::COL_REQUIRED_CONTINUITY));
			fare->setValidityPeriod(rows->getInt(FareTableSync::COL_VALIDITY_PERIOD));
		}

		template<> void OldLoadSavePolicy<FareTableSync,Fare>::Save(
			Fare* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<FareTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getType()->getTypeNumber());
			query.addField(object->getCurrency());
			if(object->getPermittedConnectionsNumber())
				query.addField(*object->getPermittedConnectionsNumber());
			else
				query.addFieldNull();
			query.addField(object->isRequiredContinuity());
			query.addField(object->getValidityPeriod());

			if(object->getType())
			{
				query.addField(object->getType()->getAccessPrice());
				query.addField(FareTableSync::SerializeSlices(object->getType()->getSlices()));
				query.addField(object->getType()->getIsUnitPrice());
				query.addFieldNull();
				query.addFieldNull();
			}
			else
			{
				query.addFieldNull();
				query.addFieldNull();
				query.addFieldNull();
				query.addFieldNull();
				query.addFieldNull();
			}
			query.execute(transaction);
		}


		template<> void OldLoadSavePolicy<FareTableSync,Fare>::Unlink(Fare* obj)
		{
		}



		template<> bool DBTableSyncTemplate<FareTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true; // TODO create fare right
//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<FareTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<FareTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<FareTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace fare
	{
		std::string FareTableSync::SerializeSlices(
			const FareType::Slices& object
		){
			stringstream s;
			bool first(true);
			BOOST_FOREACH(const FareType::Slice& it, object)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					s << ROWS_SEPARATOR;
				}

				s << it.min;
				s << FIELDS_SEPARATOR;
				s << it.max;
				s << FIELDS_SEPARATOR;
				s << it.price;
			}
			return s.str();
		}



		FareType::Slices FareTableSync::GetSlicesFromSerializedString(
			const std::string& serializedString
		){
			FareType::Slices result;

			if(!serializedString.empty())
			{
				vector<string> rows;
				split(rows, serializedString, is_any_of(ROWS_SEPARATOR));
				BOOST_FOREACH(const string& row, rows)
				{
					try
					{
						// Parsing of the string
						vector<string> fields;
						split(fields, row, is_any_of(FIELDS_SEPARATOR));
						if(fields.size()>=3)
						{
							FareType::Slice slice;
							slice.min = lexical_cast<int>(fields[0]);
							slice.max = lexical_cast<int>(fields[1]);
							slice.price = lexical_cast<double>(fields[2]);
							// Storage
							result.push_back(slice);
						}
					}
					catch(bad_lexical_cast)
					{ // If bad cast, the row is ignored
						continue;
					}
				}
			}
			return result;
		}



		FareTableSync::SearchResult FareTableSync::Search(
			Env& env,
			optional<string> name,
			bool orderByName,
			bool raisingOrder,
			int first /*= 0*/,
			boost::optional<std::size_t> number  /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if(name)
			{
				query << " AND " << COL_NAME << " LIKE " << Conversion::ToDBString(*name);
			}
			if(orderByName)
			{
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			}
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		FareTableSync::NamedList FareTableSync::GetList(
			util::Env& env
		){
			NamedList result;
			SearchResult fares(Search(env));
			result.push_back(make_pair(RegistryKeyType(0), "(inconnu)"));

			BOOST_FOREACH(const boost::shared_ptr<Fare>& fare, fares)
			{
				result.push_back(make_pair(fare->getKey(), fare->getName()));
			}

			return result;
		}
	}
}
