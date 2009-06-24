
/** ImpExModule class header.
	@file ImpExModule.h
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

#ifndef SYNTHESE_ImpExModule_H__
#define SYNTHESE_ImpExModule_H__

#include "ModuleClassTemplate.hpp"

namespace synthese
{
	/**	@defgroup m16Actions 16 Actions
		@ingroup m16

		@defgroup m16Pages 16 Pages
		@ingroup m16

		@defgroup m16Functions 16 Functions
		@ingroup m16

		@defgroup m16Exceptions 16 Exceptions
		@ingroup m16

		@defgroup m16Alarm 16 Messages recipient
		@ingroup m16

		@defgroup m16LS 16 Table synchronizers
		@ingroup m16

		@defgroup m16Admin 16 Administration pages
		@ingroup m16

		@defgroup m16Rights 16 Rights
		@ingroup m16

		@defgroup m16Logs 16 DB Logs
		@ingroup m16
		
		@defgroup m16 Import export framework
		@ingroup m1
		
		(Module documentation)
		
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
		};
	}
	/** @} */
}

#endif // SYNTHESE_ImpExModule_H__
