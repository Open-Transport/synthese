
/** DeparturesTableRowServiceNumberInterfaceElement class implementation.
	@file DeparturesTableRowServiceNumberInterfaceElement.cpp

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

#include "DeparturesTableTypes.h"
#include "DeparturesTableRowServiceNumberInterfaceElement.h"
#include "Service.h"
#include "ValueElementList.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;

	template<> const string util::FactorableTemplate<LibraryInterfaceElement, departurestable::DeparturesTableRowServiceNumberInterfaceElement>::FACTORY_KEY("departurestableservicenumbercell");

	namespace departurestable
	{
		void DeparturesTableRowServiceNumberInterfaceElement::storeParameters(ValueElementList& vel)
		{
		}

		string DeparturesTableRowServiceNumberInterfaceElement::display(
			std::ostream& stream
			, const interfaces::ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/ ) const
		{
			const ArrivalDepartureRow* row(static_cast<const ArrivalDepartureRow*>(object));

			stream << row->first.servicePointer.getService()->getServiceNumber();

			return string();
		}

		DeparturesTableRowServiceNumberInterfaceElement::~DeparturesTableRowServiceNumberInterfaceElement()
		{
		}
	}
}
