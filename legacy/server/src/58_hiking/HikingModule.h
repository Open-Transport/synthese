
/** HikingModule class header.
	@file HikingModule.h
	@author Hugues
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

#ifndef SYNTHESE_HikingModule_H__
#define SYNTHESE_HikingModule_H__

#include "ModuleClassTemplate.hpp"

namespace synthese
{
	/**	@defgroup m58Actions 58 Actions
		@ingroup m58

		@defgroup m58Pages 58 Pages
		@ingroup m58

		@defgroup m58Functions 58 Functions
		@ingroup m58

		@defgroup m58Exceptions 58 Exceptions
		@ingroup m58

		@defgroup m58Alarm 58 Messages recipient
		@ingroup m58

		@defgroup m58LS 58 Table synchronizers
		@ingroup m58

		@defgroup m58Admin 58 Administration pages
		@ingroup m58

		@defgroup m58Rights 58 Rights
		@ingroup m58

		@defgroup m58Logs 58 DB Logs
		@ingroup m58

		@defgroup m58 58 Hiking
		@ingroup m5

		(Module documentation)

		@{
	*/

	/** 58 Hiking Module namespace.
		@author Hugues Romain
		@date 2010
	*/
	namespace hiking
	{

		/** 58 Hiking Module class.
			@author Hugues
			@date 2010
		*/
		class HikingModule:
			public server::ModuleClassTemplate<HikingModule>
		{
		private:

		public:
			/** Initialization of the 58 Hiking module after the automatic database loads.
				@author Hugues
				@date 2010
			*/
			void initialize();
		};
	}
	/** @} */
}

#endif // SYNTHESE_HikingModule_H__
