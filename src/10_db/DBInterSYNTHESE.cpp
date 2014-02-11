
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
#include "DBSQLInterSYNTHESEContent.hpp"
#include "DBTransaction.hpp"
#include "Env.h"
#include "Field.hpp"
#include "InterSYNTHESEConfigItem.hpp"
#include "InterSYNTHESEModule.hpp"
#include "InterSYNTHESEQueue.hpp"
#include "InterSYNTHESESlave.hpp"
#include "Log.h"

#include <geos/geom/Geometry.h>
#include <geos/io/WKTWriter.h>

using namespace boost;
using namespace std;
using namespace geos::io;

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
		const string DBInterSYNTHESE::TYPE_DELETE_STATEMENT = "dstmt";
		const string DBInterSYNTHESE::FIELD_SEPARATOR = ":";

		DBInterSYNTHESE::DBInterSYNTHESE():
			FactorableTemplate<InterSYNTHESESyncTypeFactory, DBInterSYNTHESE>()
		{
		}



		bool DBInterSYNTHESE::sync(
			const string& parameter,
			const InterSYNTHESEIdFilter* idFilter
		) const	{
			DB& db(*DBModule::GetDB());

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
					string sql(parameter.substr(i+1));
					trim(sql);
					if(sql.substr(0, 11) == "DELETE FROM")
					{
						db.execUpdate(sql, *_transaction);
					}
					else if(sql.substr(0, 12) == "REPLACE INTO")
					{
						size_t j(13);
						for(; j<sql.size() && sql[j]!=' '; ++j) ;
						string tableName = sql.substr(13, j-13);

						vector<string> p;
						split(p, sql, is_any_of("(,"));
						if(p.size() > 2)
						{
							RegistryKeyType id(lexical_cast<RegistryKeyType>(p[1]));
						
							db.execUpdate(sql, *_transaction);

							db.addDBModifEvent(
								DB::DBModifEvent(
									tableName,
				// TODO remove MODIF_UPDATE and _objectAdded attribute or find an other way to choose between the two types
				//					_objectAdded ? DB::MODIF_INSERT : DB::MODIF_UPDATE,
									DB::MODIF_INSERT,
									id
								),
								optional<DBTransaction&>()
							);

							// Inter-SYNTHESE sync
							DBSQLInterSYNTHESEContent content(
								decodeTableId(id),
								sql
							);
							inter_synthese::InterSYNTHESEModule::Enqueue(
								content,
								optional<DBTransaction&>()
							);
						}
					}
				}
				catch(...)
				{
					return false;
				}
			}
			else if(reqType == TYPE_DELETE_STATEMENT)
			{
				string idStr(parameter.substr(i+1));
				try
				{
					RegistryKeyType id(lexical_cast<RegistryKeyType>(idStr));
					RegistryTableType tableId(decodeTableId(id));
					string tableName(DBModule::GetTableSync(tableId)->getFormat().NAME);

					// STMT execution
					db.deleteStmt(
						id,
						*_transaction
					);
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
					// Table number
					string tableNumberStr(parameter.substr(l, i-l));
					RegistryTableType tableId(lexical_cast<RegistryTableType>(tableNumberStr));
					RegistryKeyType id(0);
					DBRecord r(*DBModule::GetTableSync(tableId));
					string tableName(r.getTable()->getFormat().NAME);
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
							++i;
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

							// Saving of id
							if(fieldName == TABLE_COL_ID)
							{
								id = lexical_cast<RegistryKeyType>(content);
							}
						}
						++i;
					}

					// Content extraction
					DBContent content;
					BOOST_FOREACH(const FieldsList::value_type& field, r.getTable()->getFieldsList())
					{
						StmtFields::const_iterator it(stmtFields.find(field.name));
						if(	it == stmtFields.end() ||
							!it->second
						){
							content.push_back(Cell(optional<string>()));
						}
						else
						{
							try
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
							catch(bad_lexical_cast&)
							{
								content.push_back(Cell(optional<string>()));
							}
						}
					}

					r.setContent(content);

					db.replaceStmt(
						id,
						r,
						*_transaction
					);
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



		boost::shared_ptr<DBTableSync> DBInterSYNTHESE::_getTableSync( const std::string& perimeter )
		{
			// Detection of the table by id
			RegistryTableType tableId(
				lexical_cast<RegistryTableType>(perimeter)
			);
			return DBModule::GetTableSync(tableId);
		}



		void DBInterSYNTHESE::initQueue(
			const InterSYNTHESESlave& slave,
			const std::string& perimeter
		) const	{

			try
			{
				boost::shared_ptr<DBTableSync> tableSync(_getTableSync(perimeter));

				boost::shared_ptr<DBDirectTableSync> directTableSync(
					dynamic_pointer_cast<DBDirectTableSync, DBTableSync>(
						tableSync
				)	);
				if(!directTableSync.get())
				{
					return;
				}

				// Getting all requests
/*				Env env;
				DBDirectTableSync::RegistrableSearchResult result(
					directTableSync->search(
						string(),
						env,
						FIELDS_ONLY_LOAD_LEVEL
				)	); */
				DBResultSPtr rows(directTableSync->searchRecords(string()));

				DBTransaction transaction(false);

				// Add the clean request
				if(	!slave.get<InterSYNTHESEConfig>() ||
					!slave.get<InterSYNTHESEConfig>()->get<Multimaster>()
				){
					transaction.addQuery("DELETE FROM "+ tableSync->getFormat().NAME);
				}

				// Build the dump
				while (rows->next())
				//BOOST_FOREACH(const DBDirectTableSync::RegistrableSearchResult::value_type& it, result)
				{
					size_t cols(rows->getNbColumns());
					DBContent content;
					for(size_t col(0); col < cols; ++col)
					{
						content.push_back(rows->getText(col));
					}
					DBRecord r(*tableSync);
					r.setContent(content);
					transaction.addReplaceStmt(r);
//					directTableSync->saveRegistrable(*it, transaction);
				}

				// Export
				DBTransaction saveTransaction;
				BOOST_FOREACH(const DBTransaction::Query& query, transaction.getQueries())
				{
					stringstream content;
					RequestEnqueue visitor(content);
					if(apply_visitor(visitor, query))
					{
						slave.enqueue(
							DBInterSYNTHESE::FACTORY_KEY,
							content.str(),
							saveTransaction,
							true
						);
					}
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



		DBInterSYNTHESE::RequestEnqueue::RequestEnqueue(
			std::stringstream& result
		):	_result(result)
		{}



		bool DBInterSYNTHESE::RequestEnqueue::operator()( const std::string& sql )
		{
			_result <<
				DBInterSYNTHESE::TYPE_SQL << DBInterSYNTHESE::FIELD_SEPARATOR <<
				sql;
			return true;
		}



		bool DBInterSYNTHESE::RequestEnqueue::operator()( const DBRecord& r )
		{
			assert(r.getTable()->getFormat().ID != InterSYNTHESEQueue::CLASS_NUMBER);
			_result <<
				DBInterSYNTHESE::TYPE_REPLACE_STATEMENT << DBInterSYNTHESE::FIELD_SEPARATOR <<
				r.getTable()->getFormat().ID << DBInterSYNTHESE::FIELD_SEPARATOR;

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
			return true;
		}



		bool DBInterSYNTHESE::RequestEnqueue::operator()(
			util::RegistryKeyType id
		){
			_result <<
				DBInterSYNTHESE::TYPE_DELETE_STATEMENT << DBInterSYNTHESE::FIELD_SEPARATOR <<
				id
			;
			return true;
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

		
#ifndef _WINDOWS
		void DBInterSYNTHESE::ContentGetter::operator()( const size_t& i ) const
		{
			string s(lexical_cast<string>(i));
			_result <<
				0 << DBInterSYNTHESE::FIELD_SEPARATOR <<
				s.size() << DBInterSYNTHESE::FIELD_SEPARATOR <<
				s
			;
		}
#endif


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
				_result.write(blob->first, blob->second);
			}
			else
			{
				_result <<
					1 << DBInterSYNTHESE::FIELD_SEPARATOR <<
					0 << DBInterSYNTHESE::FIELD_SEPARATOR
				;
			}
		}



		void DBInterSYNTHESE::ContentGetter::operator()( const boost::shared_ptr<geos::geom::Geometry>& geom ) const
		{
			string str;
			if(geom)
			{
				boost::shared_ptr<geos::geom::Geometry> projected(geom);
				if(	CoordinatesSystem::GetStorageCoordinatesSystem().getSRID() !=
					static_cast<CoordinatesSystem::SRID>(geom->getSRID())
					){
						projected = CoordinatesSystem::GetStorageCoordinatesSystem().convertGeometry(*geom);
				}

				WKTWriter wkt;

				if(projected->isValid())
				{
					str = wkt.write(projected.get());
				}
				else
				{
					Log::GetInstance().warn("bad geometry, maybe due to a failed conversion");
				}
			}
			_result <<
				0 << DBInterSYNTHESE::FIELD_SEPARATOR <<
				str.size() << DBInterSYNTHESE::FIELD_SEPARATOR <<
				str
			;
		}



		
		bool DBInterSYNTHESE::mustBeEnqueued(
			const std::string& configPerimeter,
			const std::string& messagePerimeter
		) const {
			// Do not enqueue if not in the perimeter and if it
			// is the InterSYNTHESEQueue itself. It may happens if the user select
			// this table in the inter_synthese_admin configuration but
			// this is non sense.
			return configPerimeter == messagePerimeter &&
				messagePerimeter != lexical_cast<string>(InterSYNTHESEQueue::CLASS_NUMBER);
		}



		void DBInterSYNTHESE::initSync() const
		{
			assert(!_transaction.get());

			_transaction.reset(new DBTransaction);
		}



		void DBInterSYNTHESE::closeSync() const
		{
			assert(_transaction.get());

			_transaction->run();
			_transaction.reset();
		}



		bool DBInterSYNTHESE::ItemsLess::operator()( const InterSYNTHESEConfigItem* lhs, const InterSYNTHESEConfigItem* rhs ) const
		{
			if(lhs == NULL && rhs == NULL)
			{
				return false;
			}

			if(lhs == NULL)
			{
				return true;
			}

			if(rhs == NULL)
			{
				return false;
			}

			boost::shared_ptr<DBTableSync> table1(_getTableSync(lhs->get<SyncPerimeter>()));
			boost::shared_ptr<DBTableSync> table2(_getTableSync(rhs->get<SyncPerimeter>()));

			return table1->getFactoryKey() < table2->getFactoryKey();
		}



		DBInterSYNTHESE::SortedItems DBInterSYNTHESE::sort( const RandomItems& randItems ) const
		{
			set<const InterSYNTHESEConfigItem*, ItemsLess> sortedSet(randItems.begin(), randItems.end());
			SortedItems result(sortedSet.begin(), sortedSet.end());
			return result;
		}
}	}

