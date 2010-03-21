
/** ReplaceQuery class header.
	@file ReplaceQuery.h

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

#ifndef SYNTHESE_db_ReplaceQuery_h__
#define SYNTHESE_db_ReplaceQuery_h__

#include "DBModule.h"
#include "SQLite.h"

namespace synthese
{
	namespace db
	{
		/** ReplaceQuery class.
			@ingroup m10
		*/
		template<class TableSync>
		class ReplaceQuery
		{
		private:
			std::vector<std::string> _fields;
			util::RegistryKeyType _id;

		public:
			ReplaceQuery(
				typename TableSync::ObjectType& object
			){
				if(object.getKey() <= 0)
				{
					object.setKey(TableSync::getId());
				}
				_id=object.getKey();
			}

			
			void addField(const boost::posix_time::ptime& value
			){
				_fields.push_back("'" + (value.is_not_a_date_time() ? "" : boost::gregorian::to_iso_extended_string(value.date()) + " " + boost::posix_time::to_simple_string(value.time_of_day())) + "'");
			}
			void addField(
				const boost::gregorian::date& value
			){
				_fields.push_back("'" + (value.is_not_a_date() ? "" : boost::gregorian::to_iso_extended_string(value)) + "'");
			}
			
			void addField(
				const std::string& value
			){
				std::string result;
				result.push_back('\'');
				BOOST_FOREACH(char s, value)
				{
					// Escape several characters
					if (s == '\'')
						result.push_back('\'');
					result.push_back(s);
				}
				result.push_back('\'');
				_fields.push_back(result);
			}

			void addField(int value){
				_fields.push_back(boost::lexical_cast<std::string>(value));
			}

			void addField(double value){
				_fields.push_back(boost::lexical_cast<std::string>(value));
			}

			void addField(util::RegistryKeyType value){
				_fields.push_back(boost::lexical_cast<std::string>(value));
			}

			void execute(
				boost::optional<SQLiteTransaction&> transaction
			){
				std::stringstream query;
				query
					<< " REPLACE INTO " << TableSync::TABLE.NAME << " VALUES("
					<< _id;
				BOOST_FOREACH(const std::string& field, _fields)
				{
					query << "," << field;
				}
				query << ");";
				DBModule::GetSQLite()->execUpdate(query.str(), transaction);
			}
		};
	}
}

#endif // SYNTHESE_db_ReplaceQuery_h__
