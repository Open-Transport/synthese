
/** DumpFileFormat class implementation.
	@file DumpFileFormat.cpp

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
#include "DBModule.h"
#include "DBTransaction.hpp"
#include "ObjectBase.hpp"
#include "PropertiesHTMLTable.h"
#include "SVNModule.hpp"

#include <stdio.h>
#ifdef WIN32
#define popen _popen
#define pclose _pclose
#endif
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

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



	template<>
	void ObjectField<SVNWorkingCopy, SVNWorkingCopy>::UnSerialize(
		SVNWorkingCopy& fieldObject,
		const string& text,
		const Env& env
	){
		fieldObject.setRepoURL(text);
	}



	template<>
	string ObjectField<SVNWorkingCopy, SVNWorkingCopy>::Serialize(
		const SVNWorkingCopy& fieldObject,
		util::ParametersMap::SerializationFormat format
	){
		stringstream str;
		if(format == ParametersMap::FORMAT_SQL)
		{
			str << "\"";
		}
		str << fieldObject.getRepoURL();
		if(format == ParametersMap::FORMAT_SQL)
		{
			str << "\"";
		}

		return str.str();
	}



	template<>
	void ObjectField<SVNWorkingCopy, SVNWorkingCopy>::GetLinkedObjectsIdsFromText(
		LinkedObjectsIds& list,
		const std::string& text
	){
	}



	namespace db
	{
		namespace svn
		{
			SVNWorkingCopy::SVNWorkingCopy(
				ObjectBase* object
			):	_object(object),
				_path(object ? SVNModule::GetSVNWCRootPath() / lexical_cast<string>(object->getKey()) : path())
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
					string fileName(filePath.filename());

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
					inFile.open(filePath.file_string().c_str());
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
						string fileName2(filePath2.filename());

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
						string fieldName(filePath3.replace_extension().filename().substr(idStr.size()+1));

						// File content
						string fieldContent;
						ifstream in(filePath2.file_string().c_str(), ios::in | ios::binary);
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
					shared_ptr<DBTableSync> tableSync(DBModule::GetTableSync(classId));
					if(!dynamic_cast<DBDirectTableSync*>(tableSync.get()))
					{
						throw Exception("Bad table");
					}
					DBDirectTableSync& directTableSync(dynamic_cast<DBDirectTableSync&>(*tableSync));

					// Update or creation
					shared_ptr<Registrable> rObject;
					rObject = directTableSync.getEditableRegistrable(
						key,
						_env,
						UP_LINKS_LOAD_LEVEL,
						AUTO_CREATE
					);
					shared_ptr<ObjectBase> object(dynamic_pointer_cast<ObjectBase, Registrable>(rObject));
					object->loadFromRecord(map, _env);
					directTableSync.saveRegistrable(*rObject, transaction);


					//////////////////////////////////////////////////////////////////////////
					// Sub objects

					ImportedObjects subObjects;

					path subDirPath(dirPath / idStr);
					if(	is_directory(subDirPath)
					){
						subObjects = _import(subDirPath, transaction);
					}

					// Deletions
					const RegistryBase& registry(directTableSync.getRegistry(Env::GetOfficialEnv()));
					if( registry.contains(object->getKey()))
					{
						shared_ptr<ObjectBase> mObject(
							dynamic_pointer_cast<ObjectBase, Registrable>(
								registry.getEditableObject(object->getKey())
						)	);
						if(mObject.get())
						{
							BOOST_FOREACH(ObjectBase* subObject, mObject->getSubObjects())
							{
								if(subObjects.find(subObject->getKey()) == subObjects.end())
								{
									RegistryTableType classId(decodeTableId(subObject->getKey()));
									shared_ptr<DBTableSync> tableSync(DBModule::GetTableSync(classId));
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
				/// Saving
				if(save)
				{
					transaction.run();
				}
			}



			void SVNWorkingCopy::_export(
				const ObjectBase& object,
				const boost::filesystem::path& dirPath
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
				ofstream dumpStream(dumpFilePath.file_string().c_str());
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
				if(creation)
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
							it->path().filename().size() >= fileNameMainPart.size() &&
							it->path().filename().substr(0, fileNameMainPart.size()) == fileNameMainPart
						){
							creation = false;
							if(it->path().filename() != fileNameWithExtension)
							{
								_svnMove(it->path(), filePath);
							}
							break;
						}
					}

					// Dump value into file
					ofstream fileStream(filePath.file_string().c_str());
					fileStream << item.second.content;
					fileStream.close();

					// If creation, svn add
					if(creation)
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
						string fileName(it->path().filename());
						if(	(fileName.size() >= commonFileName.size() + nameExtension.size()) &&
							fileName.substr(0, commonFileName.size()) == commonFileName &&
							fileName.substr(fileName.size() - nameExtension.size()) == nameExtension
						){
							_svnMove(it->path(), namePath);
							break;
						}
					}

					// Creation if the fake file if necessary
					if(creation)
					{
						ofstream nameStream(namePath.file_string().c_str());
						nameStream << endl;
						nameStream.close();
						_svnAdd(namePath);
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
						_export(*subObject, subdirPath);
					}

					// Loop on existing files in the path and remove files that correspond
					// to non existent objects
					vector<path> filesToRemove;
					for(directory_iterator it(subdirPath); it != directory_iterator(); ++it)
					{
						string fileName(it->path().filename());

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



			void SVNWorkingCopy::_exportToWC() const
			{
				// Check if the object is defined
				if(!_object)
				{
					return;
				}

				_export(*_object, _path);
			}



			bool SVNWorkingCopy::_svnUpdate(
				const std::string& user,
				const std::string& password,
				const boost::filesystem::path& localPath
			){
				_runSVN(
					user,
					password,
					"up",
					"\"" + localPath.file_string() + "\""
				);

				return true;
			}



			bool SVNWorkingCopy::_svnCommit(
				const std::string& message,
				const std::string& user,
				const std::string& password,
				const boost::filesystem::path& localPath
			){
				CommandOutput result(
					_runSVN(
						user,
						password,
						"ci",
						" -m \""+ message +"\" \""+ localPath.file_string() + "\""
				)	);
				
				return true;
			}



			void SVNWorkingCopy::_svnCheckout(
				const std::string& user,
				const std::string& password,
				const std::string& url,
				const boost::filesystem::path& localPath
			){
				CommandOutput result(
					_runSVN(
						user,
						password,
						"co",
						url + " \"" + localPath.file_string() + "\""
				)	);
				if(result.first)
				{
					throw CommandException(result);
				}
			}



			SVNWorkingCopy::CommandOutput SVNWorkingCopy::_runSVN(
				const std::string& user,
				const std::string& password,
				const std::string& command,
				const std::string& parameters
			){
				CommandOutput result(make_pair(1, string()));
				stringstream cmd;
				cmd << "svn " << command;
				cmd << " --no-auth-cache";
				cmd << " --non-interactive";
				if(!user.empty())
				{
					cmd << " --username " << user;
				}
				if(!password.empty())
				{
					cmd << " --password " << password;
				}
				cmd << " " << parameters << " 2>&1";

				stringstream response;
				FILE* pipe = popen(cmd.str().c_str(), "r");
				if(!pipe)
				{
					return result;
				}
				char buffer[128];
				while(!feof(pipe))
				{
					if(fgets(buffer, 128, pipe) != NULL)
					{
						response << buffer;
					}
				}
				result.first = pclose(pipe);
				result.second = response.str();

				return result;
			}



			void SVNWorkingCopy::_svnAdd(
				const boost::filesystem::path& file
			){

				_runSVN(
					string(),
					string(),
					"add",
					"\"" + file.file_string() + "\""
				);
			}



			void SVNWorkingCopy::_svnDelete(
				const boost::filesystem::path& file
			){
				_runSVN(
					string(),
					string(),
					"delete",
					"--force \""+ file.file_string() + "\""
				);
			}



			void SVNWorkingCopy::_svnMove(
				const boost::filesystem::path& oldFile,
				const boost::filesystem::path& newFile
			){
				_runSVN(
					string(),
					string(),
					"move",
					"\"" + oldFile.file_string() +"\" \""+ newFile.file_string() + "\""
				);
			}



			void SVNWorkingCopy::_svnMkdir(
				const std::string& user,
				const std::string& password,
				const std::string& url
			){
				CommandOutput result(
					_runSVN(
						user,
						password,
						"mkdir",
						"-m \"Object creation\" "+ url
				)	);
				if(result.first)
				{
					throw CommandException(result);
				}
			}



			void SVNWorkingCopy::create(
				const std::string& user,
				const std::string& password
			) const	{
				_svnMkdir(user, password, _repoURL);
				_svnCheckout(user, password, _repoURL, _path);
				_exportToWC();
				_svnCommit("Object creation", user, password, _path);
			}



			void SVNWorkingCopy::commit(
				const std::string& message,
				const std::string& user,
				const std::string& password
			){
				update(user, password);
				_svnCommit(message, user, password,	_path);
			}



			void SVNWorkingCopy::checkout(
				const std::string& user,
				const std::string& password
			){
				// Identification of the object id
				RegistryKeyType id(0);
				LsResult files(_svnLs(user, password, _repoURL));
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
					throw Exception("Empty repository");
				}

				// Determination of the working copy path
				_path = SVNModule::GetSVNWCRootPath() / lexical_cast<string>(id);

				// Check the precondition
				if(exists(_path))
				{
					throw Exception("The local path already exists");
				}

				_svnCheckout(user, password, _repoURL, _path);
				_importWC(true);
			}



			void SVNWorkingCopy::update(
				const std::string& user,
				const std::string& password
			){
				_exportToWC();
				_svnUpdate(user, password, _path);
				_importWC(true);
			}



			SVNWorkingCopy::LsResult SVNWorkingCopy::_svnLs(
				const std::string& user,
				const std::string& password,
				const std::string& url
			){
				CommandOutput ls(_runSVN(user, password, "ls", url));
				if(ls.first)
				{
					throw Exception("Bad URL");
				}
				LsResult files;
				split(files, ls.second, is_any_of("\n\r"), token_compress_on);
				return files;
			}
}	}	}
