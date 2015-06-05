
/** IneoTerminusModule class header.
	@file IneoTerminusModule.h

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

#ifndef SYNTHESE_IneoTerminusModule_H__
#define SYNTHESE_IneoTerminusModule_H__

#include "GraphModuleTemplate.h"

#include <vector>
#include <utility>
#include <boost/optional.hpp>

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// 61 Ineo Terminus module namespace.
	///	@author RCS Mobility
	///	@ingroup m61
	namespace ineo_terminus
	{

		/** 61 Ineo Terminus module class.
		*/
		class IneoTerminusModule:
			public graph::GraphModuleTemplate<IneoTerminusModule>
		{
		private:

		public:
			static void ParameterCallback(
				const std::string& name,
				const std::string& value
			);
		};
	}
	/** @} */
}

#endif // SYNTHESE_IneoTerminusModule_H__
