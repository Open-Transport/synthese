////////////////////////////////////////////////////////////////////////////////
/// DBDirectTableSyncTemplate class header.
///	@file DBDirectTableSyncTemplate.hpp
///	@author Hugues Romain
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

#ifndef SYNTHESE_db_DBDirectTableSyncTemplate_hpp__
#define SYNTHESE_db_DBDirectTableSyncTemplate_hpp__

#include "DBDirectTableSync.hpp"
#include "DBTableSyncTemplate.hpp"

#include "DBTypes.h"
#include "DB.hpp"
#include "DBResult.hpp"
#include "DBEmptyResultException.h"
#include "DBException.hpp"
#include "Exception.h"
#include "SelectQuery.hpp"

#include <sstream>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/optional/optional.hpp>

#include <geos/geom/Point.h>
#include <geos/geom/Envelope.h>

#include "FactorableTemplate.h"
#include "Constants.h"
#include "Conversion.h"
#include "Env.h"
#include "01_util/Log.h"


namespace synthese
{
	namespace db
	{
		class DBTransaction;

		////////////////////////////////////////////////////////////////////////
		///	Table synchronizer template (abstract class) for standard SYNTHESE
		///	tables.
		///
		/// A standard SYNTHESE table has the following properties :
		///		- Removal allowed
		///		- Autoincrement activated
		///		- an internal class corresponds to the records of the table
		///		- a Load and a Save function convert records into class object
		///			and vice versa
		///	@ingroup m10
		////////////////////////////////////////////////////////////////////////
		template <class K, class T>
		class DBDirectTableSyncTemplate:
			public DBTableSyncTemplate<K>,
			public DBDirectTableSync
		{
		public:
			typedef K		TableSync;
			typedef T		ObjectType;
			typedef std::vector<boost::shared_ptr<T> > SearchResult;


		public:
			static FieldsList GetFieldsList()
			{
				if(_FIELDS[0].empty())
				{
					T object;
					return object.getFields();
				}
				else
				{
					FieldsList l;
					for(size_t i(0); !_FIELDS[i].empty(); ++i)
					{
						l.push_back(_FIELDS[i]);
					}
					return l;
				}
			}

			////////////////////////////////////////////////////////////////////
			/// Object fetcher, with read/write permissions.
			///	@param key UID of the object
			///	@param env Environment to write the object and read/write the links.
			///		- if Default/NULL : no environment, then object is only read from the database
			///		- if non NULL : the object is read from the environment if exists, else from the database
			///		- use Env::GetOfficialEnv() to use the official environment
			///	@param linkLevel Level of load recursion (see each TableSync to know precisely the signification of each level for each class)
			///	@param autoCreate :
			///		- AUTO_CREATE : returns an empty object if the specified key was not found in the table (the returned object is <u>not</u> stored in the database).
			///		- NEVER_CREATE (default) : throws a ObjectNotFound<T> exception if the specified key was not found in the table
			///	@return shared pointer Pointer to an object corresponding to the fetched record.
			///	@throw ObjectNotFound<T> if the object was not found and if autoCreate is deactivated
			///	@todo implement upgrade of load level
			static boost::shared_ptr<T> GetEditable(
				util::RegistryKeyType key,
				util::Env& env,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL,
				AutoCreation autoCreate = NEVER_CREATE
			){
				if (env.getRegistry<T>().contains(key))
				{
					return env.getEditableRegistry<T>().getEditable(key);
				}

				boost::shared_ptr<T> object;
				try
				{
					DBResultSPtr rows(DBTableSyncTemplate<K>::_GetRow(key));
					object.reset(new T(rows->getKey()));
					env.getEditableRegistry<T>().add(object);
					Load(object.get(), rows, env, linkLevel);
				}
				catch (typename db::DBEmptyResultException<K>&)
				{
					if (autoCreate == NEVER_CREATE)
					{
						throw util::ObjectNotFoundException<T>(key, "Object not found in "+ K::TABLE.NAME);
					}
					object.reset(new T(key));
				}

				return object;
			}

			virtual boost::shared_ptr<util::Registrable> getEditableRegistrable(
				util::RegistryKeyType key,
				util::Env& environment,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL,
				AutoCreation autoCreate = NEVER_CREATE
			) const {
				return boost::dynamic_pointer_cast<util::Registrable, T>(GetEditable(key, environment, linkLevel, autoCreate));
			}


			/** Object fetcher, with read only permissions.
				@param key UID of the object
				@param environment Environment to write the object and read/write the links.
					- if Default/NULL : no environment, then object is only read from the database
					- if non NULL : the object is read from the environment if exists, else from the database
					- use Env::GetOfficialEnv() to use the official environment
				@param linkLevel Level of load recursion (see each TableSync to know precisely the signification of each level for each class)
				@return shared pointer Pointer to an object corresponding to the fetched record.
				@throw DBEmptyResultException if the object was not found

				Note : in case of writing in the environment, the object present in the environment will have read/write permissions.
			*/
			static boost::shared_ptr<const T> Get(
				util::RegistryKeyType key,
				util::Env& environment,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL,
				AutoCreation autoCreate = NEVER_CREATE
			){
				return boost::const_pointer_cast<const T>(GetEditable(key,environment,linkLevel,autoCreate));
			}



			virtual boost::shared_ptr<const util::Registrable> getRegistrable(
				util::RegistryKeyType key,
				util::Env& environment,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL,
				AutoCreation autoCreate = NEVER_CREATE
			) const {
				return boost::dynamic_pointer_cast<const util::Registrable, const T>(Get(key, environment, linkLevel, autoCreate));
			}



			/** Object properties loader from the database.
				@param obj Pointer to the object to load from the database
				@param rows Row to read
				@param environment Environment to read and populate
				@author Hugues Romain
				@date 2007
				@warning To complete the load when building the RAM environment, follow the properties load by the link method
			*/
			static void Load(
				T* obj,
				const DBResultSPtr& rows,
				util::Env& environment,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);


			virtual void loadRegistrable(
				util::Registrable& obj,
				const DBResultSPtr& rows,
				util::Env& environment,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			) const {
				Load(&dynamic_cast<T&>(obj), rows, environment, linkLevel);
			}

			static void Unlink(
				T* obj
			);


			/** Saving of the object in database.
				@param obj Object to save
				@author Hugues Romain
				@date 2007

				The object is recognized by its key :
				- if the object has already a key, then the corresponding record is replaced
				- if the object does not have any key, then the autoincrement function generates one for it.
			*/
			static void Save(
				T* obj,
				boost::optional<DBTransaction&> transaction = boost::optional<DBTransaction&>()
			);


			virtual void saveRegistrable(
				util::Registrable& obj,
				boost::optional<DBTransaction&> transaction = boost::optional<DBTransaction&>()
			) const {
				Save(&dynamic_cast<T&>(obj), transaction);
			}



			////////////////////////////////////////////////////////////////////
			///	Object creator helper.
			///	@param row data row
			///	@return boost::shared_ptr<T> The created object
			///	@author Hugues Romain
			///	@date 2008
			/// This static method can be overloaded.
			////////////////////////////////////////////////////////////////////
			static boost::shared_ptr<T> GetNewObject(
				const DBResultSPtr& row
			){
				return boost::shared_ptr<T>(new T(row->getKey()));
			}



			virtual boost::shared_ptr<util::Registrable> createRegistrable(
				const DBResultSPtr& row
			) const	{
				return boost::dynamic_pointer_cast<util::Registrable, T>(GetNewObject(row));
			}


			static SearchResult LoadFromQuery(
				const SelectQuery<K>& query,
				util::Env& env,
				util::LinkLevel linkLevel
			){
				return LoadFromQuery(query.toString(), env, linkLevel);
			}


			////////////////////////////////////////////////////////////////////
			/// Load objects into an environment, from a SQL query, and return
			/// the list of loaded objects.
			///	@param query SQL query
			///	@param env Environment to write
			///	@param linkLevel Link level
			/// @return search result (oredered vector of pointer to objects)
			///	@throws Exception if the load failed
			////////////////////////////////////////////////////////////////////
			static SearchResult LoadFromQuery(
				const std::string& query,
				util::Env& env,
				util::LinkLevel linkLevel
			){
				SearchResult result;
				util::Registry<T>& registry(env.template getEditableRegistry<T>());
				DBResultSPtr rows = DBModule::GetDB()->execQuery(query);
				while (rows->next ())
				{
					util::RegistryKeyType key(rows->getKey());
					try
					{
						if(registry.contains(key))
						{
							result.push_back(registry.getEditable(key));
						}
						else
						{
							boost::shared_ptr<T> object(K::GetNewObject(rows));
							registry.add(object);
							Load(object.get(), rows, env, linkLevel);
							result.push_back(object);
					}	}
					catch(std::exception& e)
					{
						if(registry.contains(key))
						{
							registry.remove(key);
						}
						util::Log::GetInstance().warn("Skipped object in results load of " + query, e);
				}	}
				return result;
			}


			//////////////////////////////////////////////////////////////////////////
			/// Search of objects by maximal distance from a point.
			/// @pre the envelope must be at the default SRID
			/// @param envelope the envelope which the objects must belong to
			/// @param env the environment to populate
			/// @param linkLevel level of link
			///	@return Found objects
			///	@author Hugues Romain
			///	@date 2010
			/// @since 3.2.1
			static SearchResult SearchByEnvelope(
				const geos::geom::Envelope& envelope,
				util::Env& env,
				util::LinkLevel linkLevel
			){
				// TODO: MySQL support.

				std::stringstream subQuery;
				subQuery << "SELECT pkid FROM idx_" << K::TABLE.NAME << "_" << TABLE_COL_GEOMETRY << " WHERE " <<
					"xmin > " << envelope.getMinX() << " AND xmax < " << envelope.getMaxX() <<
					" AND ymin > " << envelope.getMinY() << " AND ymax < " << envelope.getMaxY()
				;

				SelectQuery<K> query;
				query.addTableField(TABLE_COL_ID);
				query.addWhere(
					ComposedExpression::Get(
						FieldExpression::Get(K::TABLE.NAME, TABLE_COL_ID),
						ComposedExpression::OP_IN,
						SubQueryExpression::Get(subQuery.str())
				)	);

				return LoadFromQuery(query, env, linkLevel);
			}


			//////////////////////////////////////////////////////////////////////////
			/// Search of objects by maximal distance from a point.
			/// @pre the table must have a geometry
			/// @param point point from where calculate the distance
			/// @param distanceLimit maximal distance
			/// @param env the environment to populate
			/// @param linkLevel level of link
			///	@return Found objects
			///	@author Hugues Romain
			///	@date 2010
			/// @since 3.2.1
			static SearchResult SearchByMaxDistance(
				const geos::geom::Point& point,
				double distanceLimit,
				util::Env& env,
				util::LinkLevel linkLevel
			){
				// TODO: MySQL support.

				if(point.isEmpty())
				{
					return SearchResult();
				}

				boost::shared_ptr<geos::geom::Point> minPoint(
					DBModule::GetStorageCoordinatesSystem().convertPoint(
						*CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(
							point.getX() - distanceLimit,
							point.getY() - distanceLimit
				)	)	);
				boost::shared_ptr<geos::geom::Point> maxPoint(
					DBModule::GetStorageCoordinatesSystem().convertPoint(
						*CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(
							point.getX() + distanceLimit,
							point.getY() + distanceLimit
				)	)	);

				geos::geom::Envelope envelope(*minPoint->getCoordinate(), *maxPoint->getCoordinate());

				return SearchByEnvelope(envelope, env, linkLevel);
			}
		};
	}
}

#endif // SYNTHESE_db_DBDirectTableSyncTemplate_hpp__
