
/** ForumModule class header.
	@file ForumModule.hpp
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

#ifndef SYNTHESE_ForumModule_H__
#define SYNTHESE_ForumModule_H__

#include "ModuleClassTemplate.hpp"

namespace synthese
{
	/**	@defgroup m52Exceptions 52.01 Exceptions
		@ingroup m52

		@defgroup m52LS 52.10 Table synchronizers
		@ingroup m52

		@defgroup m52Pages 52.11 Interface Pages
		@ingroup m52

		@defgroup m52Rights 52.12 Rights
		@ingroup m52

		@defgroup m52Logs 52.13 DB Logs
		@ingroup m52

		@defgroup m52Admin 52.14 Administration pages
		@ingroup m52

		@defgroup m52Actions 52.15 Actions
		@ingroup m52

		@defgroup m52Functions 52.15 Functions
		@ingroup m52

		@defgroup m52File 52.16 File formats
		@ingroup m52

		@defgroup m52Alarm 52.17 Messages recipient
		@ingroup m52

		@defgroup m52 52 Forum
		@ingroup m5

		@todo Write Module documentation

		@{
	*/

	/** 52 Forum Module namespace.
		@author Hugues Romain
		@date 2010
	*/
	namespace forum
	{

		/** 52 Forum Module class.
			@author Hugues Romain
			@date 2010
		*/
		class ForumModule:
			public server::ModuleClassTemplate<ForumModule>
		{
		public:
			/** Initialization of the 52 Forum module after the automatic database loads.
				@author Hugues Romain
				@date 2010
			*/
			void initialize();
		};
	}
	/** @} */
}

#endif // SYNTHESE_ForumModule_H__
