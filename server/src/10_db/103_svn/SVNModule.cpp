
/** SVNModule class implementation.
	@file SVNModule.cpp

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

#include "SVNModule.hpp"

using namespace std;
using namespace boost::filesystem;

namespace synthese
{
	using namespace db::svn;

	template<> const std::string util::FactorableTemplate<server::ModuleClass, SVNModule>::FACTORY_KEY("103_svn");

	namespace db
	{
		namespace svn
		{
			const string SVNModule::SYNTHESE_PARAM_SVN_WC_ROOT_PATH = "svn_wc_root_path";
			path SVNModule::_svn_wc_root_path;
		}
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<SVNModule>::NAME("SVN storage");


		template<> void ModuleClassTemplate<SVNModule>::PreInit()
		{
			RegisterParameter(SVNModule::SYNTHESE_PARAM_SVN_WC_ROOT_PATH, "", &SVNModule::ParameterCallback);
		}



		template<> void ModuleClassTemplate<SVNModule>::Init()
		{
		}


		template<> void ModuleClassTemplate<SVNModule>::Start()
		{
		}



		template<> void ModuleClassTemplate<SVNModule>::End()
		{
		}



		template<> void ModuleClassTemplate<SVNModule>::InitThread(
			
			){
		}



		template<> void ModuleClassTemplate<SVNModule>::CloseThread(
			
			){
		}
	}

	namespace db
	{
		namespace svn
		{
			SVNModule::SVNModule()
			{}



			void SVNModule::ParameterCallback(
				const std::string& name,
				const std::string& value
			){
				if(name == SYNTHESE_PARAM_SVN_WC_ROOT_PATH)
				{
					_svn_wc_root_path = value;
				}
			}



			const boost::filesystem::path& SVNModule::GetSVNWCRootPath()
			{
				return _svn_wc_root_path;
			}
}	}	}
