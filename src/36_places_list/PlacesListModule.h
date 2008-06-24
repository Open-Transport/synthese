
/** PlacesListModule class header.
	@file PlacesListModule.h

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


#ifndef SYNTHESE_PlacesListModule_H__
#define SYNTHESE_PlacesListModule_H__

#include "36_places_list/Types.h"

#include "01_util/ModuleClass.h"
#include "01_util/FactorableTemplate.h"

#include <utility>
#include <vector>

namespace synthese
{
	/**	@defgroup m36Actions 36 Actions
		@ingroup m36

		@defgroup m36Pages 36 Pages
		@ingroup m36

		@defgroup m36Functions 36 Functions
		@ingroup m36

		@defgroup m36Exceptions 36 Exceptions
		@ingroup m36

		@defgroup m36LS 36 Table synchronizers
		@ingroup m36

		@defgroup m36Admin 36 Administration pages
		@ingroup m36

		@defgroup m36Rights 36 Rights
		@ingroup m36

		@defgroup m36Logs 36 DB Logs
		@ingroup m36
		
		@defgroup m36 36 Transport website
		@ingroup m3
		
		(Module documentation)
		
		@{
	*/

	/** 16 Transport website namespace. */
	namespace transportwebsite
	{
		/** 16 Transport website module class. */
		class PlacesListModule : public util::FactorableTemplate<util::ModuleClass, PlacesListModule>
		{
		public:

			void initialize();

			virtual std::string getName() const;

			static std::vector<std::pair<AccessibilityParameter, std::string> >  GetAccessibilityNames();
		};
	}

	/** @} */
}

#endif // SYNTHESE_PlacesListModule_H__
