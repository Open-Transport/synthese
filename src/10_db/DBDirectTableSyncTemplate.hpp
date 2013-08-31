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

#include "FullSynchronizationPolicy.hpp"
#include "StandardLoadSavePolicy.hpp"

#include "DBTypes.h"
#include "DB.hpp"
#include "DBResult.hpp"
#include "DBEmptyResultException.h"
#include "DBException.hpp"
#include "Exception.h"
#include "RegistryBase.h"
#include "SelectQuery.hpp"

#include <sstream>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/optional/optional.hpp>

#include <geos/geom/Point.h>
#include <geos/geom/Envelope.h>

#include "FactorableTemplate.h"
#include "Constants.h"
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
		template <
			class K, // Table sync class
			class T, // Object class
			template <class, class> class SynchronizationPolicy = FullSynchronizationPolicy,
			template <class, class> class LoadSavePolicy = StandardLoadSavePolicy // for compatibility with old classes
		>
		class DBDirectTableSyncTemplate:
			public DBTableSyncTemplate<K>,
			public DBDirectTableSync,
			public SynchronizationPolicy<K, T>,
			public LoadSavePolicy<K, T>
		{
		public:
			typedef K		TableSync;
			typedef T		ObjectType;
			typedef std::vector<boost::shared_ptr<T> > SearchResult;


		public:


			virtual FieldsList getFieldsList() const { return K::GetFieldsList(); }



			////////////////////////////////////////////////////////////////////
			/// Object fetcher, with read/write permissions.
			///	@param key UID of the object
			///	@param env Environment to write the object and read/write the links.
			///		- if Default/NULL : no environment, then object is only read from the database
			///		- if non NULL : the object is read from the environment if exists, else from the database
			///		- use Env::GetOfficialEnv() to use the official environment
			///	@param linkLevel Level of load recursion (see each TableSync to know precisely the signification of each level for each class)
			///	@return shared pointer Pointer to an object corresponding to the fetched record.
			///	@throw ObjectNotFound<T> if the object was not found and if autoCreate is deactivated
			///	@todo implement upgrade of load level
			static boost::shared_ptr<T> GetEditable(
				util::RegistryKeyType key,
				util::Env& env,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
			


			template<class S>
			static boost::shared_ptr<S> GetCastEditable(
				util::RegistryKeyType key,
				util::Env& env,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);


			virtual boost::shared_ptr<util::Registrable> getEditableRegistrable(
				util::RegistryKeyType key,
				util::Env& environment,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			) const {
				return GetCastEditable<util::Registrable>(key, environment, linkLevel);
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
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			template<class S>
			static boost::shared_ptr<const S> GetCast(
				util::RegistryKeyType key,
				util::Env& environment,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);


			virtual boost::shared_ptr<const util::Registrable> getRegistrable(
				util::RegistryKeyType key,
				util::Env& environment,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			) const {
				return GetCast<util::Registrable>(key, environment, linkLevel);
			}



			virtual void loadRegistrable(
				util::Registrable& obj,
				const DBResultSPtr& rows,
				util::Env& environment,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			) const {
				K::Load(&dynamic_cast<T&>(obj), rows, environment, linkLevel);
			}



			virtual void saveRegistrable(
				util::Registrable& obj,
				boost::optional<DBTransaction&> transaction = boost::optional<DBTransaction&>()
			) const {
				K::Save(&dynamic_cast<T&>(obj), transaction);
			}



			virtual boost::shared_ptr<util::Registrable> newObject(
			) const {
				return boost::dynamic_pointer_cast<util::Registrable, T>(
					K::GetNewObject()
				);
			}



			virtual boost::shared_ptr<util::Registrable> newObject(
				const Record& record
			) const {
				return boost::dynamic_pointer_cast<util::Registrable, T>(
					K::GetNewObject(record)
				);
			}


			virtual boost::shared_ptr<util::Registrable> createRegistrable(
				const DBResultSPtr& row
			) const	{
				return boost::dynamic_pointer_cast<util::Registrable, T>(K::GetNewObject(*row));
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
							boost::shared_ptr<T> object(K::GetNewObject(*rows));
							registry.add(object);
							K::Load(object.get(), rows, env, linkLevel);
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
				SelectQuery<K> query;
				std::stringstream subQuery;

				if(DBModule::GetDB()->isBackend(DB::SQLITE_BACKEND))
				{
					string tableName = "idx_" + K::TABLE.NAME + "_" + TABLE_COL_GEOMETRY;

					/* Spatial lequest on linestring (or other geometrical objects) enveloppes indexes :
					* The linestring must be retained if it does have an intersection with the "envelope" parameter
					* There is an intersection if the linestring envelope have at least an intersection with the envelope
					*/
					subQuery << "( c.xmin < " << envelope.getMaxX() << " AND c.xmax > " << envelope.getMinX() <<
						" AND c.ymin < " << envelope.getMaxY() << " AND c.ymax > " << envelope.getMinY() << " )"
						;

					/* WARNING :
					* sqlite crash (core dump) with this spatial request if join isn't explicit.
					* so please keep it explicit here
					*/

					query.addTableAndEqualJoin("pkid", TABLE_COL_ID, tableName, "c");
					query.addWhere(
						SubQueryExpression::Get(subQuery.str())
						);
				}
				else if(DBModule::GetDB()->isBackend(DB::MYSQL_BACKEND))
				{
					subQuery << "MBRContains(GeomFromText('Polygon((" <<
						envelope.getMinX() << " " << envelope.getMinY() << "," <<
						envelope.getMinX() << " " << envelope.getMaxY() << "," <<
						envelope.getMaxX() << " " << envelope.getMaxY() << "," <<
						envelope.getMaxX() << " " << envelope.getMinY() << "," <<
						envelope.getMinX() << " " << envelope.getMinY() << "))'),geometry)";
						
					query.addWhere(
						SubQueryExpression::Get(subQuery.str())
						);
				}

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
				if(point.isEmpty())
				{
					return SearchResult();
				}

				boost::shared_ptr<geos::geom::Point> minPoint(
					CoordinatesSystem::GetStorageCoordinatesSystem().convertPoint(
						*CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(
							point.getX() - distanceLimit,
							point.getY() - distanceLimit
				)	)	);
				boost::shared_ptr<geos::geom::Point> maxPoint(
					CoordinatesSystem::GetStorageCoordinatesSystem().convertPoint(
						*CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(
							point.getX() + distanceLimit,
							point.getY() + distanceLimit
				)	)	);

				geos::geom::Envelope envelope(*minPoint->getCoordinate(), *maxPoint->getCoordinate());

				return SearchByEnvelope(envelope, env, linkLevel);
			}



			virtual const util::RegistryBase& getRegistry(
				const util::Env& env
			) const {
				return static_cast<const util::RegistryBase&>(env.getRegistry<T>());
			}



			virtual util::RegistryBase& getEditableRegistry(
				util::Env& env
			) const {
				return static_cast<util::RegistryBase&>(env.getEditableRegistry<T>());
			}



			virtual RegistrableSearchResult search(
				const std::string& whereClause,
				util::Env& environment,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			) const;



			virtual void rowsAdded(
				DB* db,
				const DBResultSPtr& rows
			) const {
				K::RowsAdded(db, rows);
			}

			virtual void rowsUpdated(
				DB* db,
				const DBResultSPtr& rows
			) const {
				K::RowsUpdated(db, rows);
			}

			virtual void rowsRemoved(
				DB* db,
				const RowIdList& rowIds
			) const {
				K::RowsRemoved(db, rowIds);
			}



			static bool Contains(
				util::RegistryKeyType id
			);


			virtual bool contains(util::RegistryKeyType id) const { return Contains(id); }
		};



		template <
			class K, // Table sync class
			class T, // Object class
			template <class, class> class SynchronizationPolicy,
			template <class, class> class LoadSavePolicy
		>
		bool DBDirectTableSyncTemplate<K, T, SynchronizationPolicy, LoadSavePolicy>::Contains(
			util::RegistryKeyType id
		){
			SelectQuery<K> q;
			q.addTableField(TABLE_COL_ID);
			q.addWhereField(TABLE_COL_ID, id);
			DBResultSPtr rows(q.execute());
			return rows->next();
		}



		template <
			class K, // Table sync class
			class T, // Object class
			template <class, class> class SynchronizationPolicy,
			template <class, class> class LoadSavePolicy
		>
		DBDirectTableSync::RegistrableSearchResult DBDirectTableSyncTemplate<K, T, SynchronizationPolicy, LoadSavePolicy>::search(
			const std::string& whereClause,
			util::Env& env,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		) const	{

			std::stringstream query;
			query <<
				"SELECT " << DBTableSyncTemplate<K>::GetFieldsGetter() <<
				" FROM " << K::TABLE.NAME;
			if(!whereClause.empty())
			{
				query << " WHERE " << whereClause;
			}
			RegistrableSearchResult result;
			util::Registry<T>& registry(env.template getEditableRegistry<T>());
			DBResultSPtr rows = DBModule::GetDB()->execQuery(query.str());
			while (rows->next ())
			{
				util::RegistryKeyType key(rows->getKey());
				try
				{
					if(registry.contains(key))
					{
						result.push_back(
							boost::static_pointer_cast<util::Registrable, T>(
								registry.getEditable(key)
						)	);
					}
					else
					{
						boost::shared_ptr<T> object(
							K::GetNewObject(*rows)
						);
						registry.add(object);
						Load(object.get(), rows, env, linkLevel);
						result.push_back(
							boost::static_pointer_cast<util::Registrable, T>(
								object
						)	);
				}	}
				catch(std::exception& e)
				{
					if(registry.contains(key))
					{
						registry.remove(key);
					}
					util::Log::GetInstance().warn("Skipped object in results load of " + query.str(), e);
			}	}
			return result;

		}



		template<
			class K,
			class T,
			template <class, class> class SynchronizationPolicy,
			template <class, class> class LoadSavePolicy
		>
		boost::shared_ptr<T> DBDirectTableSyncTemplate<K, T, SynchronizationPolicy, LoadSavePolicy>::GetEditable(
			util::RegistryKeyType key,
			util::Env& env,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL*/
		){
			if (env.getRegistry<T>().contains(key))
			{
				return env.getEditableRegistry<T>().getEditable(key);
			}

			boost::shared_ptr<T> object;
			try
			{
				DBResultSPtr rows(DBTableSyncTemplate<K>::_GetRow(key));
				object = K::GetNewObject(*rows);
				env.getEditableRegistry<T>().add(object);
				K::Load(object.get(), rows, env, linkLevel);
			}
			catch (typename db::DBEmptyResultException<K>&)
			{
				throw util::ObjectNotFoundException<T>(key, "Object not found in "+ K::TABLE.NAME);
			}

			return object;
		}



		template<
			class K,
			class T,
			template <class, class> class SynchronizationPolicy,
			template <class, class> class LoadSavePolicy
		>
		template<class S>
		boost::shared_ptr<S> DBDirectTableSyncTemplate<K, T, SynchronizationPolicy, LoadSavePolicy>::GetCastEditable(
			util::RegistryKeyType key,
			util::Env& environment,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL*/
		){
			try
			{
				boost::shared_ptr<T> t(GetEditable(key, environment, linkLevel));
				if(!dynamic_cast<S*>(t.get()))
				{
					throw util::ObjectNotFoundException<S>(key, "Bad type conversion from "+ K::TABLE.NAME);
				}
				return boost::static_pointer_cast<S, T>(t);
			}
			catch(util::ObjectNotFoundException<T>& e)
			{
				throw util::ObjectNotFoundException<S>(key, e.getMessage());
			}
		}



		template<
			class K,
			class T,
			template <class, class> class SynchronizationPolicy,
			template <class, class> class LoadSavePolicy
		>
		boost::shared_ptr<const T> DBDirectTableSyncTemplate<K, T, SynchronizationPolicy, LoadSavePolicy>::Get(
			util::RegistryKeyType key,
			util::Env& environment,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL*/
		){
			return boost::const_pointer_cast<const T>(
				GetEditable(key,environment,linkLevel)
			);
		}



		template<
			class K,
			class T,
			template <class, class> class SynchronizationPolicy,
			template <class, class> class LoadSavePolicy
		>
		template<class S>
		boost::shared_ptr<const S> DBDirectTableSyncTemplate<K, T, SynchronizationPolicy, LoadSavePolicy>::GetCast(
			util::RegistryKeyType key,
			util::Env& environment,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL*/
		){
			return boost::const_pointer_cast<const S>(
				GetCastEditable<S>(key,environment,linkLevel)
			);
		}
}	}

#endif // SYNTHESE_db_DBDirectTableSyncTemplate_hpp__
