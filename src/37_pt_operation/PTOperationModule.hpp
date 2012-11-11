
/** 37 pt_operation module class header.
	@file PTOperationModule.hpp
	@author Hugues Romain
	@date 2011
	@since 3.2.1

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

#ifndef SYNTHESE_PTOperationModule_H__
#define SYNTHESE_PTOperationModule_H__

#include "GraphModuleTemplate.h"

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// 37 pt_operation Module namespace.
	/// @image html uml_pt_operation.png
	//////////////////////////////////////////////////////////////////////////
	///	@author Hugues Romain
	///	@date 2011
	/// @ingroup m37
	/// @since 3.2.1
	namespace pt_operation
	{
		/**	@defgroup m37Exceptions 37.01 Exceptions
			@ingroup m37

			@defgroup m37LS 37.10 Tables
			@ingroup m37
			@image html uml_pt_operation_tables.png

			@defgroup m37Rights 37.12 Rights
			@ingroup m37

			@defgroup m37Logs 37.13 DB Logs
			@ingroup m37

			@defgroup m37Admin 37.14 Administration pages
			@ingroup m37

			@defgroup m37Actions 37.15 Actions
			@ingroup m37

			@defgroup m37Functions 37.15 Services
			@ingroup m37

			@defgroup m37File 37.16 File formats
			@ingroup m37

			@defgroup m37Alarm 37.17 Messages recipient
			@ingroup m37

			@defgroup m37 37 pt_operation
			@ingroup m3
			@image html uml_pt_operation_tables.png

			@{
		*/


		//////////////////////////////////////////////////////////////////////////
		/// 37 pt_operation Module class.
		//////////////////////////////////////////////////////////////////////////
		///	@author Hugues Romain
		///	@date 2011
		/// @since 3.2.1
		class PTOperationModule:
			public graph::GraphModuleTemplate<PTOperationModule>
		{
		};
	}
	/** @} */
}

#endif // SYNTHESE_PTOperationModule_H__
