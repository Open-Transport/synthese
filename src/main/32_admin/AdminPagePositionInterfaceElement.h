
/** AdminPagePositionInterfaceElement class header.
	@file AdminPagePositionInterfaceElement.h

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

#ifndef SYNTHESE_AdminPagePositionInterfaceElement_H__
#define SYNTHESE_AdminPagePositionInterfaceElement_H__

#include <string>

#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace admin
	{
		class AdminRequest;

		/** Position of current admin page in the tree.
			@ingroup m32
		*/
		class AdminPagePositionInterfaceElement : public interfaces::ValueInterfaceElement
		{
			// List of parameters to store
			//interfaces::ValueInterfaceElement* _parameter1;
			//...

			static std::string getUpPages(const AdminInterfaceElement* page, const server::FunctionRequest<admin::AdminRequest>* request, bool isFirst = true);

		public:
			/** Controls and store the internals parameters.
			@param vel Parameters list to read
			*/
			void storeParameters(interfaces::ValueElementList& vel);

			std::string getValue(const interfaces::ParametersVector&, interfaces::VariablesMap& variables, const void* object = NULL, const server::Request* request = NULL ) const;

			~AdminPagePositionInterfaceElement();
		};
	}
}

#endif // SYNTHESE_AdminPagePositionInterfaceElement_H__

