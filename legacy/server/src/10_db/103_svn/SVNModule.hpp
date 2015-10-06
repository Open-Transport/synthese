
/** SVNModule class header.
	@file SVNModule.hpp

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

#ifndef SYNTHESE_svn_SVNModule_hpp__
#define SYNTHESE_svn_SVNModule_hpp__

#include "ModuleClassTemplate.hpp"

#include <boost/filesystem.hpp>

namespace synthese
{
	namespace db
	{
		namespace svn
		{
			/** SVNModule class.
				@ingroup m103
			*/
			class SVNModule:
				public server::ModuleClassTemplate<SVNModule>
			{
			public:
				static const std::string SYNTHESE_PARAM_SVN_WC_ROOT_PATH;

				static boost::filesystem::path _svn_wc_root_path;

				SVNModule();

				/** Called whenever a parameter registered by this module is changed
				*/
				static void ParameterCallback(
					const std::string& name,
					const std::string& value
				);

				static const boost::filesystem::path& GetSVNWCRootPath();
			};
}	}	}

#endif // SYNTHESE_svn_SVNModule_hpp__
