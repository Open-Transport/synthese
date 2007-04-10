
/** FormattedNumberInterfaceElement class header.
	@file FormattedNumberInterfaceElement.h

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

#ifndef SYNTHESE_FormattedNumberInterfaceElement_H__
#define SYNTHESE_FormattedNumberInterfaceElement_H__

#include "11_interfaces/LibraryInterfaceElement.h"
#include <string>

namespace synthese
{
	namespace interfaces
	{
		class ValueInterfaceElement;

		/** Formatted number interface element.
			@ingroup m11Values refValues
		*/
		class FormattedNumberInterfaceElement : public ValueInterfaceElement
		{
		public:
			static const std::string TYPE_CHAR_2;
			static const std::string TYPE_IDENTICAL;

		private:
			boost::shared_ptr<interfaces::ValueInterfaceElement> _numberVIE;
			boost::shared_ptr<interfaces::ValueInterfaceElement> _formatVIE;
			boost::shared_ptr<interfaces::ValueInterfaceElement> _numberToAdd;

		public:
			std::string	getValue(
				const ParametersVector&
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL ) const;
			void storeParameters(ValueElementList& vel);
		};

	}
}

#endif // SYNTHESE_FormattedNumberInterfaceElement_H__
