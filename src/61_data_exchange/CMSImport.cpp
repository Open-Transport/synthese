
//////////////////////////////////////////////////////////////////////////
/// CMSImport class implementation.
/// @file CMSImport.cpp
/// @author Bruno Coudoin
/// @date 2014
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2014 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "CMSImport.hpp"

#include "CMSModule.hpp"
#include "MimeType.hpp"
#include "MimeTypes.hpp"
#include "ParametersMap.h"
#include "WebpageContent.hpp"
#include "WebPageTableSync.h"
#include "WebsiteTableSync.hpp"

#include <fstream>
#include <streambuf>
#include <stdio.h>

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::filesystem;

namespace synthese
{
	using namespace data_exchange;
	using namespace cms;
	using namespace impex;
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat, CMSImport>::FACTORY_KEY("CMS Import");
	}

	namespace data_exchange
	{
		const string CMSImport::Importer_::PARAMETER_DIRECTORY = "directory";
		const string CMSImport::Importer_::PARAMETER_PARENT_ID = "parent_id";
		const string CMSImport::Importer_::PARAMETER_IMPORT_SITE_ID = "import_site_id";
		const string CMSImport::Importer_::PARAMETER_MAX_AGE = "import_max_age"; // In seconds
		const string CMSImport::Importer_::PARAMETER_EXCLUDE_LIST = "exclude_list";

		const bool CMSImport::Importer_::IMPORTABLE(true);

		bool CMSImport::Importer_::parseFiles() const	{
			_site->set<Name>(_directory);
			// Take the last directory as the ClientURL
			// The -2 avoids to count a trailing '/' in consideration
			string::size_type found =
				_directory.find_last_of("/",
									   _directory.length() - 2);
			if(found != string::npos)
			{
				_site->set<ClientURL>(_directory.substr(found));
			}

			_importDir( path(_directory), _parent.get(), path(_directory));
			return true;
		}

		bool CMSImport::Importer_::_isExcluded(
			const vector<boost::regex> &regexps,
			const std::string &fileName
		) const
		{
			BOOST_FOREACH(const boost::regex &oneRegexp, regexps)
			{
				if(regex_match(fileName, oneRegexp))
				{
						return true;
				}
			}
			return false;
		}

		/// Import recursively the files in directory @currentDir under
		/// the page @parent
		void CMSImport::Importer_::_importDir(const path &directoryPath,
			cms::Webpage *parent,
			path currentDir) const
		{
			size_t rank(0);
			for ( boost::filesystem::directory_iterator end, dir(currentDir);
				   dir != end; ++dir )
			{
				string pageName(dir->path().filename());
				string fullPath(dir->path().string());

				if(_isExcluded(_excludeListRegEx, pageName))
				{
					continue;
				}

				Webpage *page = new Webpage(WebPageTableSync::getId());
				_env.getEditableRegistry<Webpage>().add(boost::shared_ptr<Webpage>(page));
				_pages.push_back(page);
				page->set<Title>(pageName);
				page->setRoot(_site.get());
				page->setRank(rank++);
				page->setParent(parent);
				page->set<SmartURLPath>(fullPath.substr(directoryPath.string().length()));
				_site->addPage(*page);
				if( is_directory(*dir))
				{
					_importDir(directoryPath, page, *dir);
				}
				else
				{
					// It's a file, load its content
					page->set<MaxAge>(_maxAge);
					string extension(dir->path().extension());
					if(!extension.empty())
					{
						// boost path includes the . in the extension but
						// MimeTypes does not require it
						extension = extension.substr(1);
					}
					MimeType mimeType(MimeTypes::GetMimeTypeByExtension(extension));

					std::ifstream ifile(fullPath.c_str(), std::ifstream::in);
					std::string content;

					ifile.seekg(0, std::ios::end);
					content.reserve(ifile.tellg());
					ifile.seekg(0, std::ios::beg);

					content.assign((std::istreambuf_iterator<char>(ifile)),
								std::istreambuf_iterator<char>());
					WebpageContent c(content, false, mimeType, true);
					page->set<WebpageContent>(c);
				}
			}
		}

		CMSImport::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm)
		{}

		ParametersMap CMSImport::Importer_::getParametersMap() const
		{
			ParametersMap map;
			if(_parent.get())
			{
				map.insert(PARAMETER_PARENT_ID, _parent->getKey());
			}
			if(_site.get())
			{
				map.insert(PARAMETER_IMPORT_SITE_ID, _site->getKey());
			}
			if(!_maxAge.is_not_a_date_time())
			{
				map.insert(PARAMETER_MAX_AGE, _maxAge.total_seconds() / 60);
			}
			map.insert(PARAMETER_EXCLUDE_LIST, _excludeList);
			return map;
		}



		void CMSImport::Importer_::setFromParametersMap(const ParametersMap& map, bool doImport)
		{
			_directory = map.getDefault<string>(PARAMETER_DIRECTORY);
			if(_directory.empty())
			{
				throw Exception("Missing directory");
			}
			if( !is_directory(_directory))
			{
				throw Exception("Given directory does not exists");
			}

			if(map.getDefault<RegistryKeyType>(PARAMETER_PARENT_ID, 0))
			{
				try
				{
					_parent = WebPageTableSync::GetEditable(
						map.get<RegistryKeyType>(PARAMETER_PARENT_ID),
						_env
					);
					_site = WebsiteTableSync::GetEditable(_parent->getRoot()->getKey(), _env);
				}
				catch(ObjectNotFoundException<Webpage>&)
				{
					throw Exception("No such CMS web page parent");
				}
			}
			else if(map.getDefault<RegistryKeyType>(PARAMETER_IMPORT_SITE_ID, 0))
			{
				try
				{
					_site = WebsiteTableSync::GetEditable(
						map.get<RegistryKeyType>(PARAMETER_IMPORT_SITE_ID),
						_env
					);
				}
				catch(ObjectNotFoundException<Website>&)
				{
					throw Exception("No such CMS site");
				}
			}

			// Create a site if none provided
			if(!_site.get())
			{
				_site.reset(new Website());
			}
			else
			{
				// @TODO A site is provided, remove all its pages
			}

			_maxAge = minutes(map.getDefault<long>(PARAMETER_MAX_AGE, 0));
			_excludeList = map.getDefault<string>(PARAMETER_EXCLUDE_LIST);

			// Regexp are compiled, create them once here
			vector<string> excludes;
			boost::split(excludes, _excludeList, boost::algorithm::is_any_of(" "));
			BOOST_FOREACH(const string &oneexp, excludes)
			{
				try
				{
					if(oneexp[0] == '*')
					{
						// Special case to support regexp starting with * like in bash
						// in this case with prepend a '.' to the '*' to create '.*'
						_excludeListRegEx.push_back(boost::regex("." + oneexp));
					}
					else
					{
						_excludeListRegEx.push_back(boost::regex(oneexp));
					}
				}
				catch(boost::regex_error& e)
				{
					throw Exception("Bad regular expression in exclude list: " + string(e.what()));
				}
			}

		}



		db::DBTransaction CMSImport::Importer_::_save() const
		{
			DBTransaction transaction;

			WebsiteTableSync::Save(_site.get(), transaction);
			BOOST_FOREACH(const Registry<Webpage>::value_type& webPage, _env.getRegistry<Webpage>())
			{
				WebPageTableSync::Save(webPage.second.get(), transaction);
			}
			return transaction;
		}

	}
}

