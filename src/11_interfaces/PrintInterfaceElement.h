
/** PrintInterfaceElement class header.
	@file PrintInterfaceElement.h

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

#ifndef SYNTHESE_PrintInterfaceElement_H__
#define SYNTHESE_PrintInterfaceElement_H__

#include "LibraryInterfaceElement.h"
#include "FactorableTemplate.h"
#include <vector>

namespace synthese
{
	namespace interfaces
	{
		/** Print interface library element.
			@ingroup m11Library refLibrary

		Usage :
			@code {{$ <param1> <param2> ... <paramn>}} @endcode

			The output is the concatenation of each param.
			@warning no space will be inserted between the parameters.

			Example :
			@code {{$ This { is } an [ interesting way of coding] ! {{param 1}}}} @endcode
			with the first execution parameter containing the ? character, it outputs :
			@code This is an interesting way of coding!? @endcode
		*/
		class PrintInterfaceElement
			: public util::FactorableTemplate<interfaces::LibraryInterfaceElement, PrintInterfaceElement>
		{
			std::vector<boost::shared_ptr<interfaces::LibraryInterfaceElement> > _parameters; // Parameters storage

		public:
			/** Parameters parser.
				The parser copies the ValueElementList as is.
			*/
			void storeParameters(ValueElementList& vel);

			std::string display(
				std::ostream& stream
				, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL
			) const;
		};
	}
}

#endif // SYNTHESE_PrintInterfaceElement_H__
