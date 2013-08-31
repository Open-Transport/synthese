
/** InterSYNTHESEPackage class implementation.
	@file InterSYNTHESEPackage.cpp

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

#include "InterSYNTHESEPackage.hpp"

#include "DBDirectTableSync.hpp"
#include "InterSYNTHESEModule.hpp"
#include "Import.hpp"
#include "MD5Wrapper.h"
#include "User.h"

using namespace boost;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace impex;
	using namespace inter_synthese;
	using namespace security;
	using namespace util;

	CLASS_DEFINITION(InterSYNTHESEPackage, "t111_inter_synthese_packages", 111)
	FIELD_DEFINITION_OF_OBJECT(InterSYNTHESEPackage, "inter_synthese_package_id", "inter_synthese_package_ids")

	FIELD_DEFINITION_OF_TYPE(Code, "code", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(LockUser, "lock_user_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(LockTime, "lock_time", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(LockServerName, "lock_server_name", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(LastJSON, "last_json", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Objects, "object_ids", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(FullTables, "full_tables", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Public, "public", SQL_BOOLEAN)

	namespace inter_synthese
	{
		const string InterSYNTHESEPackage::TAG_IMPORT = "import";
		const string InterSYNTHESEPackage::TAG_LOCK_USER = "lock_user";
		const string InterSYNTHESEPackage::TAG_OBJECT = "object";
		const string InterSYNTHESEPackage::SEPARATOR = ":";
		const string InterSYNTHESEPackage::TAG_TABLE = "table";
		const string InterSYNTHESEPackage::ATTR_ID = "id";
		const string InterSYNTHESEPackage::ATTR_NAME = "name";
		


		InterSYNTHESEPackage::InterSYNTHESEPackage(
			util::RegistryKeyType id /*= 0*/
		):	Registrable(id),
			Object<InterSYNTHESEPackage, InterSYNTHESEPackageRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(Code),
					FIELD_DEFAULT_CONSTRUCTOR(Objects),
					FIELD_DEFAULT_CONSTRUCTOR(FullTables),
					FIELD_DEFAULT_CONSTRUCTOR(Import),
					FIELD_DEFAULT_CONSTRUCTOR(LockUser),
					FIELD_DEFAULT_CONSTRUCTOR(LockTime),
					FIELD_DEFAULT_CONSTRUCTOR(LockServerName),
					FIELD_DEFAULT_CONSTRUCTOR(LastJSON),
					FIELD_VALUE_CONSTRUCTOR(Public, false)
			)	)
		{}
		


		//////////////////////////////////////////////////////////////////////////
		/// Gets if the current SYNTHESE server is the repository of the current
		/// package.
		/// The current SYNTESE server is the repository of the package when no
		/// import is defined on it.
		bool InterSYNTHESEPackage::isRepository() const
		{
			return !get<Import>();
		}



		//////////////////////////////////////////////////////////////////////////
		/// Gets if the current SYNTHESE server is a working copy of the current
		/// package.
		/// The current SYNTESE server is a working copy when an
		/// import is defined on it.
		bool InterSYNTHESEPackage::isWorkingCopy() const
		{
			return get<Import>();
		}



		//////////////////////////////////////////////////////////////////////////
		/// Gets if the current package can be written, depending on the lock status.
		/// For the repository : the package must not be locked by a working copy.
		/// For a working copy : the package must be locked by itself.
		bool InterSYNTHESEPackage::isWritable() const
		{
			return
				(isWorkingCopy() && !get<LockTime>().is_not_a_date_time()) ||
				(isRepository() && get<LockTime>().is_not_a_date_time()) ;
		}



		//////////////////////////////////////////////////////////////////////////
		/// Exports import, lock user, and objects contained in the package.
		void InterSYNTHESEPackage::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix /*= std::string() */
		) const	{

			// Import
			if(get<Import>())
			{
				boost::shared_ptr<ParametersMap> importPM(new ParametersMap);
				get<Import>()->toParametersMap(*importPM);
				map.insert(TAG_IMPORT, importPM);
			}

			// Lock user
			if(get<LockUser>())
			{
				boost::shared_ptr<ParametersMap> lockUserPM(new ParametersMap);
				get<LockUser>()->toParametersMap(*lockUserPM, true);
				map.insert(TAG_LOCK_USER, lockUserPM);
			}

			// Objects
			BOOST_FOREACH(const Objects::Type::value_type& item, get<Objects>())
			{
				boost::shared_ptr<ParametersMap> objectPM(new ParametersMap);
				item->toParametersMap(*objectPM, false);
				map.insert(TAG_OBJECT, objectPM);
			}

			// Tables
			BOOST_FOREACH(const FullTables::Type::value_type& table, get<FullTables>())
			{
				boost::shared_ptr<ParametersMap> tablePM(new ParametersMap);
				tablePM->insert(ATTR_ID, lexical_cast<string>(table->getFormat().ID));
				tablePM->insert(ATTR_NAME, table->getFormat().NAME);
				map.insert(TAG_TABLE, tablePM);
			}
		}



		//////////////////////////////////////////////////////////////////////////
		/// Locks the package and build the dump of its content.
		/// See _buildDump for the dump format.
		//////////////////////////////////////////////////////////////////////////
		/// @param user the user who locks the package
		/// @param serverName the name of the server who locks the package
		/// @pre the lock mutex is locked
		/// @pre if lock asked, the user has sufficient right to lock the package
		/// @pre the package is not already locked
		std::string InterSYNTHESEPackage::lockAndBuildDump(
			const security::User& user,
			const std::string& serverName
		){
			ptime now(second_clock::local_time());
			set<LockTime>(now);
			set<LockUser>(const_cast<User&>(user));
			set<LockServerName>(serverName);
			return _buildDump(now);
		}



		//////////////////////////////////////////////////////////////////////////
		/// Build the dump of the package content without locking it.
		/// See _buildDump for the dump format.
		//////////////////////////////////////////////////////////////////////////
		string InterSYNTHESEPackage::buildDump(
		) const {
			return _buildDump(ptime(not_a_date_time));
		}



		//////////////////////////////////////////////////////////////////////////
		/// Locks the package by a local user.
		//////////////////////////////////////////////////////////////////////////
		/// @param user the user who locks the package
		/// @pre the lock mutex is locked
		/// @pre if lock asked, the user has sufficient right to lock the package
		/// @pre the package is not already locked
		void InterSYNTHESEPackage::localLock( const security::User& user )
		{
			ptime now(second_clock::local_time());
			set<LockTime>(now);
			set<LockUser>(const_cast<User&>(user));
			set<LockServerName>(string());
		}




		//////////////////////////////////////////////////////////////////////////
		/// Unlocks the package by a local user.
		//////////////////////////////////////////////////////////////////////////
		/// @pre the lock mutex is locked
		/// @pre the package was already locked
		void InterSYNTHESEPackage::unlock()
		{
			ptime nadt(not_a_date_time);
			set<LockTime>(nadt);
			set<LockUser>(optional<User&>());
			set<LockServerName>(string());
		}



		//////////////////////////////////////////////////////////////////////////
		/// Builds a dump of the package.
		/// A dump consists in two sections :
		///  - the json section, containing all non binary fields. Binary fields are
		///    present in the json part, but their content is replaced by the md5
		///    checksum of their content. Each object is represented by an item 
		///    present in the object array. The main object represents the package
		///    itself. Each object can contain sub-objects
		///    which are stored in the object array. The recursion level is infinite.
		///    The json data is preceded by the size of the part in bytes, and the :
		///    separator.
		///  - the binary section, containing all binary data as independent parts.
		///    Each part begins with the : separator, the id of the object,
		///    the : separator, the field name, the : separator, its size in
		///    bytes, the : separator, and then the raw binary data
		//////////////////////////////////////////////////////////////////////////
		/// @param lockTime a lock was done with this dump
		/// @return the dump containing all the package content
		std::string InterSYNTHESEPackage::_buildDump(
			const boost::posix_time::ptime& lockTime
		) const	{
			// Variables
			stringstream binaryStream;
			ParametersMap map;

			// Export of the package itself
			map.insert(Key::FIELD.name, getKey());
			map.insert(Name::FIELD.name, get<Name>());
			map.insert(Code::FIELD.name, get<Code>());
			if(!lockTime.is_not_a_date_time())
			{
				map.insert(LockTime::FIELD.name, lockTime);
			}
			map.insert(Public::FIELD.name, get<Public>());

			// Unique objects
			BOOST_FOREACH(const Objects::Type::value_type& item, get<Objects>())
			{
				boost::shared_ptr<ParametersMap> objectPM(new ParametersMap);
				_dumpObject(*item, *objectPM, binaryStream);
				map.insert(TAG_OBJECT, objectPM);
			}

			// Full tables
			Env fullTablesEnv;
			BOOST_FOREACH(const FullTables::Type::value_type& table, get<FullTables>())
			{
				boost::shared_ptr<DBDirectTableSync> directTableSync(
					boost::dynamic_pointer_cast<DBDirectTableSync, DBTableSync>(
						table
				)	);
				if(!directTableSync.get())
				{
					continue;
				}
				DBDirectTableSync::RegistrableSearchResult objects(
					directTableSync->search(
						string(),
						Env::GetOfficialEnv() // TODO replace by fullTableEnv in case of non full synchro table
				)	);
				BOOST_FOREACH(const DBDirectTableSync::RegistrableSearchResult::value_type& item, objects)
				{
					boost::shared_ptr<ParametersMap> objectPM(new ParametersMap);
					_dumpObject(*item, *objectPM, binaryStream);
					map.insert(TAG_OBJECT, objectPM);
				}
			}

			// JSON generation
			stringstream json;
			map.outputJSON(json, string());
			string jsonStr(json.str());

			// Output
			string result(
				lexical_cast<string>(jsonStr.size()) +
				SEPARATOR +
				jsonStr +
				binaryStream.str()
			);
			return result;
		}



		//////////////////////////////////////////////////////////////////////////
		/// Recursive object dump.
		/// @param object the object to dump
		/// @retval pm the parameters map to populate with the object data
		/// @retval binaryStream the stream where the binary data is written
		void InterSYNTHESEPackage::_dumpObject(
			const Registrable& object,
			util::ParametersMap& pm,
			std::stringstream& binaryStream
		){
			// Object non binary fields
			object.toParametersMap(pm, false, false);

			// Object binary fields
			MD5Wrapper md5;
			FilesMap filesMap;
			object.toFilesMap(filesMap);
			BOOST_FOREACH(const FilesMap::Map::value_type& item, filesMap.getMap())
			{
				// Variables
				const string& fieldName(item.first);
				const string& content(item.second.content);

				// Md5 of the content
				pm.insert(
					fieldName,
					md5.getHashFromString(content)
				);

				// Raw binary content
				binaryStream <<SEPARATOR<< object.getKey() <<SEPARATOR<< fieldName <<SEPARATOR<< content.size() <<SEPARATOR<< content;
			}

			// Sub objects
			BOOST_FOREACH(const SubObjects::value_type& it, object.getSubObjects())
			{
				boost::shared_ptr<ParametersMap> subObjectPM(new ParametersMap);
				_dumpObject(*it, *subObjectPM, binaryStream);
				pm.insert(TAG_OBJECT, subObjectPM);
			}
		}



		void InterSYNTHESEPackage::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			InterSYNTHESEModule::AddPackage(*this);
		}



		void InterSYNTHESEPackage::unlink()
		{
			InterSYNTHESEModule::RemovePackage(get<Code>());
		}



		InterSYNTHESEPackage::PackageAddress::PackageAddress(
			const std::string& url
		):	port("80")
		{
			if(!url.empty())
			{
				size_t pos(0);
				for(; pos+1< url.size() && (url[pos] != '/' || url[pos+1] != '/'); ++pos) ;
				if(pos + 1 == url.size())
				{
					throw Exception("Bad URL");
				}
				size_t pos2(pos+2);
				for(; pos2<url.size() && url[pos2]!=':' && url[pos2]!='/'; ++pos2) ;
				host = url.substr(pos+2, pos2 - pos - 2);
				if(pos2 < url.size())
				{
					if(url[pos2] == ':')
					{
						++pos2;
						pos = pos2;
						for(; pos2<url.size() && url[pos2]!='/'; ++pos2) ;
						port = url.substr(pos, pos2-pos);
					}
					smartURL = url.substr(pos2+1);
				}
			}
		}
}	}
