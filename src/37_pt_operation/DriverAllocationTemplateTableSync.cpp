
/** DriverAllocationTemplateTableSync class implementation.
	@file DriverAllocationTemplateTableSync.cpp

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

#include "DriverAllocationTemplateTableSync.hpp"

#include "SelectQuery.hpp"

#include <sstream>
#include <boost/tokenizer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost;
using namespace std;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace pt;
	using namespace pt_operation;
	using namespace util;


	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,DriverAllocationTemplateTableSync>::FACTORY_KEY("37.40 Driver allocation templates");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DriverAllocationTemplateTableSync>::TABLE(
			"t088_driver_allocation_templates"
		);

		template<> const Field DBTableSyncTemplate<DriverAllocationTemplateTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<DriverAllocationTemplateTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(SimpleObjectFieldDefinition<Date>::FIELD.name.c_str(), ""));
			return r;
		};



		template<> bool DBTableSyncTemplate<DriverAllocationTemplateTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true; // TODO
		}



		template<> void DBTableSyncTemplate<DriverAllocationTemplateTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DriverAllocationTemplateTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DriverAllocationTemplateTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO log the removal
		}
	}

	namespace pt_operation
	{
		DriverAllocationTemplateTableSync::SearchResult DriverAllocationTemplateTableSync::Search(
			util::Env& env,
			boost::gregorian::date date /*= boost::gregorian::date(not_a_date_time)*/,
			size_t first /*= 0*/,
			boost::optional<std::size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByDate /*= true*/,
			bool raisingOrder /*= true*/,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			SelectQuery<DriverAllocationTemplateTableSync> query;
			if (!date.is_not_a_date())
			{
				query.addWhereField(SimpleObjectFieldDefinition<Date>::FIELD.name, to_iso_extended_string(date));
			}
			if (orderByDate)
			{
				query.addOrderField(SimpleObjectFieldDefinition<Date>::FIELD.name, raisingOrder);
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
}	}
