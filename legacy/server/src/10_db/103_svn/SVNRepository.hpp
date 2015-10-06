
/** SVNRepository class header.
	@file SVNRepository.hpp

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

#ifndef SYNTHESE_svn_SVNRepository_hpp__
#define SYNTHESE_svn_SVNRepository_hpp__

#include "SVNCommands.hpp"

#include <boost/filesystem/path.hpp>

namespace synthese
{
	namespace db
	{
		namespace svn
		{
			/** Subversion repository access class.
				@ingroup m103
			*/
			class SVNRepository
			{
			private:
				std::string _url;

			public:
				SVNRepository();
				SVNRepository(const std::string& url);

				/// @name Getters
				//@{
					const std::string& getURL() const { return _url; }
				//@}

				/// @name Setters
				//@{
					void setURL(const std::string& value){ _url = value; }
				//@}

				/// @name Services
				//@{
					//////////////////////////////////////////////////////////////////////////
					/// Creates the directory specified by the URL of the repository
					void mkdir(
						const std::string& user,
						const std::string& password
					) const;



					//////////////////////////////////////////////////////////////////////////
					/// Checks out a working copy in the local file system
					void checkout(
						const std::string& user,
						const std::string& password,
						const boost::filesystem::path& localPath
					) const;



					//////////////////////////////////////////////////////////////////////////
					/// Obtains the list of the elements contained in the root of the repository
					SVNCommands::LsResult ls(
						const std::string& user,
						const std::string& password
					) const;



					//////////////////////////////////////////////////////////////////////////
					/// Obtains the list of the elements contained in a specified directory of the repository
					SVNCommands::LsResult ls(
						const std::string& path,
						const std::string& user,
						const std::string& password
					) const;
				//@}
			};
}	}	}

#endif // SYNTHESE_cms_SVNRepository_hpp__
