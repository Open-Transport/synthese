
/** DeparturesTableInterfaceElement class header.
	@file DeparturesTableInterfaceElement.h

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

#ifndef SYNTHESE_DeparturesTableInterfaceElement_H__
#define SYNTHESE_DeparturesTableInterfaceElement_H__

#include <string>

#include "11_interfaces/LibraryInterfaceElement.h"

namespace synthese
{
    namespace interfaces
    {
	class ValueInterfaceElement;
	
    }


	namespace departurestable
	{

		/** Departure table based on a display screen definition.
			@ingroup m34
			@todo Verify the documentation

			Displays : The defined departure table.
			Parameters :
				-# Row multiplier (???)
				-# Displayed pages : {intermediate|destination|normal} :
					- intermediate : Show one intermediate stop per page
					- destination : Show one page with the destination
					- normal : Don't handle pages
				-# Separator between pages
				-# Number of departures to hide (default = 0)
		*/
		class DeparturesTableInterfaceElement : public interfaces::LibraryInterfaceElement
		{
		public:
			static const std::string VALUE_NORMAL;
			static const std::string VALUE_INTERMEDIATE;
			static const std::string VALUE_DESTINATION;

		private:
			interfaces::ValueInterfaceElement* _multiplicateurRangeeVIE;
			interfaces::ValueInterfaceElement* _pagesVIE;
			interfaces::ValueInterfaceElement* _pageSeparator;
			interfaces::ValueInterfaceElement* _departuresToHide;
			interfaces::ValueInterfaceElement* _message;

		public:
			DeparturesTableInterfaceElement();
			~DeparturesTableInterfaceElement();

			void storeParameters(interfaces::ValueElementList& vel);
			std::string display(std::ostream& stream, const interfaces::ParametersVector& parameters, interfaces::VariablesMap& variables, const void* object = NULL, const server::Request* request = NULL) const;
			const std::string getLabel(const interfaces::ParametersVector& parameters) const;
		};

	}
}

#endif // SYNTHESE_DeparturesTableInterfaceElement_H__
