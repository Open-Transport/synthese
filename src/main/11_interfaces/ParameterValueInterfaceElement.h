
/** ParameterValueInterfaceElement class header.
	@file ParameterValueInterfaceElement.h

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

#ifndef SYNTHESE_ParameterValueInterfaceElement_H__
#define SYNTHESE_ParameterValueInterfaceElement_H__


#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		/** Execution parameter reading Value Interface Element class.
			
			Usage :
			@code {{param <rank>}} @endcode

			@ingroup m11Values refValues
		*/
		class ParameterValueInterfaceElement : public ValueInterfaceElement
		{
		private:
			ValueInterfaceElement* _rank;

		public:
			std::string getValue( const ParametersVector&, interfaces::VariablesMap& variables, const void* object = NULL, const server::Request* request = NULL ) const;
			void storeParameters(ValueElementList& vel);
		};
	}
}
#endif // SYNTHESE_ParameterValueInterfaceElement_H__
