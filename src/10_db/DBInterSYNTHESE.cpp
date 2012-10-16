
/** InterSYNTHESEDB class implementation.
	@file InterSYNTHESEDB.cpp

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

#include "DBInterSYNTHESE.hpp"

#include "DBException.hpp"
#include "DBDirectTableSync.hpp"
#include "DBModule.h"
#include "DBTransaction.hpp"
#include "Env.h"
#include "Field.hpp"
#include "InterSYNTHESESlave.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace inter_synthese;
	using namespace util;

	template<>
	const string FactorableTemplate<InterSYNTHESESyncTypeFactory, db::DBInterSYNTHESE>::FACTORY_KEY = "db";

	namespace db
	{
		const string DBInterSYNTHESE::TYPE_SQL = "sql";
		const string DBInterSYNTHESE::TYPE_REPLACE_STATEMENT = "rstmt";
		const string DBInterSYNTHESE::FIELD_SEPARATOR = ":";

		DBInterSYNTHESE::DBInterSYNTHESE():
			FactorableTemplate<InterSYNTHESESyncTypeFactory, DBInterSYNTHESE>()
		{
		}



		bool DBInterSYNTHESE::sync( const std::string& parameter ) const
		{
			size_t i(0);
			for(; i<parameter.size() && parameter[i]!=FIELD_SEPARATOR[0]; ++i) ;
			if(i == parameter.size())
			{
				return false;
			}
			string reqType(parameter.substr(0, i));
			if(reqType == TYPE_SQL)
			{
				try
				{
					DBModule::GetDB()->execQuery(parameter.substr(i+1));
				}
				catch(...)
				{
					return false;
				}
			}
			else if(reqType == TYPE_REPLACE_STATEMENT)
			{
				++i;
				size_t l=i;
				for(; i<parameter.size() && parameter[i]!=FIELD_SEPARATOR[0]; ++i) ;
				if(i == parameter.size())
				{
					return false;
				}
				
				try
				{
					// Table name
					string tableName(parameter.substr(l, i-l));
					DBRecord r(*DBModule::GetTableSync(tableName));
					++i;

					// Fields loop
					typedef std::map<
						std::string,
						boost::optional<std::string>
					> StmtFields;

					StmtFields stmtFields;

					while(i<parameter.size())
					{
						// Field name
						l = i;
						for(; i<parameter.size() && parameter[i]!=FIELD_SEPARATOR[0]; ++i) ;
						if(i == parameter.size())
						{
							return false;
						}
						string fieldName(parameter.substr(l, i-l));
						++i;

						// Is null
						if(i == parameter.size())
						{
							return false;
						}
						bool isNull(parameter[i] == '1');
						++i;
						if(i == parameter.size())
						{
							return false;
						}
						++i;

						if(isNull)
						{
							stmtFields.insert(make_pair(fieldName, optional<string>()));
							for(; i<parameter.size() && parameter[i]!=FIELD_SEPARATOR[0]; ++i) ;
						}
						else
						{
							// Size
							if(i == parameter.size())
							{
								return false;
							}
							l = i;
							for(; i<parameter.size() && parameter[i]!=FIELD_SEPARATOR[0]; ++i) ;
							size_t dataSize(lexical_cast<size_t>(parameter.substr(l, i-l)));
							++i;

							// Content
							if(i + dataSize > parameter.size())
							{
								return false;
							}
							string content(parameter.substr(i, dataSize));
							stmtFields.insert(make_pair(fieldName, content));
							i+=dataSize;
						}
						++i;
					}

					// Content extraction
					DBContent content;
					BOOST_FOREACH(const FieldsList::value_type& field, r.getTable()->getFieldsList())
					{
						StmtFields::const_iterator it(stmtFields.find(field.name));
						if(it == stmtFields.end())
						{
							return false;
						}

						if(!it->second)
						{
							content.push_back(Cell(optional<string>()));
						}
						else
						{
							switch(field.type)
							{
							case SQL_INTEGER:
								content.push_back(Cell(lexical_cast<RegistryKeyType>(*it->second)));
								break;

							case SQL_DOUBLE:
								content.push_back(Cell(lexical_cast<double>(*it->second)));
								break;

							case SQL_BOOLEAN:
								content.push_back(Cell(lexical_cast<bool>(*it->second)));
								break;

							default:
								content.push_back(Cell(it->second));
								break;

							}
						}
					}

					r.setContent(content);

					// STMT execution
					DBModule::GetDB()->saveRecord(r);
				}
				catch (DBException&)
				{
					return false;
				}
			}
			else
			{
				return false;
			}

			return true;
		}



		void DBInterSYNTHESE::initQueue(
			const InterSYNTHESESlave& slave,
			const std::string& perimeter
		) const	{

			try
			{
				// Detection of the table by id
				RegistryTableType tableId(
					lexical_cast<RegistryTableType>(perimeter)
				);
				shared_ptr<DBTableSync> tableSync(
					DBModule::GetTableSync(
						tableId
				)	);

				shared_ptr<DBDirectTableSync> directTableSync(
					dynamic_pointer_cast<DBDirectTableSync, DBTableSync>(
						tableSync
				)	);
				if(!directTableSync.get())
				{
					return;
				}

				// Getting all requests
				Env env;
				DBDirectTableSync::RegistrableSearchResult result(
					directTableSync->search(
						string(),
						env
				)	);

				// Build the dump
				DBTransaction transaction;
				BOOST_FOREACH(const DBDirectTableSync::RegistrableSearchResult::value_type& it, result)
				{
					directTableSync->saveRegistrable(*it, transaction);
				}

				// Export
				DBTransaction saveTransaction;
				BOOST_FOREACH(const DBTransaction::Query& query, transaction.getQueries())
				{
					stringstream content;
					RequestEnqueue visitor(content);
					apply_visitor(visitor, query);
					slave.enqueue(
						DBInterSYNTHESE::FACTORY_KEY,
						content.str(),
						saveTransaction
					);
				}
				saveTransaction.run();
			}
			catch (bad_lexical_cast&)
			{
			}
			catch(DBException&)
			{
			}
		}



		string DBInterSYNTHESE::GetSQLContent( const std::string& sql )
		{
			stringstream content;
			RequestEnqueue visitor(content);
			visitor(sql);
			return content.str();
		}



		string DBInterSYNTHESE::GetRStmtContent( const DBRecord& r )
		{
			stringstream content;
			RequestEnqueue visitor(content);
			visitor(r);
			return content.str();
		}



		DBInterSYNTHESE::RequestEnqueue::RequestEnqueue(
			std::stringstream& result
		):	_result(result)
		{}



		void DBInterSYNTHESE::RequestEnqueue::operator()( const std::string& sql )
		{
			_result <<
				DBInterSYNTHESE::TYPE_SQL << DBInterSYNTHESE::FIELD_SEPARATOR <<
				sql;
		}



		void DBInterSYNTHESE::RequestEnqueue::operator()( const DBRecord& r )
		{
			_result <<
				DBInterSYNTHESE::TYPE_REPLACE_STATEMENT << DBInterSYNTHESE::FIELD_SEPARATOR <<
				r.getTable()->getFormat().NAME << DBInterSYNTHESE::FIELD_SEPARATOR;

			ContentGetter visitor(_result);
			DBContent::const_iterator it(r.getContent().begin());
			BOOST_FOREACH(const FieldsList::value_type& field, r.getTable()->getFieldsList())
			{
				// Name
				_result << field.name << DBInterSYNTHESE::FIELD_SEPARATOR;

				// Size and content
				apply_visitor(visitor, *it);
				_result << DBInterSYNTHESE::FIELD_SEPARATOR;

				// Next content
				++it;
			}
		}



		DBInterSYNTHESE::ContentGetter::ContentGetter(
			std::stringstream& result
		):	_result(result)
		{}



		void DBInterSYNTHESE::ContentGetter::operator()( const int& i ) const
		{
			string s(lexical_cast<string>(i));
			_result <<
				0 << DBInterSYNTHESE::FIELD_SEPARATOR <<
				s.size() << DBInterSYNTHESE::FIELD_SEPARATOR <<
				s
			;
		}

		
		
		void DBInterSYNTHESE::ContentGetter::operator()( const size_t& i ) const
		{
			string s(lexical_cast<string>(i));
			_result <<
				0 << DBInterSYNTHESE::FIELD_SEPARATOR <<
				s.size() << DBInterSYNTHESE::FIELD_SEPARATOR <<
				s
			;
		}



		void DBInterSYNTHESE::ContentGetter::operator()( const double& d ) const
		{
			string s(lexical_cast<string>(d));
			_result <<
				0 << DBInterSYNTHESE::FIELD_SEPARATOR <<
				s.size() << DBInterSYNTHESE::FIELD_SEPARATOR <<
				s
			;
		}



		void DBInterSYNTHESE::ContentGetter::operator()( const util::RegistryKeyType& id ) const
		{
			string s(lexical_cast<string>(id));
			_result <<
				0 << DBInterSYNTHESE::FIELD_SEPARATOR <<
				s.size() << DBInterSYNTHESE::FIELD_SEPARATOR <<
				s
			;
		}



		void DBInterSYNTHESE::ContentGetter::operator()( const boost::optional<std::string>& str ) const
		{
			if(str)
			{
				_result <<
					0 << DBInterSYNTHESE::FIELD_SEPARATOR <<
					str->size() << DBInterSYNTHESE::FIELD_SEPARATOR <<
					*str
				;
			}
			else
			{
				_result <<
					1 << DBInterSYNTHESE::FIELD_SEPARATOR <<
					0 << DBInterSYNTHESE::FIELD_SEPARATOR
				;
			}
		}



		void DBInterSYNTHESE::ContentGetter::operator()( const boost::optional<Blob>& blob ) const
		{
			if(blob)
			{
				_result <<
					0 << DBInterSYNTHESE::FIELD_SEPARATOR <<
					blob->second << DBInterSYNTHESE::FIELD_SEPARATOR;
				for(size_t i(0); i<blob->second; ++i)
				{
					_result.put(blob->first[i]);
				}
			}
			else
			{
				_result <<
					1 << DBInterSYNTHESE::FIELD_SEPARATOR <<
					0 << DBInterSYNTHESE::FIELD_SEPARATOR
				;
			}
		}
}	}

