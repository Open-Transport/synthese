
/** CMSRight class header.
	@file CMSRight.hpp
	@author Hugues Romain
	@date 2011

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

#ifndef SYNTHESE_cms_CMSRight_H__
#define SYNTHESE_cms_CMSRight_H__

#include "RightTemplate.h"

namespace synthese
{
	namespace cms
	{
		/** CMSRight Right Class.
			@ingroup m36Rights refRights

			Here are the different right levels :

			Private rights : none

			Public rights :
				- USE : View a CMS page
				- READ : View a CMS page source
				- WRITE : Edit a CMS page source
				- DELETE : Delete a CMS page

			Perimeter definition :
				- an ID of a website : apply on all pages of the website and the website itself
				- an ID of a page : apply on the page and its subpages. All accesses on the website are forbidden
		*/
		class CMSRight:
			public security::RightTemplate<CMSRight>
		{
		public:
			/** Displays the interpretation of the string perimeter of the right.
				@return description of the perimeter
			*/
			std::string	displayParameter(util::Env& env)	const;


			/** Tests if a page or site is included in the right perimeter.
				@param id of a site or a page
				@return true if the current perimeter includes the compared one, false else
			*/
			bool perimeterIncludes(const std::string& perimeter, util::Env& env) const;
		};
}	}

#endif // SYNTHESE_cms_CMSRight_H__
