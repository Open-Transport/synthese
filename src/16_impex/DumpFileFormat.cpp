
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

#include "DumpFileFormat.hpp"

#include "AdminFunctionRequest.hpp"
#include "DataSource.h"
#include "DataSourceAdmin.h"
#include "DBDirectTableSync.hpp"
#include "DBModule.h"
#include "ObjectBase.hpp"
#include "PropertiesHTMLTable.h"

#include <fstream>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::filesystem;
using namespace boost::posix_time;
using namespace boost::algorithm;

namespace synthese
{
	using namespace util;
	using namespace impex;
	using namespace db;
	using namespace server;
	using namespace admin;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat, impex::DumpFileFormat>::FACTORY_KEY("Dump");
	}

	namespace impex
	{
		const string DumpFileFormat::Exporter_::PARAMETER_OBJECT_ID = "object_id";
		const string DumpFileFormat::Exporter_::PARAMETER_PATH = "path";




		DumpFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::DataSource& dataSource
		):	OneFileTypeImporter<Importer_>(env, dataSource),
			Importer(env, dataSource)
		{
		}



		bool DumpFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			std::ostream& os,
			boost::optional<const admin::AdminRequest&> adminRequest
		) const	{

			_readFileSystem(filePath);
			return true;
		}



		ObjectBase* DumpFileFormat::Importer_::_readFileSystem(
			const boost::filesystem::path filePath
		) const	{

			// File id
			RegistryKeyType key(
				lexical_cast<RegistryKeyType>(filePath.filename().substr(0, filePath.filename().size()-5))
			);

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

			// Data
			size_t fieldRank(0);
			bool firstLineOfRecord(true);
			stringstream fileContent;
			while(getline(inFile, line))
			{
				if(	line.size() > 1 &&
					line.substr(line.size()-2,1) == "\"" &&
					(line.size() == 2 || line.substr(line.size()-3,1) != "\\")
				){
					if(firstLineOfRecord)
					{
						fileContent << line.substr(1, line.size()-3);
					}
					else
					{
						fileContent << line.substr(0, line.size()-2);
					}
					map.insert(fieldNames[fieldRank], fileContent.str());
					++fieldRank;
					if(fieldRank > fieldNames.size())
					{
						break;
					}
					firstLineOfRecord = true;
					fileContent.clear();
				}
				else
				{
					fileContent << line.substr(firstLineOfRecord ? 1 : 0) << endl;
				}
			}

			//////////////////////////////////////////////////////////////////////////
			// Attachments and sub objects
			SubObjects subObjects;

			path directory(filePath.parent_path());
			for(directory_iterator it(directory); it != directory_iterator(); ++it)
			{
				path attFilePath(*it);
				string fileName(attFilePath.filename());

				// Sub-objects
				if(	is_directory(filePath) &&
					fileName == lexical_cast<string>(key)
				){
					// Dumps
					for(directory_iterator it2(attFilePath); it2 != directory_iterator(); ++it2)
					{
						path subFilePath(*it2);
						if(	is_directory(subFilePath) ||
							subFilePath.filename().substr(subFilePath.filename().size() - 5) != ".dump"
						){
							continue;
						}

						subObjects.push_back(_readFileSystem(subFilePath));
					}
				}
				else if(
					!is_directory(attFilePath) &&
					fileName.size() >= filePath.filename().size()-3 &&
					fileName.substr(0, lexical_cast<string>(key).size()+1) == lexical_cast<string>(key) +"_" &&
					fileName.substr(attFilePath.filename().size() - 5) != ".dump" &&
					fileName.substr(attFilePath.filename().size() - 5) != ".name"
				){
					// Field id
					vector<string> parts;
					split(parts, fileName, is_any_of("_."), token_compress_on);

					// File content
					stringstream fileContent;
					ifstream inFile;
					inFile.open(filePath.file_string().c_str());
					string line;
					while(getline(inFile, line))
					{
						fileContent << line;
					}
					map.insert(
						filePath.filename().substr(
							parts[0].size()+1,
							filePath.filename().size() - parts[0].size() - parts[parts.size()-1].size() - 2
						),
						fileContent.str()
					);
				}
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
			shared_ptr<Registrable> rObject(
				directTableSync.getEditableRegistrable(
					key,
					_env,
					UP_LINKS_LOAD_LEVEL,
					AUTO_CREATE
			)	);
			shared_ptr<ObjectBase> object(dynamic_pointer_cast<ObjectBase, Registrable>(rObject));
			object->loadFromRecord(map, _env);
			return object.get();
		}



		db::DBTransaction DumpFileFormat::Importer_::_save() const
		{
			DBTransaction t;
			DBModule::SaveEntireEnv(_env, t);
			return t;
		}



		void DumpFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			impex::OneFileTypeImporter<Importer_>::_setFromParametersMap(map);
		}



		util::ParametersMap DumpFileFormat::Importer_::_getParametersMap() const
		{
			return ParametersMap();
		}



		void DumpFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const admin::AdminRequest& request
		) const	{

			stream << "<h1>Fichiers</h1>";

			AdminFunctionRequest<DataSourceAdmin> reloadRequest(request);
			PropertiesHTMLTable t(reloadRequest.getHTMLForm());
			stream << t.open();
			stream << t.title("Mode");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.title("Fichiers");
			stream << t.cell("Fichier dump", t.getForm().getTextInput(PARAMETER_PATH, _pathsSet.empty() ? string() : _pathsSet.begin()->file_string()));
			stream << t.close();
		}



		void DumpFileFormat::Exporter_::setFromParametersMap( const util::ParametersMap& map )
		{
			// The object to dump
			RegistryKeyType objectId(map.get<RegistryKeyType>(PARAMETER_OBJECT_ID));
			shared_ptr<DBTableSync> table(DBModule::GetTableSync(decodeTableId(objectId)));
			DBDirectTableSync* dTable(dynamic_cast<DBDirectTableSync*>(table.get()));
			if(!dTable)
			{
				throw Exception("Table must identify its record by id");
			}
			Env env;
			_object = dynamic_pointer_cast<const ObjectBase, const Registrable>(dTable->getRegistrable(objectId, env));
			if(!_object.get())
			{
				throw Exception("Table objects must be dumpable");
			}

			// The path
			_path = path(map.get<string>(PARAMETER_PATH));
		}



		util::ParametersMap DumpFileFormat::Exporter_::getParametersMap() const
		{
			ParametersMap map;
			if(_object.get())
			{
				map.insert(PARAMETER_OBJECT_ID, _object->getKey());
			}
			map.insert(PARAMETER_PATH, _path.file_string());
			return map;
		}



		void DumpFileFormat::Exporter_::_dumpToFileSystem(
			const ObjectBase& object,
			const boost::filesystem::path directory
		) const	{

			RegistryKeyType key(object.getKey());


			//////////////////////////////////////////////////////////////////////////
			// Files

			ParametersMap filesMap;
			object.toParametersMap(filesMap, true);
			BOOST_FOREACH(const ParametersMap::Map::value_type& item, filesMap.getMap())
			{
				path filePath(directory / path(lexical_cast<string>(key) + "_" + item.first));
				ofstream fileStream(filePath.file_string().c_str());
				fileStream << item.second;
				fileStream.close();
			}


			//////////////////////////////////////////////////////////////////////////
			// Attributes

			ParametersMap attributesMap;
			object.toParametersMap(attributesMap, false);
			path dumpFilePath(directory / path(lexical_cast<string>(key) + ".dump"));
			ofstream dumpStream(dumpFilePath.file_string().c_str());

			// Field names
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
			dumpStream << ") VALUES (" << endl;

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
			dumpStream << ")" << endl << ";" << endl;
			dumpStream.close();


			//////////////////////////////////////////////////////////////////////////
			// Name file
			if(!object.getName().empty())
			{
				path namePath(directory / path(lexical_cast<string>(key) + "_" + object.getName() + ".name"));
				ofstream nameStream(namePath.file_string().c_str());
				nameStream << endl;
				nameStream.close();
			}


			//////////////////////////////////////////////////////////////////////////
			// Sub objects
			SubObjects subObjects(object.getSubObjects());
			if(!subObjects.empty())
			{
				path dirPath(directory / lexical_cast<string>(key));
				create_directory(dirPath);

				BOOST_FOREACH(const SubObjects::value_type& subObject, subObjects)
				{
					_dumpToFileSystem(*subObject, dirPath);
				}
			}
		}



		void synthese::impex::DumpFileFormat::Exporter_::build(
			std::ostream& os
		) const	{

			_dumpToFileSystem(*_object, _path);

		}
}	}
