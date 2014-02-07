
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

#include "ActionException.h"
#include "CMSModule.hpp"
#include "CMSRight.hpp"
#include "MimeType.hpp"
#include "MimeTypes.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "User.h"
#include "WebpageContent.hpp"
#include "WebPageTableSync.h"
#include "WebsiteTableSync.hpp"

#include "boost/filesystem.hpp"
#include <fstream>
#include <streambuf>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::filesystem;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, cms::CMSImport>::FACTORY_KEY("CMSImport");
	}

	namespace cms
	{
		const string CMSImport::PARAMETER_DIRECTORY_PATH = "directory_path";
		const string CMSImport::PARAMETER_PARENT_ID = "parent_id";
		const string CMSImport::PARAMETER_SITE_ID = "site_id";
		const string CMSImport::PARAMETER_MAX_AGE = "max_age"; // In seconds
		const string CMSImport::PARAMETER_EXCLUDE_LIST = "exclude_list";

		ParametersMap CMSImport::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_DIRECTORY_PATH, _directoryPath);
			if(_parent.get())
			{
				map.insert(PARAMETER_PARENT_ID, _parent->getKey());
			}
			if(_site.get())
			{
				map.insert(PARAMETER_SITE_ID, _site->getKey());
			}
			map.insert(PARAMETER_MAX_AGE, _maxAge);
			map.insert(PARAMETER_EXCLUDE_LIST, _excludeList);
			return map;
		}



		void CMSImport::_setFromParametersMap(const ParametersMap& map)
		{
			_directoryPath = map.getDefault<string>(PARAMETER_DIRECTORY_PATH);
			if(_directoryPath.empty())
			{
				throw ActionException("Missing directory path");
			}
			if( !is_directory(_directoryPath))
			{
				throw ActionException("Given directory does not exists");
			}

			if(map.getDefault<RegistryKeyType>(PARAMETER_PARENT_ID, 0))
			{
				try
				{
					_parent = WebPageTableSync::GetEditable(
						map.get<RegistryKeyType>(PARAMETER_PARENT_ID),
						*_env
					);
					_site = WebsiteTableSync::GetEditable(_parent->getRoot()->getKey(), *_env);
				}
				catch(ObjectNotFoundException<Webpage>&)
				{
					throw ActionException("No such CMS web page parent");
				}
			}
			else if(map.getDefault<RegistryKeyType>(PARAMETER_SITE_ID, 0))
			{
				try
				{
					_site = WebsiteTableSync::GetEditable(
						map.get<RegistryKeyType>(PARAMETER_SITE_ID),
						*_env
					);
				}
				catch(ObjectNotFoundException<Website>&)
				{
					throw ActionException("No such CMS site");
				}
			}
			size_t maxAge = map.getDefault<size_t>(PARAMETER_MAX_AGE);
			_maxAge = boost::posix_time::time_duration(0, 0, maxAge);

			_excludeList = map.getDefault<string>(PARAMETER_EXCLUDE_LIST);
		}



		void CMSImport::run(
			Request& request
		){

			// Create a site if none provided
			if(!_site.get())
			{
				Website newSite;
				newSite.set<Name>(_directoryPath);

				// Take the last directory as the ClientURL
				// The -2 avoids to count a trailing '/' in consideration
				string::size_type found =
					_directoryPath.find_last_of("/",
												_directoryPath.length() - 2);
				if(found != string::npos)
				{
					newSite.set<ClientURL>(_directoryPath.substr(found));
				}
				WebsiteTableSync::Save(&newSite);
				_site = WebsiteTableSync::GetEditable(
					newSite.getKey(),
					*_env
				);
			}

			_importDir( _parent.get(), path(_directoryPath));

		}

		/// Import recursively the files in directory @currentDir under
		/// the page @parent
		void CMSImport::_importDir(
				Webpage *parent,
				path currentDir)
		{
			size_t rank(0);
			for ( boost::filesystem::directory_iterator end, dir(currentDir);
				   dir != end; ++dir )
			{
				Webpage page;
				string pageName(dir->path().filename());
				string fullPath(dir->path().string());
				page.set<Title>(pageName);
				page.setRoot(_site.get());
				page.setRank(rank++);
				page.setParent(parent);
				page.set<SmartURLPath>(fullPath.substr(_directoryPath.length()));
				_site->addPage(page);
				if( is_directory(*dir))
				{
					WebPageTableSync::Save(&page);
					_importDir(&page, *dir);
				}
				else
				{
					// It's a file, load its content
					page.set<MaxAge>(_maxAge);
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
					page.set<WebpageContent>(c);
					WebPageTableSync::Save(&page);
				}

			}
		}

		bool CMSImport::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<CMSRight>(WRITE);
		}
	}
}

