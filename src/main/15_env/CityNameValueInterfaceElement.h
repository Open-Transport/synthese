
/** CityNameValueInterfaceElement class header.
	@file CityNameValueInterfaceElement.h

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

#ifndef SYNTHESE_CityNameValueInterfaceElement_H__
#define SYNTHESE_CityNameValueInterfaceElement_H__

#include "01_util/UId.h"

#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		class ValueElementList;
	}
	namespace env
	{
		/** City Name.
			@ingroup m15Values refValues
		*/
		class CityNameValueInterfaceElement : public interfaces::ValueInterfaceElement
		{
		private:
			boost::shared_ptr<interfaces::ValueInterfaceElement> _uid;

		public:
			std::string getValue(
				const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL, const server::Request* request = NULL) const;

			/** Parser.
				@param text Optional parameter :
					- if defined : the uid of the city to name
					- if empty : the name of the city provided at runtime as current object
			*/
			void storeParameters(interfaces::ValueElementList& vel);
		};
	}
}
#endif // SYNTHESE_CityNameValueInterfaceElement_H__

