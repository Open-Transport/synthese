
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
#include "InterSYNTHESEPackage.hpp"
#include "InterSYNTHESEPackageContent.hpp"
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
	using namespace inter_synthese;
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
		const bool CMSImport::Importer_::IS_PERMANENT_THREAD(false);

		bool CMSImport::Importer_::parseFiles() const
		{
			// Load metadata if found
			string metadata;
			_readMetadata(metadata);
			if(!metadata.empty())
			{
				metadata = lexical_cast<string>(metadata.size()) +
					InterSYNTHESEPackage::SEPARATOR[0] +
					metadata;

				boost::shared_ptr<InterSYNTHESEPackage> package(new InterSYNTHESEPackage);
				_ispc.reset(new InterSYNTHESEPackageContent(_env, metadata, package, *this));


				BOOST_FOREACH(InterSYNTHESEPackageContent::LoadedObjects::value_type &reg, _ispc->getLoadedObjects())
				{
					if(decodeTableId(reg->getKey()) == WebPageTableSync::TABLE.ID)
					{
						Webpage *wp(dynamic_cast<Webpage*>(reg.get()));
						string path;
						_getPageFullPath(wp, path);
						_logLoad(string("Loading page from metadata: ") + wp->getName()
								 +  " path=" + path + " " + lexical_cast<string>(wp->getKey()));
						// We keep the metadata loaded pages in a map where the key is
						// the full page path to make it easy to find them in the file parsing
						// stage.
						_metadataPages[path] = wp;
					}
					else if(decodeTableId(reg->getKey()) == WebsiteTableSync::TABLE.ID)
					{
						_site = dynamic_pointer_cast<Website>(reg);
						_logLoad("Reusing site from metadata: " + _site->getName() +
								 " " + lexical_cast<string>(_site->getKey()));
					}
				}
			}

			if(!_site.get())
			{
				_site.reset(_createDefaultSite());
			}
			try {
				_logDebug("Parsing file on disk");
				_importDir( path(_directory), _site, _parent.get(), path());
			} catch(Exception &e)
			{
				_logLoad(string("Exception in importDir: ") + e.what());
			}

			return true;
		}

		/// Return the full path in @path of the given @page
		void CMSImport::Importer_::_getPageFullPath(Webpage *page, string &path) const
		{
			if(!page)
			{
				return;
			}

            
			if(path.empty())
			{
				path = page->getName();
			}
			else
			{
				path = page->getName() + "/" + path;
			}

			_getPageFullPath(page->getParent(true), path);
		}

		// Create a site based on the given directory name
		Website *CMSImport::Importer_::_createDefaultSite() const
		{
			Website *site(new Website());

			// Take the last directory as the ClientURL
			// The -2 avoids to count a trailing '/' in consideration
			string::size_type found =
					_directory.find_last_of("/",
											_directory.length() - 2);
			if(found != string::npos)
			{
				site->set<ClientURL>(_directory.substr(found));
			}

			string siteName(path(_directory).filename().string());
			site->set<Name>(siteName);
			_logLoad("Creation of site: " + siteName);

			return site;
		}

		void CMSImport::Importer_::_readMetadata(string &metadata) const
		{
			path metadataFile(_directory);
			metadataFile /= "metadata.json";
			ifstream metadataStream (metadataFile.string().c_str());
			if (metadataStream.is_open())
			{
				string line;
				while ( getline (metadataStream, line) )
				{
					metadata += line;
				}
				metadataStream.close();
				_logLoad("Metadata file loaded");
			}
			else
			{
				_logWarning("No metadata file found");
			}
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
			boost::shared_ptr<cms::Website> &site,
			cms::Webpage *parent,
			path currentDir
		) const
		{
			size_t rank(0);
			for ( boost::filesystem::directory_iterator end, dir(directoryPath / currentDir);
				   dir != end; ++dir )
			{
				string pageName(dir->path().filename().string());
				string absPath(dir->path().string());
				path relPath(currentDir / pageName);
				bool ignoreWhiteChars = false;
				bool doNotEvaluate = true;

				if(pageName == "metadata.json")
				{
					continue;
				}

				if(_isExcluded(_excludeListRegEx, pageName))
				{
					continue;
				}

				// Special case, if this is a directory and there is a file without
				// the .dir extension then its a page with content and is already
				// handled in the page load
				if( is_directory(*dir) &&
					extension(*dir) == ".dir" &&
					exists( change_extension(*dir, "")) )
				{
					continue;
				}

				// Try to find this page in the metadata
				Webpage *page;
				MimeType mimeType;
				string fullPagePath;
				_getPageFullPath(parent, fullPagePath);
                
				if(fullPagePath.empty())
				{
					fullPagePath = pageName;
				}
				else
				{
					fullPagePath += "/" + pageName;
				}

				if(_metadataPages.find(fullPagePath) != _metadataPages.end())
				{
					page = _metadataPages[fullPagePath];

					// We don't need this page anymore remove it thus the remaining
					// pages in _metadataPages are those who are no more on disk
					_metadataPages.erase(fullPagePath);

					_logLoad("Reusing page from metadata: " + pageName + " " + lexical_cast<string>(page->getKey()));
					mimeType = page->getMimeType();
					ignoreWhiteChars = page->get<WebpageContent>().getCMSScript().getIgnoreWhiteChars();
					doNotEvaluate = page->get<WebpageContent>().getCMSScript().getDoNotEvaluate();
				}
				else
				{
					// OVE : the creation of new pages has not been tested recently, it may be broken
					// in particular the management of ranks should be reworked
					page = new Webpage(WebPageTableSync::getId());
					_env.getEditableRegistry<Webpage>().add(boost::shared_ptr<Webpage>(page));
					_logLoad("Creation of new page: " + pageName + " " + lexical_cast<string>(page->getKey()));
					page->set<Title>(pageName);

					page->set<SmartURLPath>(string("/") + fullPagePath);
					page->set<RawEditor>(true);
					page->set<MaxAge>(_maxAge);
					page->setRoot(site.get());
					page->setRank(rank++);
					page->setParent(parent);

					// Calc a mime type base on the extension
					string extension(dir->path().extension().string());
					if(!extension.empty())
					{
						// boost path includes the . in the extension but
						// MimeTypes does not require it
						extension = extension.substr(1);
					}
					mimeType = MimeTypes::GetMimeTypeByExtension(extension);
				}

				site->addPage(*page);
				if( is_directory(*dir))
				{
					_logDebug("Recursing at: " + relPath.string());
					_importDir(directoryPath, site, page, relPath);
				}
				else
				{
					// It's a file, load its content
					string content;
					try
					{
						ifstream ifile;
						ifile.exceptions ( ifstream::failbit | ifstream::badbit );
						ifile.open(absPath.c_str(), ifstream::in);
						ifile.seekg(0, std::ios::end);
						content.reserve(ifile.tellg());
						ifile.seekg(0, std::ios::beg);
						content.assign((std::istreambuf_iterator<char>(ifile)),
									std::istreambuf_iterator<char>());
					}
					catch(ifstream::failure &e)
					{
						_logWarning(string("Failed to load file '") + absPath + "': " + e.what());
					}

					WebpageContent c(content, ignoreWhiteChars, mimeType, doNotEvaluate);
					page->set<WebpageContent>(c);

					// Special case, if there is a file with .dir extension
					// then this page is its parent
					if(exists(absPath + ".dir"))
					{
						_logLoad("Creation of root page: " + pageName);
						_importDir(directoryPath, site,
								   page, path(relPath.string() + ".dir"));
					}
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
			map.insert(PARAMETER_DIRECTORY, _directory);
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

			if(_site.get())
			{
				Log::GetInstance().debug("CMS Import Save site: "+ lexical_cast<string>(_site->getKey()));
				WebsiteTableSync::Save(_site.get(), transaction);
			}

			BOOST_FOREACH(const Registry<Webpage>::value_type& webPage, _env.getRegistry<Webpage>())
			{
				Log::GetInstance().debug("CMS Import Save: "+ lexical_cast<string>(webPage.second->getKey()));
				WebPageTableSync::Save(webPage.second.get(), transaction);
			}

			// Deletions of metadata pages for which no file on disk where found
			BOOST_FOREACH(MetadataPageMap::value_type &item, _metadataPages)
			{
				// Not the Remove function because the cascaded updates are already done by the object comparisons
				Log::GetInstance().debug("CMS SAVE Delete uneeded pages: "+ lexical_cast<string>(item.second->getKey()));
				DBModule::GetDB()->deleteStmt(
					item.second->getKey(),
					transaction
				);
			}
			return transaction;
		}

	}
}

