
/** SecurityModule class header.
	@file SecurityModule.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#ifndef SYNTHESE_SecurityModule_H__
#define SYNTHESE_SecurityModule_H__

#include <string>
#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "ModuleClassTemplate.hpp"
#include "UId.h"

namespace synthese
{
	/**	@defgroup m12Actions 12 Actions
		@ingroup m12

		@defgroup m12Pages 12 Pages
		@ingroup m12

		@defgroup m12Values 12 Values
		@ingroup m12

		@defgroup m12Functions 12 Functions
		@ingroup m12

		@defgroup m12LS 12 Table synchronizers
		@ingroup m12

		@defgroup m12Admin 12 Administration pages
		@ingroup m12

		@defgroup m12Rights 12 Rights
		@ingroup m12

		@defgroup m12Logs 12 DB Logs
		@ingroup m12

		@defgroup m12 12 Security
		@ingroup m1
		@{
	*/

	/** 12 Security module namespace.
	*/
	namespace security
	{
		class User;
		class Profile;

		/** 12 Security module class.
		*/
		class SecurityModule:
			public server::ModuleClassTemplate<SecurityModule>
		{
			friend class server::ModuleClassTemplate<SecurityModule>;
			
		public:
			static const std::string ROOT_PROFILE;
			static const std::string ROOT_RIGHTS;
			static const std::string ROOT_USER;

		private:
			static boost::shared_ptr<Profile>	_rootProfile;
			static boost::shared_ptr<User>		_rootUser;

		public:
			static std::vector<std::pair<std::string, std::string> > getRightsTemplates();
			static std::vector<std::pair<uid, std::string> > getProfileLabels(bool withAll=false, int first=0, int last=-1);
			static std::vector<std::pair<uid, std::string> > getUserLabels(bool withAll=false, int first=0, int last=-1);
			static std::vector<std::pair<std::string, std::string> > getRightLabels(bool withAll=false);

			/** List of the sub profiles of the current one.
					@return std::vector<Profile*> List of the sub profiles of the current one.
					@author Hugues Romain
					@date 2007					
			*/
			static std::vector<boost::shared_ptr<Profile> > getSubProfiles(boost::shared_ptr<const Profile> profile);
		};
	}
	/** @} */
}

#endif // SYNTHESE_SecurityModule_H__
