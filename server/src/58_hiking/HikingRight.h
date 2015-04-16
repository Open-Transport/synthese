
/** HikingRight class header.
	@file HikingRight.h
	@author Hugues Romain
	@date 2010

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

#ifndef SYNTHESE_hiking_HikingRight_H__
#define SYNTHESE_hiking_HikingRight_H__

#include "12_security/RightTemplate.h"

namespace synthese
{
	namespace hiking
	{
		/** HikingRight Right Class.
			@ingroup m58Rights refRights

			Here are the different right levels :

			Private rights :
				- USE :
				- READ :
				- WRITE :
				- DELETE :

			Public rights :
				- USE :
				- READ :
				- WRITE :
				- DELETE :

			Perimeter definition :
				-
		*/
		class HikingRight : public security::RightTemplate<HikingRight>
		{
		public:
			/** Displays the interpretation of the string perimeter of the right.
				@return description of the perimeter
			*/
			std::string	displayParameter(util::Env& env)	const;


			/** Test of inclusion of an other perimeter by the current one.
				@param perimeter to compare with
				@return true if the current perimeter includes the compared one, false else
			*/
			bool perimeterIncludes(const std::string& perimeter, util::Env& env) const;
		};
	}
}

#endif // SYNTHESE_hiking_HikingRight_H__
