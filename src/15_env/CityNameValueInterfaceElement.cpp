
/** CityNameValueInterfaceElement class implementation.
	@file CityNameValueInterfaceElement.cpp

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

#include "CityNameValueInterfaceElement.h"

#include "15_env/City.h"

#include <string>

#include "01_util/UId.h"
#include "01_util/Conversion.h"

#include "11_interfaces/Interface.h"
#include "11_interfaces/ValueElementList.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace env;

	namespace util
	{
		template<> const std::string FactorableTemplate<LibraryInterfaceElement, CityNameValueInterfaceElement>::FACTORY_KEY = "city_name";
	}

	namespace env
	{
		string CityNameValueInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, interfaces::VariablesMap& variables
			, const void* object, const server::Request* request) const
		{
/*			if (_uid == NULL || Conversion::ToLongLong(_uid->getValue(parameters)) == 0 )
			{
*/				const City* city = (const City*) object;
				stream << city->getName();
/*			}
			else
			{
				const Environment* __Environnement = request->getSite()->getEnvironment();
				uid cityUID = Conversion::ToLongLong(_uid->getValue(parameters));

				const City* curCommune = __Environnement->getCities().get(cityUID);
				return curCommune->getName();
			}*/
			return string ();
		}

		void CityNameValueInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_uid = vel.isEmpty() ? shared_ptr<LibraryInterfaceElement>() : vel.front();
		}
	}
}
