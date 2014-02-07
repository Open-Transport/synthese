
//////////////////////////////////////////////////////////////////////////
/// CMSExport class implementation.
/// @file CMSExport.cpp
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

#include "CMSExport.hpp"

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
		template<> const string FactorableTemplate<FileFormat, CMSExport>::FACTORY_KEY("CMS Export");
	}

	namespace data_exchange
	{
		const string CMSExport::Importer_::PARAMETER_DIRECTORY = "directory";
		const string CMSExport::Importer_::PARAMETER_PARENT_ID = "parent_id";
		const string CMSExport::Importer_::PARAMETER_IMPORT_SITE_ID = "import_site_id";
		const string CMSExport::Importer_::PARAMETER_WITH_METADATA = "with_metadata";

		const bool CMSExport::Importer_::IMPORTABLE(true);
		const bool CMSExport::Importer_::IS_PERMANENT_THREAD(false);

		bool CMSExport::Importer_::parseFiles() const	{
			return true;
		}

		CMSExport::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm)
		{}

		ParametersMap CMSExport::Importer_::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_DIRECTORY, _directory);
			if(_parent.get())
			{
				map.insert(PARAMETER_PARENT_ID, _parent->getKey());
			}
			if(_site.get())
			{
				map.insert(PARAMETER_IMPORT_SITE_ID, _site->getKey());
			}
			map.insert(PARAMETER_WITH_METADATA, _withMetadata);
			return map;
		}



		void CMSExport::Importer_::setFromParametersMap(const ParametersMap& map, bool doImport)
		{
			_directory = map.getDefault<string>(PARAMETER_DIRECTORY);
			if(_directory.empty())
			{
				throw Exception("Missing directory");
			}
			if( !is_directory(_directory))
			{
				try {
					create_directory(_directory);
				}
				catch(filesystem_error &e)
				{
					throw Exception("Failed to create directory: " + string(e.what()));
				}
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
			else
			{
				throw Exception("You must provide at least a site or a parent");
			}

			_withMetadata = map.getDefault<bool>(PARAMETER_WITH_METADATA);
		}

		/// Export recursively the files in directory @currentDir that are under
		/// the page @parent
		void CMSExport::Importer_::_exportDir(const path &directoryPath,
			cms::Webpage *parent,
			path currentDir) const
		{
			WebpageContent content(parent->get<WebpageContent>());
			string pageName( parent->get<Title>() );
			string pageNameWithExt( pageName );
			if(pageNameWithExt.empty())
			{
				pageNameWithExt = lexical_cast<string>(parent->getKey());
				if(!content.getMimeType().getDefaultExtension().empty())
				{
					pageNameWithExt += "." + content.getMimeType().getDefaultExtension();
				}
				else
				{
					pageNameWithExt += ".unknown";
				}
			}

			// savePage
			create_directory( currentDir );

			// We create the page on disk if there is some content in it
			// or if there is no subpages
			if(!content.getCMSScript().getCode().empty() ||
			   parent->getChildren().empty())
			{
				path currentFile(currentDir / pageNameWithExt);
				_logLoad("Creation of page: " + currentFile.string());
				ofstream file( currentFile.string().c_str() );
				file << content.getCMSScript().getCode().c_str();
				file.close();
				// If there are subpages we need to differentiate the file and the dir
				pageName += ".dir";
			}
			BOOST_FOREACH(const Webpage::ChildrenType::value_type& page, parent->getChildren())
			{
				_exportDir(directoryPath, page.second, currentDir / pageName);
			}
		}


		db::DBTransaction CMSExport::Importer_::_save() const
		{
			DBTransaction transaction;

			WebPageTableSync::SearchResult pages(
				WebPageTableSync::Search(
					Env::GetOfficialEnv(),
					(_site.get() ? _site->getKey() : optional<RegistryKeyType>()),
					(_parent.get() ?_parent->getKey() : RegistryKeyType(0))
			)	);

			for(WebPageTableSync::SearchResult::const_iterator it(pages.begin()); it != pages.end(); ++it)
			{
				boost::shared_ptr<Webpage> page(*it);

				_exportDir( path(_directory), page.get(), path(_directory));
			}
			return transaction;
		}

	}
}

