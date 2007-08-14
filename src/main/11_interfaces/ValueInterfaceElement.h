
/** ValueInterfaceElement class header.
	@file ValueInterfaceElement.h

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

#ifndef SYNTHESE_ValueInterfaceElement_H__
#define SYNTHESE_ValueInterfaceElement_H__

#include <string>

#include <boost/shared_ptr.hpp>

#include "01_util/Factorable.h"

#include "11_interfaces/Types.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace interfaces
	{
		class ValueElementList;
		class InterfacePage;

		/** @defgroup refValues Returned value Interface Elements.
			@ingroup refInt
		*/

		/** Code defined interface element which produces a string value at runtime.
			@ingroup m11
		*/
		class ValueInterfaceElement : public util::Factorable
		{
		protected:
			boost::shared_ptr<const InterfacePage>	_page;

		public:
			//! \name Setters and modifiers
			//@{
				void				setPage(boost::shared_ptr<const InterfacePage> page);
				virtual void		storeParameters(ValueElementList& vel) = 0;
			//@}

			//! \name Evaluation methods
			//@{
				virtual std::string	getValue(
					const ParametersVector&
					, interfaces::VariablesMap& variables
					, const void* object = NULL
					, const server::Request* request = NULL
					) const = 0;
				
				bool isZero(
					const ParametersVector&
					, interfaces::VariablesMap& variables
					, const void* object = NULL
					, const server::Request* request = NULL
					) const;
			//@}
		};
	}
}

#endif // SYNTHESE_ValueInterfaceElement_H__
