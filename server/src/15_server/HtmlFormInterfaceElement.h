
/** HtmlFormInterfaceElement class header.
	@file HtmlFormInterfaceElement.h

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

#ifndef SYNTHESE_HtmlFormInterfaceElement_H__
#define SYNTHESE_HtmlFormInterfaceElement_H__

#include "LibraryInterfaceElement.h"
#include "FactorableTemplate.h"
#include <vector>

namespace synthese
{
	namespace server
	{
		/** HTML Form generator Value Interface Element Class.
			@ingroup m15Library refLibrary

			Parameters :
				- 0 : form name
				- 1 : function key (optional) (empty = current function)
				- 2 : function parameters (query string format) (empty = current fixed parameters)
				- 3 : action key (optional)
				- 4 : action parameters (query string format)
				- 5 and following : fields not to fill with input type=hidden
		*/
		class HtmlFormInterfaceElement
			: public util::FactorableTemplate<interfaces::LibraryInterfaceElement, HtmlFormInterfaceElement>
		{
			// List of parameters to store
			boost::shared_ptr<interfaces::LibraryInterfaceElement>					_name;
			boost::shared_ptr<interfaces::LibraryInterfaceElement>					_function_key;
			boost::shared_ptr<interfaces::LibraryInterfaceElement>					_function_parameters;
			boost::shared_ptr<interfaces::LibraryInterfaceElement>					_action_key;
			boost::shared_ptr<interfaces::LibraryInterfaceElement>					_action_parameters;
			std::vector<boost::shared_ptr<interfaces::LibraryInterfaceElement> >	_fieldsToAvoid;
			bool																	_with_action;

		public:
			/** Parameters parser.
				@param vel Standard list of parameters
			*/
			void storeParameters(interfaces::ValueElementList& vel);
			std::string display(
				std::ostream&
				, const interfaces::ParametersVector& parameters
				,interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL) const;

		};
	}
}

#endif // SYNTHESE_HtmlFormInterfaceElement_H__

