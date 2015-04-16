
/** SuperiorValueInterfaceElement class header.
	@file SuperiorValueInterfaceElement.h

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

#ifndef SYNTHESE_SuperiorValueInterfaceElement_H__
#define SYNTHESE_SuperiorValueInterfaceElement_H__

#include "11_interfaces/LibraryInterfaceElement.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace interfaces
	{
		/** Superior operator value interface element class.
			@ingroup m11Library refLibrary

			The comparison is numerical only.
		*/
		class SuperiorValueInterfaceElement
			: public util::FactorableTemplate<interfaces::LibraryInterfaceElement, SuperiorValueInterfaceElement>
		{
		private:
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _left;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _right;

		public:

			std::string display(
				std::ostream&
				, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL) const;

			void storeParameters(interfaces::ValueElementList& vel);
		};
	}
}

#endif // SYNTHESE_SuperiorValueInterfaceElement_H__
