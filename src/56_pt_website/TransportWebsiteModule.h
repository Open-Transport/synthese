
/** TransportWebsiteModule class header.
	@file TransportWebsiteModule.h

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


#ifndef SYNTHESE_PlacesListModule_H__
#define SYNTHESE_PlacesListModule_H__

#include "GraphTypes.h"

#include "ModuleClassTemplate.hpp"

#include <utility>
#include <vector>
#include <boost/optional.hpp>

namespace synthese
{
	/**	@defgroup m56Actions 56 Actions
		@ingroup m56

		@defgroup m56Pages 56 Pages
		@ingroup m56

		@defgroup m56Functions 56 Functions
		@ingroup m56

		@defgroup m56Exceptions 56 Exceptions
		@ingroup m56

		@defgroup m56LS 56 Table synchronizers
		@ingroup m56

		@defgroup m56Admin 56 Administration pages
		@ingroup m56

		@defgroup m56Rights 56 Rights
		@ingroup m56

		@defgroup m56Logs 56 DB Logs
		@ingroup m56

		@defgroup m56 56 Transport website
		@ingroup m5


		@{
	*/

	/** 56 Transport website namespace. */
	namespace pt_website
	{
		/** 56 Transport website module class. */
		class TransportWebsiteModule:
			public server::ModuleClassTemplate<TransportWebsiteModule>
		{
		public:
			typedef std::vector<std::pair<boost::optional<graph::UserClassCode>, std::string> > UserClassNames;
			static UserClassNames GetAccessibilityNames();
		};
	}

	/** @} */
}

#endif // SYNTHESE_PlacesListModule_H__
