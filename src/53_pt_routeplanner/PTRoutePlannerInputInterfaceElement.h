
/** PTRoutePlannerInputInterfaceElemet class header.
	@file TextInputInterfaceElemet.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_PTRoutePlannerInputInterfaceElemet_H__
#define SYNTHESE_PTRoutePlannerInputInterfaceElemet_H__

#include "LibraryInterfaceElement.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace ptrouteplanner
	{
		/** Public transport route planner input Interface Element Class.
			@author Hugues Romain
			@date 2007
			@ingroup m53Library refLibrary

			Parameters :
			 - 0 : field
			 - 1 : additional html code to put inside the visible field
			 - 2 : default value (optional : discovered in the URL)
			 - 3 : style of interface page when necessary

		*/
		class PTRoutePlannerInputInterfaceElement :
			public util::FactorableTemplate<interfaces::LibraryInterfaceElement,PTRoutePlannerInputInterfaceElement>
		{
			// List of parameters to store
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _field;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _value;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _html;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _parameter1;
			
		public:
			static const std::string FIELD_ORIGIN_CITY;
			static const std::string FIELD_DESTINATION_CITY;
			static const std::string FIELD_ORIGIN_PLACE;
			static const std::string FIELD_DESTINATION_PLACE;
			static const std::string FIELD_DATE_LIST;
			static const std::string FIELD_PERIOD;

			/** Controls and store the internals parameters.
				@param vel Parameters list to read
			*/
			void storeParameters(interfaces::ValueElementList& vel);
			
			/* Displays the object.
				@param stream Stream to write on
				@param parameters Runtime parameters used in the internal parameters reading
				@param variables Runtime variables
				@param object Object to read at the display
				@param request Source request
			*/
			std::string display(
				std::ostream& stream
				, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL
				) const;
				
			~PTRoutePlannerInputInterfaceElement();
		};
	}
}

#endif // SYNTHESE_TextInputInterfaceElemet_H__
