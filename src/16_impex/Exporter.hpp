
/** Exporter class header.
	@file Exporter.hpp

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

#ifndef SYNTHESE_impex_Exporter_hpp__
#define SYNTHESE_impex_Exporter_hpp__

#include "Env.h"
#include "ParametersMap.h"

namespace synthese
{
	namespace impex
	{
		/** Exporter interface.
			@ingroup m16
			@since 3.2.0
			@date 2010
		*/
		class Exporter
		{
		protected:
			mutable util::Env						_env;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// @param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			virtual void setFromParametersMap(
				const util::ParametersMap& map
			) = 0;

			virtual util::ParametersMap getParametersMap() const = 0;

			virtual std::string getFileFormatKey() const = 0;



			/** Generic export method.
				The generic export consists in the export of each registered exportable class
				@author Hugues Romain
				@date 2010
			*/
			virtual void build(
				std::ostream& os
			) const = 0;

			virtual std::string getOutputMimeType() const = 0;
		};
	}
}

#endif // SYNTHESE_impex_Exporter_hpp__
