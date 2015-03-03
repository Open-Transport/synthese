
/** SVNWorkingCopy class implementation.
	@file SVNWorkingCopy.cpp

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

#include "SVNWorkingCopy.hpp"

#include "DBDirectTableSync.hpp"
#include "DBException.hpp"
#include "DBModule.h"
#include "DBTransaction.hpp"
#include "ObjectBase.hpp"
#include "PropertiesHTMLTable.h"
#include "SchemaMacros.hpp"
#include "SVNCommands.hpp"
#include "SVNModule.hpp"

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::filesystem;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace db;
	using namespace db::svn;
	using namespace server;
	


	FIELD_DEFINITION_OF_TYPE(db::svn::SVNWorkingCopy, "svn_url", SQL_TEXT)



	namespace db
	{
		namespace svn
		{
			using namespace SVNCommands;



			SVNWorkingCopy::SVNWorkingCopy(
			):	_object(NULL)
			{}



			SVNWorkingCopy::ImportedObjects SVNWorkingCopy::_import(
				const boost::filesystem::path& dirPath,
				db::DBTransaction& transaction,
				bool first
			){

				ImportedObjects result;

				// Loop on .dump files
				for(directory_iterator it(dirPath); it != directory_iterator(); ++it)
				{
					// Local variables
					path filePath(*it);
					string fileName(filePath.filename().string());

					// Jump over ignored files
					if(		is_directory(filePath) ||
							fileName.size() < 10 ||
							fileName.substr(fileName.size() - 5) != ".dump"
					){
						continue;
					}

					// File id
					string idStr(fileName.substr(0, fileName.size()-5));
					RegistryKeyType key(
						lexical_cast<RegistryKeyType>(idStr)
					);
					result.insert(key);


					//////////////////////////////////////////////////////////////////////////
					// Dump file parsing

					ifstream inFile;
					inFile.open(filePath.string().c_str());
					string line;
					vector<string> fieldNames;
					ParametersMap map;

					// Field names
					if(getline(inFile, line))
					{
						vector<string> parts;
						split(parts, line, is_any_of("(,)"), token_compress_on);

						for(size_t i(1); i<parts.size()-1; ++i)
						{
							fieldNames.push_back(parts[i]);
						}
					}

					// File read
					stringstream fileContent;
					while(getline(inFile, line))
					{
						fileContent << line;
					}
					string fileContentStr(fileContent.str());

					// Parsing
					bool inValue(false);
					bool escape(false);
					size_t fieldRank(0);
					string curValue;
					for(size_t p(0); p < fileContentStr.size(); ++p)
					{
						if(!inValue)
						{
							if(fileContentStr[p] == '"')
							{
								inValue = true;
								curValue.clear();
							}
						}
						else if(!escape)
						{
							if(fileContentStr[p] == '"')
							{
								inValue = false;
								map.insert(fieldNames[fieldRank], curValue);
								++fieldRank;
							}
							else if(fileContentStr[p] == '\\')
							{
								escape = true;
							}
							else
							{
								curValue.push_back(fileContentStr[p]);
							}
						}
						else
						{
							curValue.push_back(fileContentStr[p]);
							escape = false;
						}
					}

					//////////////////////////////////////////////////////////////////////////
					// Attachments

					for(directory_iterator it2(dirPath); it2 != directory_iterator(); ++it2)
					{
						// Local variables
						path filePath2(*it2);
						string fileName2(filePath2.filename().string());

						// Jump over ignored files
						if(	is_directory(filePath2) ||
							fileName2.size() < idStr.size()+2 ||
							fileName2.substr(0, idStr.size()) != idStr ||
							fileName2.substr(idStr.size(), 1) != "_" ||
							fileName2.substr(fileName2.size() - 5) == ".name"
						){
							continue;
						}

						path filePath3(filePath2);
						string fieldName(filePath3.replace_extension().filename().string().substr(idStr.size()+1));

						// File content
						string fieldContent;
						ifstream in(filePath2.string().c_str(), ios::in | ios::binary);
						char ch;
						while(in)
						{
							in.get(ch);
							if(!in)
							{
								break;
							}
							fieldContent.push_back(ch);
						}
						in.close();
						map.insert(
							fieldName,
							fieldContent
						);
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
						rObject = directTableSync.getEditableRegistrable(
							key,
							_env
						);
					}
					else
					{
						rObject = directTableSync.newObject();
						// Forbidden for inheritance classes
						if(!rObject.get())
						{
							throw synthese::Exception("Forbidden table");
						}
						rObject->setKey(key);
						_env.addRegistrable(rObject);
					}
					boost::shared_ptr<ObjectBase> object(dynamic_pointer_cast<ObjectBase, Registrable>(rObject));


					//////////////////////////////////////////////////////////////////////////
					// Sub objects

					ImportedObjects subObjects;

					path subDirPath(dirPath / idStr);
					if(	is_directory(subDirPath)
					){
						subObjects = _import(subDirPath, transaction);
					}

					// Load properties of the current object
					// Placed after the load of the sub objects to prevent bad link
					// in case of link to a sub object
					object->loadFromRecord(map, _env);
					directTableSync.saveRegistrable(*rObject, transaction);

					// Deletions
					const RegistryBase& registry(directTableSync.getRegistry(Env::GetOfficialEnv()));
					if( registry.contains(object->getKey()))
					{
						boost::shared_ptr<ObjectBase> mObject(
							dynamic_pointer_cast<ObjectBase, Registrable>(
								registry.getEditableObject(object->getKey())
						)	);
						if(mObject.get())
						{
							BOOST_FOREACH(Registrable* subObject, mObject->getSubObjects())
							{
								if(subObjects.find(subObject->getKey()) == subObjects.end())
								{
									RegistryTableType classId(decodeTableId(subObject->getKey()));
									boost::shared_ptr<DBTableSync> tableSync(DBModule::GetTableSync(classId));
									tableSync->deleteRecord(NULL, subObject->getKey(), transaction, false);
								}
							}
						}
					}

					// Save the object as the root
					if(first && !_object)
					{
						_object = object.get();
					}
				}

				return result;
			}



			void SVNWorkingCopy::_importWC(
				bool save
			){

				DBTransaction transaction;

				_import(
					_path,
					transaction,
					true
				);

				//////////////////////////////////////////////////////////////////////////
				// Saving
				if(save)
				{
					transaction.run();
				}
			}



			void SVNWorkingCopy::_export(
				const Registrable& object,
				const boost::filesystem::path& dirPath,
				const bool noCommit
			) const	{

				RegistryKeyType key(object.getKey());

				//////////////////////////////////////////////////////////////////////////
				// Attributes

				// File creation or update ?
				path dumpFilePath(dirPath / path(lexical_cast<string>(key) + ".dump"));
				bool creation(!filesystem::exists(dumpFilePath));

				// Field names
				ParametersMap attributesMap;
				object.toParametersMap(attributesMap, false, false);
				ofstream dumpStream(
					dumpFilePath.string().c_str(),
					std::ios_base::out | std::ios_base::binary
				);
				dumpStream << object.getTableName() << "(";
				bool first(true);
				BOOST_FOREACH(const ParametersMap::Map::value_type& item, attributesMap.getMap())
				{
					if(first)
					{
						first = false;
					}
					else
					{
						dumpStream << ",";
					}
					dumpStream << item.first;
				}
				dumpStream << ")" << endl;

				// Values
				first = true;
				BOOST_FOREACH(const ParametersMap::Map::value_type& item, attributesMap.getMap())
				{
					string escapedValue;
					escapedValue = item.second;
					replace_all(escapedValue, "\\", "\\\\");
					replace_all(escapedValue, "\"", "\\\"");

					if(first)
					{
						first = false;
					}
					else
					{
						dumpStream << "," << endl;
					}
					dumpStream << "\"" << escapedValue << "\"";
				}
				dumpStream << endl;
				dumpStream.close();

				// If creation, svn add
				if(creation && !noCommit)
				{
					_svnAdd(dumpFilePath);
				}

				//////////////////////////////////////////////////////////////////////////
				// Files
				FilesMap filesMap;
				object.toFilesMap(filesMap);
				BOOST_FOREACH(const FilesMap::Map::value_type& item, filesMap.getMap())
				{
					// Creation or update or move
					bool creation(true);
					string fileNameMainPart(lexical_cast<string>(key) + "_" + item.first);
					string fileNameWithExtension(fileNameMainPart + "." + item.second.mimeType.getDefaultExtension());
					path filePath(dirPath / path(fileNameWithExtension));
					for(directory_iterator it(dirPath); it != directory_iterator(); ++it)
					{
						if(	!is_directory(it->path()) &&
							it->path().filename().string().size() >= fileNameMainPart.size() &&
							it->path().filename().string().substr(0, fileNameMainPart.size()) == fileNameMainPart
						){
							creation = false;
							if(it->path().filename().string() != fileNameWithExtension && !noCommit)
							{
								_svnMove(it->path(), filePath);
							}
							break;
						}
					}

					// Dump value into file
					ofstream fileStream(filePath.string().c_str(), std::ios_base::out | std::ios_base::binary);
					fileStream << item.second.content;
					fileStream.close();

					// If creation, svn add
					if(creation && !noCommit)
					{
						_svnAdd(filePath);
					}
				}


				//////////////////////////////////////////////////////////////////////////
				// Name file
				if(!object.getName().empty())
				{
					path namePath(dirPath / path(lexical_cast<string>(key) + "_" + object.getName() + ".name"));
					string commonFileName(lexical_cast<string>(key) + "_");
					string nameExtension(".name");

					// Search for an existing file
					bool creation(true);
					for(directory_iterator it(dirPath); it != directory_iterator(); ++it)
					{
						// Jump over directories
						if(is_directory(it->path()))
						{
							continue;
						}

						// Existing name ?
						if(namePath == it->path())
						{
							creation = false;
							break;
						}

						// Search for preceding name
						string fileName(it->path().filename().string());
						if(	(fileName.size() >= commonFileName.size() + nameExtension.size()) &&
							fileName.substr(0, commonFileName.size()) == commonFileName &&
							fileName.substr(fileName.size() - nameExtension.size()) == nameExtension &&
							!noCommit
						){
							_svnMove(it->path(), namePath);
							break;
						}
					}

					// Creation if the fake file if necessary
					if(creation)
					{
						ofstream nameStream(
							namePath.string().c_str(),
							std::ios_base::out | std::ios_base::binary
						);
						nameStream << endl;
						nameStream.close();
						if (!noCommit)
						{
							_svnAdd(namePath);
						}
					}
				}


				//////////////////////////////////////////////////////////////////////////
				// Sub objects
				SubObjects subObjects(object.getSubObjects());
				path subdirPath(dirPath / lexical_cast<string>(key));

				// No sub object : delete subdirectory if exists
				if(subObjects.empty())
				{
					if(exists(subdirPath))
					{
						_svnDelete(subdirPath);
					}
				}
				else // Sub objects present
				{
					// Existing file with the same name than the directory (should never happen)
					if(exists(subdirPath) && !is_directory(subdirPath))
					{
						_svnDelete(subdirPath);
					}

					// If no existing file, add a new directory
					if(!exists(subdirPath))
					{
						create_directory(subdirPath);
						_svnAdd(subdirPath);
					}

					// Loop on objects to export
					set<RegistryKeyType> existingIds;
					BOOST_FOREACH(const SubObjects::value_type& subObject, subObjects)
					{
						// Register the id
						existingIds.insert(subObject->getKey());

						// Export of the object (recursion)
						_export(*subObject, subdirPath, noCommit);
					}

					// Loop on existing files in the path and remove files that correspond
					// to non existent objects
					vector<path> filesToRemove;
					for(directory_iterator it(subdirPath); it != directory_iterator(); ++it)
					{
						string fileName(it->path().filename().string());

						// Id part of the path
						vector<string> nameParts;
						split(nameParts, fileName, is_any_of("_."));
						if(nameParts.empty())
						{
							continue;
						}
						try
						{
							// Check if the file corresponds to an existent record
							RegistryKeyType id(lexical_cast<RegistryKeyType>(nameParts[0]));
							if(existingIds.find(id) == existingIds.end())
							{
								filesToRemove.push_back(it->path());
							}
						}
						catch(bad_lexical_cast)
						{
						}
					}

					// Remove files corresponding to the deleted objects
					BOOST_FOREACH(const path& file, filesToRemove)
					{
						_svnDelete(file);
					}
				}
			}



			void SVNWorkingCopy::_exportToWC(const bool noCommit) const
			{
				// Check if the object is defined
				if(!_object)
				{
					return;
				}

				_export(*_object, _path, noCommit);
			}



			void SVNWorkingCopy::create(
				const std::string& user,
				const std::string& password,
				const bool noCommit
			) const	{
				if (!noCommit)
				{
					_repo.mkdir(user, password);
					_repo.checkout(user, password, _path);
				}
				else
				{
					// Creation of the local repository
					create_directory(_path);
				}
				_exportToWC(noCommit);
				if (!noCommit)
				{
					_svnCommit("Object creation", user, password, _path);
				}
			}



			//////////////////////////////////////////////////////////////////////////
			/// Performs a commit into the server.
			/// @param message the message to attach to the commit
			/// @param user valid login on the server
			/// @param password password corresponding to the login
			/// @param noCommit to avoid commit (will only save to the working copy)
			void SVNWorkingCopy::commit(
				const std::string& message,
				const std::string& user,
				const std::string& password,
				const bool noCommit,
				const bool noUpdate
			){
				update(user, password, noUpdate, false /* Save to WC */);
				if(!noCommit)
				{
					_svnCommit(message, user, password,	_path);
				}
			}



			void SVNWorkingCopy::checkout(
				const std::string& user,
				const std::string& password
			){
				// Identification of the object id
				RegistryKeyType id(0);
				LsResult files(_repo.ls(user, password));
				BOOST_FOREACH(const string& file, files)
				{
					if(	file.size() > 10 &&
						file.substr(file.size() - 5) == ".dump"
					){
						id = lexical_cast<RegistryKeyType>(file.substr(0, file.size() - 5));
						break;
					}
				}
				if(!id)
				{
					throw synthese::Exception("Empty repository");
				}

				// Determination of the working copy path
				_path = SVNModule::GetSVNWCRootPath() / lexical_cast<string>(id);

				// Check the precondition
				if(exists(_path))
				{
					throw synthese::Exception("The local path already exists");
				}

				_repo.checkout(user, password, _path);
				_importWC(true);
			}



			void SVNWorkingCopy::update(
				const std::string& user,
				const std::string& password,
				bool noUpdate,
				bool noWCSave
			){
				if(exists(_path))
				{
					if(!noWCSave)
					{
						_exportToWC(false);
					}
					if(!noUpdate)
					{
						_svnUpdate(user, password, _path);
					}
				}
				else
				{
					_repo.checkout(user, password, _path);
				}
				_importWC(true);
			}



			void SVNWorkingCopy::setObject( ObjectBase* value )
			{
				_object = value;
				if(_object)
				{
					string keyStr(lexical_cast<string>(_object->getKey()));
					_path = SVNModule::GetSVNWCRootPath() / keyStr;
				}
			}



			bool SVNWorkingCopy::from_string( const std::string& text )
			{
				if(getRepo().getURL() == text)
				{
					return false;
				}
				else
				{
					setRepo(SVNRepository(text));
					return true;
				}
			}



			bool SVNWorkingCopy::LoadFromRecord(
				Type& fieldObject,
				ObjectBase& object,
				const Record& record,
				const util::Env& env
			){
				return SimpleObjectFieldDefinition<SVNWorkingCopy>::_UpdateFromString(
					fieldObject,
					record,
					&SVNWorkingCopy::from_string
				);
			}



			std::string SVNWorkingCopy::to_string() const
			{
				return getRepo().getURL();
			}



			void SVNWorkingCopy::SaveToFilesMap(
				const SVNWorkingCopy& fieldObject,
				const ObjectBase& object,
				FilesMap& map
			){
				SimpleObjectFieldDefinition<SVNWorkingCopy>::_SaveToFilesMap(
					fieldObject,
					map,
					&SVNWorkingCopy::to_string
				);
			}



			void SVNWorkingCopy::SaveToParametersMap(
				const Type& fieldObject,
				const ObjectBase& object,
				util::ParametersMap& map,
				const std::string& prefix,
				boost::logic::tribool withFiles
			){
				SimpleObjectFieldDefinition<SVNWorkingCopy>::_SaveToParametersMap(
					fieldObject,
					map,
					prefix,
					withFiles,
					&SVNWorkingCopy::to_string
				);
			}



			void SVNWorkingCopy::SaveToDBContent(
				const Type& fieldObject,
				const ObjectBase& object,
				DBContent& content
			){
				boost::optional<std::string> text(
					fieldObject.getRepo().getURL()
				);
				content.push_back(Cell(text));
			}



			void SVNWorkingCopy::GetLinkedObjectsIds( LinkedObjectsIds& list, const Record& record )
			{

			}
}	}	}

