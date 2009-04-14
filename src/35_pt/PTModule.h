
/** PTModule class header.
	@file PTModule.h

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

#ifndef SYNTHESE_PTModule_H__
#define SYNTHESE_PTModule_H__


#include "GraphModuleTemplate.h"


namespace synthese
{
	/**	@defgroup m35Actions 35 Actions
		@ingroup m35

		@defgroup m35Pages 35 Pages
		@ingroup m35

		@defgroup m35Library 35 Interface library
		@ingroup m35

		@defgroup m35Functions 35 Functions
		@ingroup m35

		@defgroup m35LS 35 Table synchronizers
		@ingroup m35

		@defgroup m35Admin 35 Administration pages
		@ingroup m35

		@defgroup m35Rights 35 Rights
		@ingroup m35

		@defgroup m35Logs 35 DB Logs
		@ingroup m35

		@defgroup m35 35 Transport
		@ingroup m3
	@{
	*/

	/** 35 Transport network module namespace.
		The env name is due to historical reasons.
	*/
	namespace pt
	{
		/** 35 Transport network module class.
		*/
		class PTModule:
			public graph::GraphModuleTemplate<PTModule>
		{


		public:
		
			void initialize();
			

			virtual std::string getName() const;
			
		};
	}
	/** @} */
}

#endif // SYNTHESE_EnvModule_H__
