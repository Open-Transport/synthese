
/** InterSYNTHESEIdFilter class header.
	@file InterSYNTHESEIdFilter.hpp

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

#ifndef SYNTHESE_inter_synthese_InterSYNTHESEIdFilter_hpp__
#define SYNTHESE_inter_synthese_InterSYNTHESEIdFilter_hpp__

#include "FactoryBase.h"
#include "UtilTypes.h"

namespace synthese
{
	namespace impex
	{
		class DataSource;
	}

	namespace inter_synthese
	{
		/** InterSYNTHESEIdFilter class.
			@ingroup m19
		*/
		class InterSYNTHESEIdFilter:
			public util::FactoryBase<InterSYNTHESEIdFilter>
		{
		protected:
			impex::DataSource* _dataSource;

		public:
			void setDataSource(impex::DataSource* value){ _dataSource = value; }

			virtual std::string convertId(
				util::RegistryTableType tableId,
				const std::string& fieldName,
				const std::string& objectId
			) const = 0;
		};
	}
}

#endif // SYNTHESE_inter_synthese_InterSYNTHESEIdFilter_hpp__

	