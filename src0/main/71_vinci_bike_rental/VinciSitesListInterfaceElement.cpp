
/** VinciSitesListInterfaceElement class implementation.
	@file VinciSitesListInterfaceElement.cpp
	@author Hugues Romain
	@date 2008

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

#include "VinciSitesListInterfaceElement.h"

#include "71_vinci_bike_rental/VinciBikeRentalModule.h"
#include "71_vinci_bike_rental/VinciSiteLoginAction.h"

#include "11_interfaces/ValueElementList.h"

#include "05_html/HTMLForm.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace html;
	
	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, vinci::VinciSitesListInterfaceElement>::FACTORY_KEY("vinci_sites_field");
	}

	namespace vinci
	{
		void VinciSitesListInterfaceElement::storeParameters(ValueElementList& vel)
		{
			//_parameter1 = vel.front();
		}

		string VinciSitesListInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {

			string emptyString;
			HTMLForm f(emptyString,emptyString);
			stream << f.getSelectInput(VinciSiteLoginAction::PARAMETER_SITE_ID, VinciBikeRentalModule::GetSitesName(UNKNOWN_VALUE), static_cast<uid>(UNKNOWN_VALUE));
			return string();
		}

		VinciSitesListInterfaceElement::~VinciSitesListInterfaceElement()
		{
		}
	}
}
