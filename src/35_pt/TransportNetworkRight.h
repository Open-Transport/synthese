
/** TransportNetworkRight class header.
	@file TransportNetworkRight.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_env_TransportNetworkRight_H__
#define SYNTHESE_env_TransportNetworkRight_H__

#include "RightTemplate.h"

namespace synthese
{
	namespace pt
	{
		/** TransportNetworkRight Right Class.
			@ingroup m35Rights refRights

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
		class TransportNetworkRight:
			public security::RightTemplate<TransportNetworkRight>
		{
		public:
			/** Displays the interpretation of the string perimeter of the right.
				@return description of the perimeter
			*/
			std::string	displayParameter(
				util::Env& env = util::Env::GetOfficialEnv()
			)	const;
			

			/** Test of inclusion of an other perimeter by the current one.
				@param perimeter to compare with
				@return true if the current perimeter includes the compared one, false else
			*/
			bool perimeterIncludes(
				const std::string& perimeter,
				util::Env& env = util::Env::GetOfficialEnv()
			) const;
		};
	}
}

#endif // SYNTHESE_env_TransportNetworkRight_H__
