
/** VinciCurrentSiteInterfaceElement class implementation.
	@file VinciCurrentSiteInterfaceElement.cpp
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

#include "VinciCurrentSiteInterfaceElement.h"

#include "30_server/Request.h"

#include "71_vinci_bike_rental/VinciBikeRentalModule.h"
#include "71_vinci_bike_rental/VinciSite.h"
#include "71_vinci_bike_rental/VinciSiteTableSync.h"

#include "11_interfaces/ValueElementList.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	
	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, vinci::VinciCurrentSiteInterfaceElement>::FACTORY_KEY("vinci_current_site");
	}

	namespace vinci
	{
		void VinciCurrentSiteInterfaceElement::storeParameters(ValueElementList& vel)
		{
			//_parameter1 = vel.front();
		}

		string VinciCurrentSiteInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {
			
			uid siteId(VinciBikeRentalModule::GetSessionSite(request->getSession()));

			try
			{
				shared_ptr<const VinciSite> site(VinciSiteTableSync::Get(siteId));
				stream << site->getName();
			}
			catch(...)
			{

			}
			return string();
		}

		VinciCurrentSiteInterfaceElement::~VinciCurrentSiteInterfaceElement()
		{
		}
	}
}
