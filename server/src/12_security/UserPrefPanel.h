
/** UserPrefPanel class header.
	@file UserPrefPanel.h

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

#ifndef SYNTHESE_security_UserPrefPanel_h__
#define SYNTHESE_security_UserPrefPanel_h__

#include "FactoryBase.h"

#include <ostream>

namespace synthese
{
	namespace security
	{
		class User;

		/** UserPrefPanel class.
			@ingroup m12
		*/
		class UserPrefPanel
		:	public util::FactoryBase<UserPrefPanel>
		{
		public:
			class Args
			{
				//const User* _user;
			};

//			UserPrefPanel(const User* user)
//				: _user(user)
//				, util::Factorable<UserPrefPanel>()
//			{}

			virtual void display(std::ostream& stream, const User* user) const = 0;
			virtual const std::string& getName() const = 0;
		};
	}
}

#endif // SYNTHESE_security_UserPrefPanel_h__
