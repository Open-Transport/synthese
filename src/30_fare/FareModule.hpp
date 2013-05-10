
/** FareModule class header.
	@file FareModule.hpp

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

#ifndef SYNTHESE_FareModule_H__
#define SYNTHESE_FareModule_H__


#include "GraphModuleTemplate.h"
#include "SecurityTypes.hpp"
#include "Registry.h"

#include <vector>
#include <utility>
#include <boost/optional.hpp>

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// 30 Fare module namespace.
	///	@author Hugues Romain
	///	@ingroup m30
	namespace fare
	{
		/**	@defgroup m30Actions 30 Actions
			@ingroup m30

			@defgroup m30Pages 30 Pages
			@ingroup m30

			@defgroup m30Library 30 Interface library
			@ingroup m30

			@defgroup m30Functions 30 Functions
			@ingroup m30

			@defgroup m30LS 30 Table synchronizers
			@ingroup m30

			@defgroup m30Admin 30 Administration pages
			@ingroup m30

			@defgroup m30Rights 30 Rights
			@ingroup m30

			@defgroup m30Logs 30 DB Logs
			@ingroup m30

			@defgroup m30File 30.16 File formats
			@ingroup m30

			@defgroup m30WFS 30.39 WFS types
			@ingroup m30

			@defgroup m30 30 Transport
			@ingroup m3

		@{
		*/

		/** 30 Fare module class.
		*/
		class FareModule:
			public graph::GraphModuleTemplate<FareModule>
		{
		public:

		private:

		public:
		};
	}
	/** @} */
}

#endif
