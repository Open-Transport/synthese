
/** ImpExModule class header.
	@file ImpExModule.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_ImpExModule_H__
#define SYNTHESE_ImpExModule_H__

#include "ModuleClassTemplate.hpp"

#include <boost/optional.hpp>

namespace synthese
{
	/**	@defgroup m16Exceptions 16 Exceptions
		@ingroup m16.01

		@defgroup m16LS 16 Table synchronizers
		@ingroup m16.10

		@defgroup m16Rights 16 Rights
		@ingroup m16.12

		@defgroup m16Logs 16 DB Logs
		@ingroup m16.13

		@defgroup m16Admin 16 Administration pages
		@ingroup m16.14

		@defgroup m16Actions 16 Actions
		@ingroup m16.15

		@defgroup m16Functions 16 Functions
		@ingroup m16.15

		@defgroup m16 16 Import export framework
		@ingroup m1

		Management of data sources and standardized import / export interface.

		@{
	*/

	/** 16 Import export framework Module namespace.
		@author Hugues Romain
		@date 2008
	*/
	namespace impex
	{

		/** 16 Import export framework Module class.
			@author Hugues Romain
			@date 2008
		*/
		class ImpExModule:
			public server::ModuleClassTemplate<ImpExModule>
		{
		private:


		public:
			typedef std::vector<std::pair<boost::optional<std::string>, std::string> > FileFormatsList;
			static FileFormatsList GetFileFormatsList();

			static void AutoImportsThread();
		};
	}
	/** @} */
}

#endif // SYNTHESE_ImpExModule_H__
