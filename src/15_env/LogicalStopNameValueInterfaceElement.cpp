
/** LogicalStopNameValueInterfaceElement class implementation.
	@file LogicalStopNameValueInterfaceElement.cpp

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

#include <string>

#include "Conversion.h"

#include "Interface.h"
#include "ValueElementList.h"

#include "NamedPlace.h"
#include "LogicalStopNameValueInterfaceElement.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace env;
	using namespace geography;

	namespace util
	{
		template<> const std::string FactorableTemplate<LibraryInterfaceElement, LogicalStopNameValueInterfaceElement>::FACTORY_KEY = "stop_name";
	}

	namespace env
	{
		string LogicalStopNameValueInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters, interfaces::VariablesMap& variables, const void* object, const server::Request* request) const
		{
/*			if (_uid == NULL || Conversion::ToLongLong(_uid->getValue(parameters)) == 0 )
			{
*/				const NamedPlace* place(static_cast<const NamedPlace*>(object));
				stream << place->getFullName();
/*			}
			else
			{
				const Environment* __Environnement = request->getSite()->getEnvironment();
				uid placeUID = Conversion::ToLongLong(_uid->getValue(parameters));

				const ConnectionPlace* place = __Environnement->getConnectionPlaces().get(placeUID);
				return place->getName();
			} */
			return string();
		}

		void LogicalStopNameValueInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_uid = (vel.size() >= 1) ? vel.front() : shared_ptr<LibraryInterfaceElement>();
		}
	}

}
