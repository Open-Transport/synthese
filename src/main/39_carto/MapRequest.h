
/** MapRequest class header.
	@file MapRequest.h

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

#ifndef SYNTHESE_MapRequest_H__
#define SYNTHESE_MapRequest_H__

#include "30_server/Request.h"

namespace synthese
{
	namespace carto
	{
		/** MapRequest class.
			@ingroup m39
		*/
		class MapRequest : public server::Request
		{
			/// @todo request parameter names declaration
			// eg : static const std::string PARAMETER_xxx;
			
			//! \name Page parameters
			//@{
				/// @todo Parsed parameters declaration
				// eg : const void*	_object;
				// eg : ParametersMap			_parameters;
			//@}


			/** Conversion from attributes to generic parameter maps.
			*/
			Request::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			*/
			void setFromParametersMap(const Request::ParametersMap& map);

		public:
			MapRequest();
			~MapRequest();

			/// @todo Getters/Setters for parsed parameters

			/** Action to run, defined by each subclass.
			*/
			void run(std::ostream& stream) const;
		};
	}
}
#endif // SYNTHESE_MapRequest_H__
