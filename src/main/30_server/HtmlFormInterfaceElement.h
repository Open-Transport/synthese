
/** HtmlFormInterfaceElement class header.
	@file HtmlFormInterfaceElement.h

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

#ifndef SYNTHESE_HtmlFormInterfaceElement_H__
#define SYNTHESE_HtmlFormInterfaceElement_H__

#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace server
	{
		class HtmlFormInterfaceElement : public interfaces::ValueInterfaceElement
		{
			// List of parameters to store
			interfaces::ValueInterfaceElement* _name;
			interfaces::ValueInterfaceElement* _function_key;
			interfaces::ValueInterfaceElement* _function_parameters;
			interfaces::ValueInterfaceElement* _action_key;
			interfaces::ValueInterfaceElement* _action_parameters;
			bool								_with_action;

		public:
			/** Parameters parser.
			Parameters order :
				-# function name
				-# function parameters (query string format)
				-# action name (blank if no action to do)
				-# action parameters (query string format)
			*/
			void storeParameters(interfaces::ValueElementList& vel);
			std::string getValue(const interfaces::ParametersVector& parameters,interfaces::VariablesMap& variables,  const void* rootObject = NULL, const server::Request* request = NULL) const;
			~HtmlFormInterfaceElement();
		};
	}
}

#endif // SYNTHESE_HtmlFormInterfaceElement_H__

