//////////////////////////////////////////////////////////////////////////
/// SpikeModule class header.
///	@file SpikeModule.hpp
///	@author Marc Jambert
///	@date 2014
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_SpikeModule_H__
#define SYNTHESE_SpikeModule_H__

#include "ModuleClassTemplate.hpp"

namespace synthese
{
	/**	@defgroup m62Actions 62 Actions
		@ingroup m62

		@defgroup m62Pages 62 Pages
		@ingroup m62

		@defgroup m62Functions 62 Functions
		@ingroup m62

		@defgroup m62Exceptions 62 Exceptions
		@ingroup m62

		@defgroup m62Alarm 62 Messages recipient
		@ingroup m62

		@defgroup m62LS 62 Table synchronizers
		@ingroup m62

		@defgroup m62Admin 62 Administration pages
		@ingroup m62

		@defgroup m62Rights 62 Rights
		@ingroup m62

		@defgroup m62Logs 62 DB Logs
		@ingroup m62

		@defgroup m62 62 Spike
		@ingroup m5

		(Module documentation)

		@{
	*/

	/** 62 Spike Module namespace.
		@author Marc Jambert
		@date 2014
	*/
	namespace spike
	{

		/** 62 Spike Module class.
			@author Marc Jambert
			@date 2014
		*/
		class SpikeModule:
			public server::ModuleClassTemplate<SpikeModule>
		{
		private:

		public:

			/** Initialization of the 62 Spike module after the automatic database loads.
				@author Marc Jambert
				@date 2014
			*/
			void initialize();

		};
	}
	/** @} */
}

#endif // SYNTHESE_SpikeModule_H__
