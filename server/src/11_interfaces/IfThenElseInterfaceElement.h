
/** IfThenElseInterfaceElement class header.
	@file IfThenElseInterfaceElement.h

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

#ifndef SYNTHESE_IfThenElseInterfaceElement_H__
#define SYNTHESE_IfThenElseInterfaceElement_H__

#include "11_interfaces/LibraryInterfaceElement.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace interfaces
	{
		/** Conditional value.

			Usage :
			@code if <condition> <to_return_if_condition_is_true> <to_return_if_condition_is_false (default=0)> @endcode

			Examples :
			The conditional value can be used for differencing the output according to a condition :
			@code print {{if {{param 2}} {the param 2 is ok} {the param 2 is ko}}} @endcode

			It can be used for branching purposes. In this case, do not forget to jump over the "else" bloc after the "then" one :
			@code
goto {{if {{param 2}} bloc1 bloc2}}

label bloc1
...
goto end_of_bloc

label bloc2
...

label end_of_bloc
			@endcode

			@ingroup m11Library refLibrary
		*/
		class IfThenElseInterfaceElement
			: public util::FactorableTemplate<interfaces::LibraryInterfaceElement, IfThenElseInterfaceElement>
		{
		private:
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _criteria;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _to_return_if_true;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _to_return_if_false;

		public:
			std::string display(
				std::ostream&
				, const ParametersVector& parameters, interfaces::VariablesMap& variables
				, const void* object = NULL, const server::Request* request = NULL) const;
			void storeParameters(ValueElementList& vel);
		};

	}
}

#endif // SYNTHESE_IfThenElseInterfaceElement_H__

