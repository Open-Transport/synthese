
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

#include <map>
#include <string>

#include "01_util/ModuleClass.h"

#include "12_security/Profile.h"

/** @defgroup m12 12 Security
@{
*/
namespace synthese
{
	namespace security
	{
		class SecurityModule : public util::ModuleClass
		{
		public:
			static const std::string ROOT_PROFILE;
			static const std::string ROOT_RIGHTS;
			static const std::string ROOT_USER;

		private:
			static Profile::Registry _profiles;

		public:
			void initialize();

			static Profile::Registry& getProfiles();

			static std::map<std::string, std::string> getRightsTemplates();
			static std::map<uid, std::string> getProfileLabels(bool withAll=false, int first=0, int last=-1);
			static std::map<uid, std::string> getUserLabels(bool withAll=false, int first=0, int last=-1);
			static std::map<std::string, std::string> getRightLabels(bool withAll=false);
		};
	}
}

/** @} */

#endif // SYNTHESE_SecurityModule_H__

