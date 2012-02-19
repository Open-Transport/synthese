//////////////////////////////////////////////////////////////////////////
/// AnalysisModule class header.
///	@file AnalysisModule.hpp
///	@author Gael Sauvanet
///	@date 2012
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

#ifndef SYNTHESE_AnalysisModule_H__
#define SYNTHESE_AnalysisModule_H__

#include "ModuleClassTemplate.hpp"

namespace synthese
{
	/**	@defgroup m60Actions 60 Actions
		@ingroup m60

		@defgroup m60Pages 60 Pages
		@ingroup m60

		@defgroup m60Functions 60 Functions
		@ingroup m60

		@defgroup m60Exceptions 60 Exceptions
		@ingroup m60

		@defgroup m60Alarm 60 Messages recipient
		@ingroup m60

		@defgroup m60LS 60 Table synchronizers
		@ingroup m60

		@defgroup m60Admin 60 Administration pages
		@ingroup m60

		@defgroup m60Rights 60 Rights
		@ingroup m60

		@defgroup m60Logs 60 DB Logs
		@ingroup m60

		@defgroup m60 60 Analysis
		@ingroup m5

		(Module documentation)

		@{
	*/

	/** 60 Analysis Module namespace.
		@author Gael Sauvanet
		@date 2012
	*/
	namespace analysis
	{

		/** 60 Analysis Module class.
			@author Gael Sauvanet
			@date 2012
		*/
		class AnalysisModule:
			public server::ModuleClassTemplate<AnalysisModule>
		{
		private:

		public:
			/** Initialization of the 60 Analysis module after the automatic database loads.
				@author Gael Sauvanet
				@date 2012
			*/
			void initialize();
		};
	}
	/** @} */
}

#endif // SYNTHESE_AnalysisModule_H__
