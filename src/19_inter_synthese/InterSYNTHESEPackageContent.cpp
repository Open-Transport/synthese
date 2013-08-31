
/** InterSYNTHESEPackageContent class implementation.
	@file InterSYNTHESEPackageContent.cpp

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

#include "InterSYNTHESEPackageContent.hpp"

#include "DBDirectTableSync.hpp"
#include "DBModule.h"
#include "DBTableSync.hpp"
#include "DBTransaction.hpp"
#include "InterSYNTHESEPackage.hpp"

#include <boost/property_tree/json_parser.hpp>

using namespace boost;
using namespace boost::posix_time;
using namespace boost::property_tree;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace impex;
	using namespace util;

	namespace inter_synthese
	{
		//////////////////////////////////////////////////////////////////////////
		/// Constructor from a serialized string for checkout operation.
		//////////////////////////////////////////////////////////////////////////
		/// @param env the environment to populate when loading objects
		/// @param s the serialized string to parse to populate the object
		/// @param import the import object which supports the checkout operation
		InterSYNTHESEPackageContent::InterSYNTHESEPackageContent(
			util::Env& env,
			const std::string& s,
			impex::Import& import
		):	_env(env),
			_package(new InterSYNTHESEPackage)
		{
			_package->set<Import>(import);
			_parseAndLoad(s);
			_env.add(_package);
		}



		//////////////////////////////////////////////////////////////////////////
		/// Constructor from a serialized string for commit operation.
		//////////////////////////////////////////////////////////////////////////
		/// @param env the environment to populate when loading objects
		/// @param s the serialized string to parse to populate the object
		/// @param package the package which is updated by the commit
		InterSYNTHESEPackageContent::InterSYNTHESEPackageContent(
			Env& env,
			const std::string& s,
			InterSYNTHESEPackage& package
		):	_env(env),
			_package(&package)
		{
			_parseAndLoad(s);
		}



		//////////////////////////////////////////////////////////////////////////
		/// Common part between the two constructors : string parsing and objects
		/// loading.
		//////////////////////////////////////////////////////////////////////////
		/// @param s the serialized string to parse to populate the object
		void InterSYNTHESEPackageContent::_parseAndLoad(
			const string& s
		){
			ContentMap contentMap;

			// Parsing the json
			size_t i(0);
			size_t jsonSize(0);
			for(; i<s.size(); ++i)
			{
				if(s[i] == InterSYNTHESEPackage::SEPARATOR[0])
				{
					jsonSize = lexical_cast<size_t>(s.substr(0, i));
					break;
				}
			}
			istringstream ss(s.substr(i+1, jsonSize));
			read_json(ss, _objects);

			// Getting the content
			for(i = i+2+jsonSize; i<s.size(); ++i)
			{
				size_t j(i);
				for(; j<s.size(); ++j)
				{
					if(s[j] == InterSYNTHESEPackage::SEPARATOR[0])
					{
						break;
					}
				}
				size_t k(j+1);
				for(; k<s.size(); ++k)
				{
					if(s[k] == InterSYNTHESEPackage::SEPARATOR[0])
					{
						break;
					}
				}
				size_t l(k+1);
				for(; l<s.size(); ++l)
				{
					if(s[l] == InterSYNTHESEPackage::SEPARATOR[0])
					{
						break;
					}
				}

				RegistryKeyType objectId(
					lexical_cast<RegistryKeyType>(s.substr(i, j-i))
				);
				string fieldName(
					s.substr(j+1, k-j-1)
				);
				size_t contentSize(
					lexical_cast<size_t>(s.substr(k+1,l-k-1))
				);
				contentMap.insert(
					make_pair(
						make_pair(objectId, fieldName),
						s.substr(l+1, contentSize)
				)	);

				i=l+1+contentSize;
			}

			// The package
			_package->set<Key>(
				_objects.get<RegistryKeyType>(Key::FIELD.name)
			);
			_package->set<Name>(
				_objects.get<string>(Name::FIELD.name)
			);
			_package->set<Code>(
				_objects.get<string>(Code::FIELD.name)
			);
			if(_objects.count(LockTime::FIELD.name))
			{
				_package->set<LockTime>(
					time_from_string(
						_objects.get<string>(LockTime::FIELD.name)
				)	);
				_package->set<LastJSON>(s);
			}
			_package->set<Public>(
				_objects.get<bool>(Public::FIELD.name)
			);
			_package->set<Objects>(
				_loadObjects(_objects, contentMap)
			);
		}



		//////////////////////////////////////////////////////////////////////////
		/// Loads objects contained in the package (objects tag in the JSON).
		//////////////////////////////////////////////////////////////////////////
		/// @param node the JSON node to read
		/// @param contentMap the map containing binary fields
		/// @return the ids of the objects loaded at the next recursion level
		Objects::Type InterSYNTHESEPackageContent::_loadObjects(
			const boost::property_tree::ptree& node,
			const ContentMap& contentMap
		){
			Objects::Type result;

			// Loop on objects
			if(node.count(InterSYNTHESEPackage::TAG_OBJECT))
			{
				BOOST_FOREACH(const ptree::value_type& objectNode, node.get_child(InterSYNTHESEPackage::TAG_OBJECT))
				{
					// The object key
					RegistryKeyType key(objectNode.second.get<RegistryKeyType>(Key::FIELD.name));

					//////////////////////////////////////////////////////////////////////////
					/// Ptree read
					ParametersMap map;
					BOOST_FOREACH(const ptree::value_type& item, objectNode.second)
					{
						// Jump over sub-objects
						if(item.first == InterSYNTHESEPackage::TAG_OBJECT)
						{
							continue;
						}

						// Attempt to read binary content
						ContentMap::const_iterator binaryIt(contentMap.find(make_pair(key, item.first)));
						if(binaryIt == contentMap.end())
						{
							// Store property from the ptree
							map.insert(item.first, item.second.data());
						}
						else
						{
							// Store property from the binary
							map.insert(item.first, binaryIt->second);
						}
					}

					//////////////////////////////////////////////////////////////////////////
					/// Transforming into object
					RegistryTableType classId(decodeTableId(key));
					boost::shared_ptr<DBTableSync> tableSync(DBModule::GetTableSync(classId));
					if(!dynamic_cast<DBDirectTableSync*>(tableSync.get()))
					{
						throw synthese::Exception("Bad table");
					}
					DBDirectTableSync& directTableSync(dynamic_cast<DBDirectTableSync&>(*tableSync));

					// Update or creation
					boost::shared_ptr<Registrable> rObject;
					if(directTableSync.contains(key))
					{
						try
						{
							rObject = directTableSync.getEditableRegistrable(
								key,
								_env
							);
						}
						catch(...) // Avoid break of the package installation because of bad value in existing data
							       // In case of exception, the object will be considered as created
						{
						}
					}
					if(!rObject.get())
					{
						rObject = directTableSync.newObject(map);
						// Should never happen
						if(!rObject.get())
						{
							throw synthese::Exception("Forbidden table");
						}
						rObject->setKey(key);
						const RegistryBase& registry(directTableSync.getRegistry(_env));
						if( !registry.contains(rObject->getKey()))
						{
							_env.addRegistrable(rObject);
						}
					}
					result.push_back(rObject.get());


					//////////////////////////////////////////////////////////////////////////
					// Recursion
					Objects::Type subObjects(
						_loadObjects(objectNode.second, contentMap)
					);

					// Load properties of the current object
					// Placed after the load of the sub objects to prevent bad link
					// in case of link to a sub object
					try
					{
						if(rObject->loadFromRecord(map, _env))
						{
							_objectsToSave.push_back(rObject.get());
						}
					}
					catch(...) // Avoid break of the package installation because of bad value in new data
							   // The object update will be ignored
					{
						
					}

					// Removal detection
					const RegistryBase& registry(directTableSync.getRegistry(Env::GetOfficialEnv()));
					if( registry.contains(rObject->getKey()))
					{
						BOOST_FOREACH(Registrable* subObject, registry.getEditableObject(rObject->getKey())->getSubObjects())
						{
							bool found(false);
							BOOST_FOREACH(const Registrable* object, subObjects)
							{
								if(object->getKey() == subObject->getKey())
								{
									found = true;
									break;
								}
							}
							if(!found)
							{
								_objectsToRemove.insert(subObject->getKey());
							}
						}
					}
			}	}

			return result;
		}



		//////////////////////////////////////////////////////////////////////////
		/// Saves the content of the package through a transaction.
		/// Deletes the removed objects too.
		//////////////////////////////////////////////////////////////////////////
		/// @retval transaction the transaction to populate
		void InterSYNTHESEPackageContent::save( DBTransaction& transaction ) const
		{
			// Deletions
			BOOST_FOREACH(RegistryKeyType id, _objectsToRemove)
			{
				transaction.addDeleteStmt(id);
			}

			// Insertions
			BOOST_FOREACH(const Registrable* object, _objectsToSave)
			{
				DBModule::SaveObject(*object, transaction);
			}
		}
}	}
